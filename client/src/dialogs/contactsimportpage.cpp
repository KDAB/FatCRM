/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Tobias Koenig <tobias.koenig@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "contactsimportpage.h"
#include "ui_contactsimportpage.h"

#include "filterproxymodel.h"
#include "itemstreemodel.h"

#include <AkonadiCore/ItemCreateJob>

#include <KJobUiDelegate>
#include <KLocalizedString>

#include <QButtonGroup>
#include <QBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <sugarcontactwrapper.h>

namespace {

enum SelectAction
{
    CreateNewContact = -1,
    ExcludeContact = -2
};

enum ChangedFlags
{
    IsNormal = 0,
    IsNew = 1,
    IsModified = 2
};

QString markupString(const QString &text, int flags)
{
    if (flags & IsNew)
        return QStringLiteral("<b>%1</b>").arg(text);
    else if (flags & IsModified)
        return QStringLiteral("<font color=\"#0000ff\"><b>%1</b></font>").arg(text);
    else
        return text;
}

QString formattedContact(const QString &prefix, int prefixFlags,
                         const QString &givenName, int givenNameFlags,
                         const QString &familyName, int familyNameFlags,
                         const QString &jobTitle, int jobTitleFlags,
                         const QString &emailAddress, int emailAddressFlags,
                         const QString &phoneNumber, int phoneNumberFlags,
                         const QString &address, int addressFlags)
{
    QStringList parts;

    // First line

    if (!prefix.isEmpty())
        parts << markupString(prefix, prefixFlags);

    if (!givenName.isEmpty())
        parts << markupString(givenName, givenNameFlags);

    if (!familyName.isEmpty())
        parts << markupString(familyName, familyNameFlags);

    if (!emailAddress.isEmpty())
        parts << markupString(QStringLiteral("&lt;%1&gt;").arg(emailAddress), emailAddressFlags);

    // Other lines below

    if (!jobTitle.isEmpty()) {
        if (!parts.isEmpty())
            parts << QStringLiteral("<br/>");
        parts << markupString(jobTitle, jobTitleFlags);
    }

    if (!phoneNumber.isEmpty()) {
        if (!parts.isEmpty())
            parts << QStringLiteral("<br/>");
        parts << markupString(phoneNumber, phoneNumberFlags);
    }

    if (!address.isEmpty()) {
        if (!parts.isEmpty())
            parts << QStringLiteral("<br/>");
        parts << markupString(address, addressFlags);
    }

    return parts.join(QStringLiteral(" "));
}

// For one-line descriptions
QString formattedContact(const KContacts::Addressee &addressee, bool withAddress = false)
{
    QStringList parts;

    if (!addressee.givenName().isEmpty())
        parts << addressee.givenName();
    else
        parts << i18n("<missing first name>");

    if (!addressee.familyName().isEmpty())
        parts << addressee.familyName();
    else
        parts << i18n("<missing last name>");

    if (!addressee.preferredEmail().isEmpty()) {
        if (parts.isEmpty())
            parts << addressee.preferredEmail();
        parts << QStringLiteral("<%1>").arg(addressee.preferredEmail());
    }

    if (withAddress) {
        const QString id = SugarContactWrapper(addressee).accountId();
        const SugarAccount account = AccountRepository::instance()->accountById(id);

        QStringList addressParts;
        if (!account.name().isEmpty())
            addressParts << account.name();

        if (!account.cityForGui().isEmpty())
            addressParts << account.cityForGui();

        if (!account.countryForGui().isEmpty())
            addressParts << account.countryForGui();

        if (!addressParts.isEmpty()) {
            parts << QStringLiteral("(%1)").arg(addressParts.join(QStringLiteral(", ")));
        }
    }

    return parts.join(QStringLiteral(" "));
}

QString formattedAddress(const KContacts::Address &address)
{
    QStringList parts;
    if (!address.street().isEmpty())
        parts << address.street();
    if (!address.locality().isEmpty())
        parts << address.locality();
    if (!address.postalCode().isEmpty())
        parts << address.postalCode();
    if (!address.country().isEmpty())
        parts << address.country();

    return parts.join(QStringLiteral(", "));
}

} // namespace

MergeWidget::UpdateCheckBoxes::UpdateCheckBoxes()
{
}

QCheckBox *MergeWidget::addFieldCheckBox(const QString &str)
{
    auto *checkBox = new QCheckBox;
    QString text(str);
    checkBox->setText(text.replace('&', QLatin1String("&&")));
    mImportedContactLayout->addWidget(checkBox);
    checkBox->setChecked(true);
    connect(checkBox, &QAbstractButton::toggled, this, &MergeWidget::updateFinalContact);

    return checkBox;
}

MergeWidget::MergeWidget(const SugarAccount &account, const KContacts::Addressee &importedAddressee,
                         const QVector<MatchPair> &possibleMatches, QWidget *parent)
    : QWidget(parent)
    , mAccount(account)
    , mImportedAddressee(importedAddressee)
    , mPossibleMatches(possibleMatches)
{
    auto *mainLayout = new QHBoxLayout(this);

    auto *importedContactBox = new QGroupBox;
    auto *matchingContactsBox = new QGroupBox;
    auto *finalContactBox = new QGroupBox;

    mainLayout->addWidget(importedContactBox, 1);
    mainLayout->addWidget(matchingContactsBox, 2);
    mainLayout->addWidget(finalContactBox, 1);

    importedContactBox->setFixedWidth(350);
    finalContactBox->setFixedWidth(350);

    importedContactBox->setTitle(i18n("Imported Contact"));
    matchingContactsBox->setTitle(i18n("Matching Contacts"));
    finalContactBox->setTitle(i18n("Final Contact"));

    // imported contact box content
    mImportedContactLayout = new QVBoxLayout(importedContactBox);
    mImportedContactLabel = new QLabel;
    mImportedContactLabel->setWordWrap(true);
    mImportedContactLayout->addWidget(mImportedContactLabel);

    SugarContactWrapper contactWrapper(mImportedAddressee);
    contactWrapper.setAccountId(mAccount.id());
    mImportedContactLabel->setText(formattedContact(mImportedAddressee, true));

    if (!contactWrapper.salutation().isEmpty()) {
        mUpdateCheckBoxes.prefix = addFieldCheckBox(contactWrapper.salutation());
    }

    if (!mImportedAddressee.givenName().isEmpty()) {
        mUpdateCheckBoxes.givenName = addFieldCheckBox(mImportedAddressee.givenName());
    }

    if (!mImportedAddressee.familyName().isEmpty()) {
        mUpdateCheckBoxes.familyName = addFieldCheckBox(mImportedAddressee.familyName());
    }

    if (!mImportedAddressee.title().isEmpty()) {
        mUpdateCheckBoxes.jobTitle = addFieldCheckBox(mImportedAddressee.title());
    }

    if (!mImportedAddressee.preferredEmail().isEmpty()) {
        mUpdateCheckBoxes.emailAddress = addFieldCheckBox(mImportedAddressee.preferredEmail());
    }

    if (!mImportedAddressee.phoneNumber(KContacts::PhoneNumber::Work).number().isEmpty()) {
        const QString str = mImportedAddressee.phoneNumber(KContacts::PhoneNumber::Work).number();
        mUpdateCheckBoxes.phoneNumber = addFieldCheckBox(str);
    }

    mImportedContactLayout->addStretch();

    // matching contacts box content
    auto *matchingContactsLayout = new QVBoxLayout(matchingContactsBox);
    mButtonGroup = new QButtonGroup(this);
    mButtonGroup->setExclusive(true);
    connect(mButtonGroup, SIGNAL(buttonClicked(int)), SLOT(updateFinalContact()));

    for (int i = 0; i < mPossibleMatches.count(); ++i) {
        const KContacts::Addressee &possibleMatch = mPossibleMatches.at(i).contact;
        auto *button = new QRadioButton;
        button->setProperty("__index", i);
        button->setText(i18n("Update %1", formattedContact(possibleMatch, true)));
        mButtonGroup->addButton(button);
        matchingContactsLayout->addWidget(button);
        button->setChecked(i == 0);
    }

    {
        auto *button = new QRadioButton;
        button->setProperty("__index", static_cast<int>(CreateNewContact));
        button->setText(i18n("Create new contact"));
        button->setChecked(mPossibleMatches.isEmpty());
        mButtonGroup->addButton(button);
        matchingContactsLayout->addWidget(button);
    }

    {
        auto *button = new QRadioButton;
        button->setProperty("__index", static_cast<int>(ExcludeContact));
        button->setText(i18n("Cancel importing this contact"));
        mButtonGroup->addButton(button);
        matchingContactsLayout->addWidget(button);
    }

    matchingContactsLayout->addStretch();

    // final contact box content
    auto *finalContactLayout = new QVBoxLayout(finalContactBox);
    mFinalContactLabel = new QLabel;
    mFinalContactLabel->setWordWrap(true);
    finalContactLayout->addWidget(mFinalContactLabel);
    finalContactLayout->addStretch();

    updateFinalContact();
}

bool MergeWidget::skipItem() const
{
    const QAbstractButton *button = mButtonGroup->checkedButton();
    const int index = button->property("__index").toInt();

    return (index == ExcludeContact);
}

static QString assemble(const QString &old, const QString &addition) {
    QString ret = old;
    if (!addition.isEmpty()) {
        if (!ret.isEmpty() && !ret.endsWith('\n'))
            ret += '\n';
        ret += addition;
    }
    return ret;
}

static const char s_descriptionMarker[] = "COMMON_DESCRIPTION";

Akonadi::Item MergeWidget::finalItem(const QString &descriptionText)
{
    const QAbstractButton *button = mButtonGroup->checkedButton();
    const int index = button->property("__index").toInt();

    QString note = mFinalContact.note();
    Q_ASSERT(note.contains(QLatin1String(s_descriptionMarker)));
    // Add the description that applies to all contacts
    if (note.contains(descriptionText)) {
        note.remove(QLatin1String(s_descriptionMarker) + '\n'); // in case it's followed by a newline
        note.remove(QLatin1String(s_descriptionMarker));        // just in case it's not
    } else {
        QString commonDescription = descriptionText;
        if (!commonDescription.isEmpty()) {
            commonDescription += '\n';
        }
        note.replace(QLatin1String(s_descriptionMarker), commonDescription);
    }
    mFinalContact.setNote(note);

    if (index == ExcludeContact) {
        Q_ASSERT(false); // should never be called
        return Akonadi::Item();
    } else if (index == CreateNewContact) {
        Akonadi::Item item;
        item.setMimeType(KContacts::Addressee::mimeType());
        item.setPayload(mFinalContact);
        return item;
    } else {
        Akonadi::Item item = mPossibleMatches.at(index).item;
        item.setPayload(mFinalContact);
        return item;
    }
}

void MergeWidget::updateFinalContact()
{
    const QAbstractButton *button = mButtonGroup->checkedButton();
    const int index = button->property("__index").toInt();

    int prefixFlags = IsNormal;
    int givenNameFlags = IsNormal;
    int familyNameFlags = IsNormal;
    int emailAddressFlags = IsNormal;
    int phoneNumberFlags = IsNormal;
    int addressFlags = IsNormal;
    int jobTitleFlags = IsNormal;

    const SugarContactWrapper importedContactWrapper(mImportedAddressee);
    SugarContactWrapper finalContactWrapper(mFinalContact);

    if (index == ExcludeContact) {
        // do nothing
    } else if (index == CreateNewContact) {
        mFinalContact = KContacts::Addressee();
        mFinalContact.setNote(assemble(QLatin1String(s_descriptionMarker), mImportedAddressee.note())); // marker for lineedit description, followed by note coming from the CSV

        if (mUpdateCheckBoxes.prefix && mUpdateCheckBoxes.prefix->isChecked()) {
            prefixFlags = IsNew;
            finalContactWrapper.setSalutation(importedContactWrapper.salutation());
        }

        if (mUpdateCheckBoxes.givenName && mUpdateCheckBoxes.givenName->isChecked()) {
            givenNameFlags = IsNew;
            mFinalContact.setGivenName(mImportedAddressee.givenName());
        }

        if (mUpdateCheckBoxes.familyName && mUpdateCheckBoxes.familyName->isChecked()) {
            familyNameFlags = IsNew;
            mFinalContact.setFamilyName(mImportedAddressee.familyName());
        }

        if (mUpdateCheckBoxes.jobTitle && mUpdateCheckBoxes.jobTitle->isChecked()) {
            jobTitleFlags = IsNew;
            mFinalContact.setTitle(mImportedAddressee.title());
        }

        if (mUpdateCheckBoxes.emailAddress && mUpdateCheckBoxes.emailAddress->isChecked()) {
            emailAddressFlags = IsNew;
            mFinalContact.insertEmail(mImportedAddressee.preferredEmail(), true);
        }

        if (mUpdateCheckBoxes.phoneNumber && mUpdateCheckBoxes.phoneNumber->isChecked()) {
            phoneNumberFlags = IsNew;
            mFinalContact.insertPhoneNumber(mImportedAddressee.phoneNumber(KContacts::PhoneNumber::Work));
        }

        KContacts::Address address(KContacts::Address::Work | KContacts::Address::Pref);
        if (!mAccount.shippingAddressStreet().isEmpty()) {
            address.setStreet(mAccount.shippingAddressStreet());
            address.setLocality(mAccount.shippingAddressCity());
            address.setRegion(mAccount.shippingAddressState());
            address.setPostalCode(mAccount.shippingAddressPostalcode());
            address.setCountry(mAccount.shippingAddressCountry());
        } else {
            address.setStreet(mAccount.billingAddressStreet());
            address.setLocality(mAccount.billingAddressCity());
            address.setRegion(mAccount.billingAddressState());
            address.setPostalCode(mAccount.billingAddressPostalcode());
            address.setCountry(mAccount.billingAddressCountry());
        }

        addressFlags = IsNew;
        mFinalContact.removeAddress(mFinalContact.address(KContacts::Address::Work|KContacts::Address::Pref));
        mFinalContact.insertAddress(address);

        finalContactWrapper.setAccountId(mAccount.id());
    } else {
        mFinalContact = mPossibleMatches.at(index).contact;

        mFinalContact.setNote(assemble(mFinalContact.note(), QLatin1String(s_descriptionMarker))); // add marker for the lineedit description
        mFinalContact.setNote(assemble(mFinalContact.note(), mImportedAddressee.note())); // add description coming from the CSV

        if (mUpdateCheckBoxes.prefix && mUpdateCheckBoxes.prefix->isChecked()) {
            if (finalContactWrapper.salutation() != importedContactWrapper.salutation()) {
                prefixFlags = (mFinalContact.prefix().isEmpty() ? IsNew : IsModified);
                finalContactWrapper.setSalutation(importedContactWrapper.salutation());
            }
        }

        if (mUpdateCheckBoxes.givenName && mUpdateCheckBoxes.givenName->isChecked()) {
            if (mFinalContact.givenName() != mImportedAddressee.givenName()) {
                givenNameFlags = (mFinalContact.givenName().isEmpty() ? IsNew : IsModified);
                mFinalContact.setGivenName(mImportedAddressee.givenName());
            }
        }

        if (mUpdateCheckBoxes.familyName && mUpdateCheckBoxes.familyName->isChecked()) {
            if (mFinalContact.familyName() != mImportedAddressee.familyName()) {
                familyNameFlags = (mFinalContact.familyName().isEmpty() ? IsNew : IsModified);
                mFinalContact.setFamilyName(mImportedAddressee.familyName());
            }
        }

        if (mUpdateCheckBoxes.jobTitle && mUpdateCheckBoxes.jobTitle->isChecked()) {
            if (mFinalContact.title() != mImportedAddressee.title()) {
                jobTitleFlags = (mFinalContact.title().isEmpty() ? IsNew : IsModified);
                mFinalContact.setTitle(mImportedAddressee.title());
            }
        }

        if (mUpdateCheckBoxes.emailAddress && mUpdateCheckBoxes.emailAddress->isChecked()) {
            if (QString::compare(mFinalContact.preferredEmail(), mImportedAddressee.preferredEmail(), Qt::CaseInsensitive) != 0) {
                emailAddressFlags = (mFinalContact.preferredEmail().isEmpty() ? IsNew : IsModified);
                mFinalContact.setEmails(QStringList());
                mFinalContact.insertEmail(mImportedAddressee.preferredEmail(), true);
            }
        }

        if (mUpdateCheckBoxes.phoneNumber && mUpdateCheckBoxes.phoneNumber->isChecked()) {
            if (mFinalContact.phoneNumber(KContacts::PhoneNumber::Work).number() != mImportedAddressee.phoneNumber(KContacts::PhoneNumber::Work).number()) {
                phoneNumberFlags = (mFinalContact.phoneNumber(KContacts::PhoneNumber::Work).number().isEmpty() ? IsNew : IsModified);
                mFinalContact.removePhoneNumber(mFinalContact.phoneNumber(KContacts::PhoneNumber::Work));
                mFinalContact.insertPhoneNumber(mImportedAddressee.phoneNumber(KContacts::PhoneNumber::Work));
            }
        }
    }

    if (index == ExcludeContact) {
        mFinalContactLabel->setText(QString());
    } else {
        mFinalContactLabel->setText(formattedContact(finalContactWrapper.salutation(), prefixFlags,
                                                     mFinalContact.givenName(), givenNameFlags,
                                                     mFinalContact.familyName(), familyNameFlags,
                                                     mFinalContact.title(), jobTitleFlags,
                                                     mFinalContact.preferredEmail(), emailAddressFlags,
                                                     mFinalContact.phoneNumber(KContacts::PhoneNumber::Work).number(), phoneNumberFlags,
                                                     formattedAddress(mFinalContact.address(KContacts::Address::Work|KContacts::Address::Pref)), addressFlags));
    }
}


ContactsImportPage::ContactsImportPage(QWidget *parent)
    : QWizardPage(parent)
    , mUi(new Ui::ContactsImportPage)
    , mContactsModel(new FilterProxyModel(DetailsType::Contact, this))
{
    mUi->setupUi(this);
}

ContactsImportPage::~ContactsImportPage()
{
    delete mUi;
}

void ContactsImportPage::setContactsModel(ItemsTreeModel *model)
{
    mContactsModel->setSourceModel(model);
}

void ContactsImportPage::cleanup()
{
}

bool ContactsImportPage::validatePage()
{
    QVector<Akonadi::Item> items;

    foreach (MergeWidget *mergeWidget, findChildren<MergeWidget*>()) {
        if (!mergeWidget->skipItem())
            items << mergeWidget->finalItem(mUi->lineEdit->text());
    }

    emit importedItems(items);

    return true;
}

bool ContactsImportPage::openContactsAfterImport() const
{
    return mUi->cbOpenContacts->isChecked();
}

void ContactsImportPage::setChosenContacts(const QVector<ContactsSet> &contacts)
{
    {
        QLayoutItem *layoutItem = nullptr;
        while ((layoutItem = mUi->mainLayout->takeAt(0)) != nullptr) {
            delete layoutItem->widget();
            delete layoutItem;
        }
    }

    foreach (const ContactsSet &contactsSet, contacts) {
        foreach (const KContacts::Addressee &addressee, contactsSet.addressees) {
            QVector<MatchPair> matchByEmail;
            QVector<MatchPair> matchByName;

            for (int row = 0; row < mContactsModel->rowCount(); ++row) {
                const QModelIndex index = mContactsModel->index(row, 0);
                const Akonadi::Item item = index.data(Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();
                Q_ASSERT(item.hasPayload<KContacts::Addressee>());
                const KContacts::Addressee possibleMatch = item.payload<KContacts::Addressee>();
                if (possibleMatch.preferredEmail() == addressee.preferredEmail()) {
                    MatchPair pair;
                    pair.contact = possibleMatch;
                    pair.item = item;
                    matchByEmail.append(pair);
                } else if (possibleMatch.givenName() == addressee.givenName() &&
                           possibleMatch.familyName() == addressee.familyName() &&
                           !addressee.givenName().isEmpty() && !addressee.familyName().isEmpty()) {
                    MatchPair pair;
                    pair.contact = possibleMatch;
                    pair.item = item;
                    matchByName.append(pair);
                }
            }

            QVector<MatchPair> allMatches;
            allMatches << matchByEmail;
            allMatches << matchByName;

            addMergeWidget(contactsSet.account, addressee, allMatches);
        }
    }

    QMetaObject::invokeMethod(this, "adjustPageSize", Qt::QueuedConnection);
}

void ContactsImportPage::adjustPageSize()
{
    setMinimumWidth(mUi->scrollArea->widget()->width() + 40);
    emit layoutChanged();
}

void ContactsImportPage::addMergeWidget(const SugarAccount &account, const KContacts::Addressee &importedAddressee,
                                        const QVector<MatchPair> &possibleMatches)
{
    auto *widget = new MergeWidget(account, importedAddressee, possibleMatches);
    mUi->mainLayout->addWidget(widget);
}
