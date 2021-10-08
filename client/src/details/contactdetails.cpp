/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

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

#include "contactdetails.h"

#include "accountrepository.h"
#include "contactdataextractor.h"
#include "editcalendarbutton.h"
#include "itemstreemodel.h"
#include "linkeditemsrepository.h"
#include "modelrepository.h"
#include "noteswindow.h"
#include "referenceddatamodel.h"
#include "selectitemdialog.h"
#include "ui_contactdetails.h"
#include "fatcrm_client_debug.h"

#include "kdcrmutils.h"
#include "kdcrmfields.h"

#include <KContacts/Address>
#include <KContacts/Addressee>

#include <KLocalizedString>

#include <QCompleter>
#include <QDesktopServices>
#include <QMessageBox>
#include <sugarcontactwrapper.h>

ContactDetails::ContactDetails(QWidget *parent)
    : Details(DetailsType::Contact, parent), mUi(new Ui::ContactDetails), mDataExtractor(new ContactDataExtractor)
{
    mUi->setupUi(this);
    mUi->urllabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

    mUi->salutation->setObjectName(KDCRMFields::salutation());
    mUi->first_name->setObjectName(KDCRMFields::firstName());
    mUi->last_name->setObjectName(KDCRMFields::lastName());
    mUi->title->setObjectName(KDCRMFields::title());
    mUi->department->setObjectName(KDCRMFields::department());
    mUi->account_id->setObjectName(KDCRMFields::accountId());
    mUi->lead_source->setObjectName(KDCRMFields::leadSource());
    mUi->reports_to_id->setObjectName(KDCRMFields::reportsToId());
    mUi->email1->setObjectName(KDCRMFields::email1());
    mUi->email2->setObjectName(KDCRMFields::email2());
    mUi->assigned_user_id->setObjectName(KDCRMFields::assignedUserId());
    mUi->phone_work->setObjectName(KDCRMFields::phoneWork());
    mUi->phone_mobile->setObjectName(KDCRMFields::phoneMobile());
    mUi->phone_home->setObjectName(KDCRMFields::phoneHome());
    mUi->phone_fax->setObjectName(KDCRMFields::phoneFax());
    mUi->phone_other->setObjectName(KDCRMFields::phoneOther());
    mUi->do_not_call->setObjectName(KDCRMFields::doNotCall());
    // Proper support for "invalid email" for each email address would require using get_relationships...
    mUi->invalid_email->setObjectName(KDCRMFields::invalidEmail());
    mUi->birthdate->setObjectName(KDCRMFields::birthdate());
    mUi->assistant->setObjectName(KDCRMFields::assistant());
    mUi->phoneAssistant->setObjectName(KDCRMFields::phoneAssistant());
    mUi->primaryAddressStreet->setObjectName(KDCRMFields::primaryAddressStreet());
    mUi->primaryAddressCity->setObjectName(KDCRMFields::primaryAddressCity());
    mUi->primaryAddressState->setObjectName(KDCRMFields::primaryAddressState());
    mUi->primaryAddressPostalcode->setObjectName(KDCRMFields::primaryAddressPostalcode());
    mUi->primaryAddressCountry->setObjectName(KDCRMFields::primaryAddressCountry());
    mUi->altAddressStreet->setObjectName(KDCRMFields::altAddressStreet());
    mUi->altAddressCity->setObjectName(KDCRMFields::altAddressCity());
    mUi->altAddressState->setObjectName(KDCRMFields::altAddressState());
    mUi->altAddressPostalcode->setObjectName(KDCRMFields::altAddressPostalcode());
    mUi->altAddressCountry->setObjectName(KDCRMFields::altAddressCountry());

    QCompleter *countriesCompleter = createCountriesCompleter();
    mUi->altAddressCountry->setCompleter(countriesCompleter);
    mUi->primaryAddressCountry->setCompleter(countriesCompleter);

    initialize();
}

ContactDetails::~ContactDetails()
{
    delete mUi;
}

void ContactDetails::setLinkedItemsRepository(LinkedItemsRepository *repo)
{
    mLinkedItemsRepository = repo;
    connect(mLinkedItemsRepository, &LinkedItemsRepository::contactModified,
            this, &ContactDetails::slotLinkedItemsModified);
}

void ContactDetails::initialize()
{
    ReferencedDataModel::setModelForCombo(mUi->account_id, AccountRef);
    //ReferencedDataModel::setModelForCombo(mUi->campaign, CampaignRef);
    ReferencedDataModel::setModelForCombo(mUi->reports_to_id, ContactRef);
    ReferencedDataModel::setModelForCombo(mUi->assigned_user_id, AssignedToRef);

    connect(mUi->calendarButton->calendarWidget(), &QCalendarWidget::clicked,
            this, &ContactDetails::slotSetBirthday);
    connect(mUi->account_id, SIGNAL(activated(int)), this, SLOT(slotAccountActivated()));
    connect(mUi->buttonSelectAccount, &QAbstractButton::clicked, this, &ContactDetails::slotSelectAccount);

    connect(mUi->primaryAddressCountry, &QLineEdit::editingFinished, this, &ContactDetails::slotPrimaryAddressCountryEditingFinished);
    connect(mUi->altAddressCountry, &QLineEdit::editingFinished, this, &ContactDetails::slotOtherAddressCountryEditingFinished);
    connect(mUi->copyPrimaryAddressButton, &QPushButton::clicked, this, [this]() {
        copyAddressFromGroup(mUi->primaryAddressGroupBox);
    });
    connect(mUi->copyOtherAddressButton, &QPushButton::clicked, this, [this]() {
        copyAddressFromGroup(mUi->otherAddressGroupBox);
    });
}

ItemDataExtractor *ContactDetails::itemDataExtractor() const
{
    return mDataExtractor.get();
}

void ContactDetails::slotSetBirthday()
{
    // TODO FIXME: use QDateEdit [or remove - who cares about birthdays...]
    mUi->birthdate->setText(KDCRMUtils::dateToString(mUi->calendarButton->calendarWidget()->selectedDate()));
    mUi->calendarButton->calendarWidget()->setSelectedDate(QDate::currentDate());
    mUi->calendarButton->calendarDialog()->close();
}

void ContactDetails::slotAccountActivated()
{
    const QString accountId = currentAccountId();
    if (accountId.isEmpty()) {
        return;
    }
    const SugarAccount account = AccountRepository::instance()->accountById(accountId);
    if (account.isEmpty()) {
        return;
    }

    const bool copyAddressFromAccount = (mUi->primaryAddressStreet->toPlainText().isEmpty() &&
                                        mUi->primaryAddressCity->text().isEmpty() &&
                                        mUi->primaryAddressState->text().isEmpty() &&
                                        mUi->primaryAddressPostalcode->text().isEmpty() &&
                                        mUi->primaryAddressCountry->text().isEmpty());

    const bool copyPhoneWorkFromAccount = mUi->phone_work->text().isEmpty();

    if (!copyAddressFromAccount && !copyPhoneWorkFromAccount) {
        return;
    }

    QString title, message;

    if (copyAddressFromAccount && !copyPhoneWorkFromAccount) {
        title = i18n("Copy address from account?");
        message = i18n("Do you want to copy the address from the account '%1' into this contact?", account.name());
    } else if (!copyAddressFromAccount && copyPhoneWorkFromAccount) {
        title = i18n("Copy phone from account?");
        message = i18n("Do you want to copy the phone number from the account '%1' into this contact?", account.name());
    } else {
        title = i18n("Copy address and phone from account?");
        message = i18n("Do you want to copy the address and phone number from the account '%1' into this contact?", account.name());
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Yes |
                              QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Cancel) {
        return;
    }

    if (copyAddressFromAccount) {
        if (!account.shippingAddressStreet().isEmpty()) {
            mUi->primaryAddressStreet->setPlainText(account.shippingAddressStreet());
            mUi->primaryAddressCity->setText(account.shippingAddressCity());
            mUi->primaryAddressState->setText(account.shippingAddressState());
            mUi->primaryAddressPostalcode->setText(account.shippingAddressPostalcode());
            mUi->primaryAddressCountry->setText(KDCRMUtils::canonicalCountryName(account.shippingAddressCountry()));
        } else {
            mUi->primaryAddressStreet->setPlainText(account.billingAddressStreet());
            mUi->primaryAddressCity->setText(account.billingAddressCity());
            mUi->primaryAddressState->setText(account.billingAddressState());
            mUi->primaryAddressPostalcode->setText(account.billingAddressPostalcode());
            mUi->primaryAddressCountry->setText(KDCRMUtils::canonicalCountryName(account.billingAddressCountry()));
        }
    }

    if (copyPhoneWorkFromAccount) {
        mUi->phone_work->setText(account.phoneOffice());
    }
}

QMap<QString, QString> ContactDetails::data(const Akonadi::Item &item) const
{
    Q_ASSERT(item.hasPayload<KContacts::Addressee>());
    KContacts::Addressee contact = item.payload<KContacts::Addressee>();
    return contactData(contact);
}

QMap<QString, QString> ContactDetails::contactData(const KContacts::Addressee &addressee) const
{
    const SugarContactWrapper contactWrapper(addressee);
    QMap<QString, QString> data;
    data.insert(KDCRMFields::salutation(), contactWrapper.salutation());
    data.insert(KDCRMFields::firstName(), addressee.givenName());
    data.insert(KDCRMFields::lastName(), addressee.familyName());
    data.insert(KDCRMFields::title(), addressee.title());
    data.insert(KDCRMFields::department(), addressee.department());
    data.insert(KDCRMFields::accountName(), addressee.organization());
    data.insert(KDCRMFields::accountId(), contactWrapper.accountId());
    data.insert(KDCRMFields::email1(), addressee.preferredEmail());
    QStringList emails = addressee.emails();
    emails.removeAll(addressee.preferredEmail());
    if (!emails.isEmpty()) {
        data.insert(KDCRMFields::email2(), emails.at(0));
    }
    data.insert(KDCRMFields::phoneHome(), addressee.phoneNumber(KContacts::PhoneNumber::Home).number());
    data.insert(KDCRMFields::phoneMobile(), addressee.phoneNumber(KContacts::PhoneNumber::Cell).number());
    data.insert(KDCRMFields::phoneWork(), addressee.phoneNumber(KContacts::PhoneNumber::Work).number());
    data.insert(KDCRMFields::phoneOther(), addressee.phoneNumber(KContacts::PhoneNumber::Car).number());
    data.insert(KDCRMFields::phoneFax(), addressee.phoneNumber(KContacts::PhoneNumber::Fax).number());

    const KContacts::Address address = addressee.address(KContacts::Address::Work | KContacts::Address::Pref);
    data.insert(KDCRMFields::primaryAddressStreet(), address.street());
    data.insert(KDCRMFields::primaryAddressCity(), address.locality());
    data.insert(KDCRMFields::primaryAddressState(), address.region());
    data.insert(KDCRMFields::primaryAddressPostalcode(), address.postalCode());
    data.insert(KDCRMFields::primaryAddressCountry(), address.country());

    const KContacts::Address other = addressee.address(KContacts::Address::Home);
    data.insert(KDCRMFields::altAddressStreet(), other.street());
    data.insert(KDCRMFields::altAddressCity(), other.locality());
    data.insert(KDCRMFields::altAddressState(), other.region());
    data.insert(KDCRMFields::altAddressPostalcode(), other.postalCode());
    data.insert(KDCRMFields::altAddressCountry(), other.country());
    data.insert(KDCRMFields::birthdate(), KDCRMUtils::dateToString(addressee.birthday().date()));
    data.insert(KDCRMFields::assistant(), contactWrapper.assistant());
    data.insert(KDCRMFields::phoneAssistant(), contactWrapper.phoneAssistant());
    data.insert(KDCRMFields::leadSource(), contactWrapper.leadSource());
    data.insert(KDCRMFields::campaign(), contactWrapper.campaign());
    data.insert(KDCRMFields::assignedUserId(), contactWrapper.assignedUserId());
    data.insert(KDCRMFields::assignedUserName(), contactWrapper.assignedUserName());
    data.insert(KDCRMFields::reportsToId(), contactWrapper.reportsToId());
    data.insert(KDCRMFields::reportsTo(), contactWrapper.reportsTo());
    data.insert(KDCRMFields::doNotCall(), contactWrapper.doNotCall());
    data.insert(KDCRMFields::invalidEmail(), contactWrapper.invalidEmail());
    data.insert(KDCRMFields::description(), addressee.note());
    data.insert(KDCRMFields::modifiedByName(), contactWrapper.modifiedByName());
    data.insert(KDCRMFields::dateModified(), contactWrapper.dateModified());
    data.insert(KDCRMFields::dateEntered(), contactWrapper.dateEntered());
    data.insert(KDCRMFields::createdByName(), contactWrapper.createdByName());
    data.insert(KDCRMFields::modifiedUserId(), contactWrapper.modifiedUserId());
    data.insert(KDCRMFields::id(), contactWrapper.id());
    data.insert(KDCRMFields::opportunityRoleFields(), contactWrapper.opportunityRoleFields());
    data.insert(KDCRMFields::cAcceptStatusFields(), contactWrapper.cAcceptStatusFields());
    data.insert(KDCRMFields::mAcceptStatusFields(), contactWrapper.mAcceptStatusFields());
    data.insert(KDCRMFields::deleted(), contactWrapper.deleted());
    data.insert(KDCRMFields::createdBy(), contactWrapper.createdBy());
    return data;
}

QMap<QString, QString> ContactDetails::fillAddressFieldsMap(QGroupBox *box) const
{
    QMap<QString, QString> map;

    if (box == mUi->primaryAddressGroupBox) {
        map.insert("street", KDCRMFields::primaryAddressStreet());
        map.insert("city", KDCRMFields::primaryAddressCity());
        map.insert("state", KDCRMFields::primaryAddressState());
        map.insert("postalcode", KDCRMFields::primaryAddressPostalcode());
        map.insert("country", KDCRMFields::primaryAddressCountry());
    } else if (box == mUi->otherAddressGroupBox) {
        map.insert("street", KDCRMFields::altAddressStreet());
        map.insert("city", KDCRMFields::altAddressCity());
        map.insert("state", KDCRMFields::altAddressState());
        map.insert("postalcode", KDCRMFields::altAddressPostalcode());
        map.insert("country", KDCRMFields::altAddressCountry());
    }

    return map;
}

void ContactDetails::updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const
{
    KContacts::Addressee addressee;
    SugarContactWrapper contactWrapper(addressee);
    if (item.hasPayload<KContacts::Addressee>()) {
        // start from the existing item (to keep custom fields not shown in GUI, I suppose)
        addressee = item.payload<KContacts::Addressee>();
        // but for anything that uses insert... below, start by clearing the lists.
        foreach (const KContacts::Address &addr, addressee.addresses())
            addressee.removeAddress(addr);
        foreach (const KContacts::PhoneNumber &nr, addressee.phoneNumbers())
            addressee.removePhoneNumber(nr);
    }

    addressee.setGivenName(data.value(KDCRMFields::firstName()));
    addressee.setFamilyName(data.value(KDCRMFields::lastName()));
    addressee.setTitle(data.value(KDCRMFields::title()));
    addressee.setDepartment(data.value(KDCRMFields::department()));
    addressee.setOrganization(data.value(KDCRMFields::accountName()));
    contactWrapper.setAccountId(data.value(KDCRMFields::accountId()));
    QStringList emails;
    emails << data.value(KDCRMFields::email1()) // first one is preferred one
           << data.value(KDCRMFields::email2());
    addressee.setEmails(emails);
    addressee.insertPhoneNumber(KContacts::PhoneNumber(data.value(KDCRMFields::phoneHome()), KContacts::PhoneNumber::Home));
    addressee.insertPhoneNumber(KContacts::PhoneNumber(data.value(KDCRMFields::phoneMobile()), KContacts::PhoneNumber::Cell));
    addressee.insertPhoneNumber(KContacts::PhoneNumber(data.value(KDCRMFields::phoneWork()), KContacts::PhoneNumber::Work));
    addressee.insertPhoneNumber(KContacts::PhoneNumber(data.value(KDCRMFields::phoneOther()), KContacts::PhoneNumber::Car));
    addressee.insertPhoneNumber(KContacts::PhoneNumber(data.value(KDCRMFields::phoneFax()), KContacts::PhoneNumber::Work | KContacts::PhoneNumber::Fax));

    KContacts::Address primaryAddress;
    primaryAddress.setType(KContacts::Address::Work | KContacts::Address::Pref);
    primaryAddress.setStreet(data.value(KDCRMFields::primaryAddressStreet()));
    primaryAddress.setLocality(data.value(KDCRMFields::primaryAddressCity()));
    primaryAddress.setRegion(data.value(KDCRMFields::primaryAddressState()));
    primaryAddress.setPostalCode(data.value(KDCRMFields::primaryAddressPostalcode()));
    primaryAddress.setCountry(KDCRMUtils::canonicalCountryName(data.value(KDCRMFields::primaryAddressCountry())));
    addressee.insertAddress(primaryAddress);

    KContacts::Address otherAddress;
    otherAddress.setType(KContacts::Address::Home);
    otherAddress.setStreet(data.value(KDCRMFields::altAddressStreet()));
    otherAddress.setLocality(data.value(KDCRMFields::altAddressCity()));
    otherAddress.setRegion(data.value(KDCRMFields::altAddressState()));
    otherAddress.setPostalCode(data.value(KDCRMFields::altAddressPostalcode()));
    otherAddress.setCountry(KDCRMUtils::canonicalCountryName(data.value(KDCRMFields::altAddressCountry())));
    addressee.insertAddress(otherAddress);

    addressee.setBirthday(KDCRMUtils::dateFromString(data.value(KDCRMFields::birthdate())));

    addressee.setNote(data.value(KDCRMFields::description()));
    contactWrapper.setAssistant(data.value(KDCRMFields::assistant()));
    contactWrapper.setPhoneAssistant(data.value(KDCRMFields::phoneAssistant()));
    contactWrapper.setLeadSource(data.value(KDCRMFields::leadSource()));
    contactWrapper.setCampaign(data.value(KDCRMFields::campaign()));
    contactWrapper.setCampaignId(data.value(KDCRMFields::campaignId()));
    contactWrapper.setCAcceptStatusFields(data.value(KDCRMFields::cAcceptStatusFields()));
    contactWrapper.setMAcceptStatusFields(data.value(KDCRMFields::mAcceptStatusFields()));
    contactWrapper.setAssignedUserId(data.value(KDCRMFields::assignedUserId()));
    contactWrapper.setAssignedUserName(data.value(KDCRMFields::assignedUserName()));
    contactWrapper.setReportsTo(data.value(KDCRMFields::reportsTo()));
    contactWrapper.setReportsToId(data.value(KDCRMFields::reportsToId()));
    contactWrapper.setOpportunityRoleFields(data.value(KDCRMFields::opportunityRoleFields()));
    contactWrapper.setModifiedByName(data.value(KDCRMFields::modifiedByName()));
    contactWrapper.setDateModified(data.value(KDCRMFields::dateModified()));
    contactWrapper.setModifiedUserId(data.value(KDCRMFields::modifiedUserId()));
    contactWrapper.setDateEntered(data.value(KDCRMFields::dateEntered()));
    contactWrapper.setCreatedByName(data.value(KDCRMFields::createdByName()));
    contactWrapper.setCreatedBy(data.value(KDCRMFields::createdBy()));
    contactWrapper.setId(data.value(KDCRMFields::id()));
    contactWrapper.setSalutation(data.value(KDCRMFields::salutation()));
    contactWrapper.setDeleted(data.value(KDCRMFields::deleted()));
    contactWrapper.setDoNotCall(data.value(KDCRMFields::doNotCall()));
    contactWrapper.setInvalidEmail(data.value(KDCRMFields::invalidEmail()));

    item.setMimeType(KContacts::Addressee::mimeType());
    item.setPayload<KContacts::Addressee>(addressee);
}

void ContactDetails::updateLinkedItemsButtons()
{
    const QString contactId = id();
    {
        const int notes = contactId.isEmpty() ? 0 : mLinkedItemsRepository->notesForContact(contactId).count() + mLinkedItemsRepository->emailsForContact(contactId).count();
        const QString buttonText = (notes == 0) ? i18n("Add Note") : i18np("View 1 Note", "View %1 Notes", notes);
        mUi->viewNotesButton->setText(buttonText);
        mUi->viewNotesButton->setEnabled(!contactId.isEmpty());
    }
}

void ContactDetails::setDataInternal(const QMap<QString, QString> &)
{
    fillComboBox(mUi->salutation, KDCRMFields::salutation());
    fillComboBox(mUi->lead_source, KDCRMFields::leadSource());

    const QUrl url = itemDataExtractor()->itemUrl(resourceBaseUrl(), id());
    if (url.isValid())
        mUi->urllabel->setText(QString("<a href=\"%1\">Open Contact in Web Browser</a>").arg(url.toString()));

    slotEnableMailToPrimary();
    connect(mUi->email1, &QLineEdit::textChanged, this, &ContactDetails::slotEnableMailToPrimary);
    connect(mUi->buttonMailToPrimary, &QAbstractButton::clicked, this, &ContactDetails::slotMailToPrimary);
    slotEnableMailToOther();
    connect(mUi->email2, &QLineEdit::textChanged, this, &ContactDetails::slotEnableMailToOther);
    connect(mUi->buttonMailToOther, &QAbstractButton::clicked, this, &ContactDetails::slotMailToOther);

    connect(mUi->phone_work, &QLineEdit::textChanged, this, [this]() {
        mUi->buttonCallWork->setEnabled(!mUi->phone_work->text().isEmpty());
    });
    connect(mUi->buttonCallWork, &QPushButton::clicked, this, [this]() {
        QDesktopServices::openUrl(QUrl("tel:" + mUi->phone_work->text()));
    });
    connect(mUi->phone_mobile, &QLineEdit::textChanged, this, [this]() {
        mUi->buttonCallMobile->setEnabled(!mUi->phone_mobile->text().isEmpty());
    });
    connect(mUi->buttonCallMobile, &QPushButton::clicked, this, [this]() {
        QDesktopServices::openUrl(QUrl("tel:" + mUi->phone_mobile->text()));
    });
    connect(mUi->phone_home, &QLineEdit::textChanged, this, [this]() {
        mUi->buttonCallHome->setEnabled(!mUi->phone_home->text().isEmpty());
    });
    connect(mUi->buttonCallHome, &QPushButton::clicked, this, [this]() {
        QDesktopServices::openUrl(QUrl("tel:" + mUi->phone_home->text()));
    });
    connect(mUi->phone_fax, &QLineEdit::textChanged, this, [this]() {
        mUi->buttonFax->setEnabled(!mUi->phone_fax->text().isEmpty());
    });
    connect(mUi->buttonFax, &QPushButton::clicked, this, [this]() {
        QDesktopServices::openUrl(QUrl("fax:" + mUi->phone_fax->text()));
    });
    connect(mUi->phone_other, &QLineEdit::textChanged, this, [this]() {
        mUi->buttonCallOtherPhone->setEnabled(!mUi->phone_other->text().isEmpty());
    });
    connect(mUi->buttonCallOtherPhone, &QPushButton::clicked, this, [this]() {
        QDesktopServices::openUrl(QUrl("tel:" + mUi->phone_other->text()));
    });

    updateLinkedItemsButtons();
}

void ContactDetails::on_buttonOpenAccount_clicked()
{
    const QString accountId = currentAccountId();
    emit openObject(DetailsType::Account, accountId);
}

void ContactDetails::on_viewNotesButton_clicked()
{
    const QString contactId = id();
    const QVector<SugarNote> notes = mLinkedItemsRepository->notesForContact(contactId);
    qCDebug(FATCRM_CLIENT_LOG) << notes.count() << "notes found for contact" << contactId;
    const QVector<SugarEmail> emails = mLinkedItemsRepository->emailsForContact(contactId);
    qCDebug(FATCRM_CLIENT_LOG) << emails.count() << "emails found for contact" << contactId;
    auto *dlg = new NotesWindow(nullptr);
    dlg->setResourceIdentifier(resourceIdentifier());
    dlg->setLinkedItemsRepository(mLinkedItemsRepository);
    dlg->setLinkedTo(contactId, type());
    dlg->setWindowTitle(i18n("Notes for contact %1", name()));
    foreach(const SugarNote &note, notes) {
        dlg->addNote(note);
    }
    foreach(const SugarEmail &email, emails) {
        dlg->addEmail(email);
    }
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void ContactDetails::slotSelectAccount()
{
    auto *dlg = new SelectItemDialog(DetailsType::Account, this);
    dlg->setModel(ModelRepository::instance()->model(DetailsType::Account));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(dlg, &SelectItemDialog::selectedItem, this, &ContactDetails::slotAccountSelected);
    dlg->show();
}

void ContactDetails::slotAccountSelected(const QString &accountId)
{
    const int idx = mUi->account_id->findData(accountId);
    if (idx > -1) {
        mUi->account_id->setCurrentIndex(idx);
        slotAccountActivated();
    }
}

void ContactDetails::slotPrimaryAddressCountryEditingFinished()
{
    mUi->primaryAddressCountry->setText(KDCRMUtils::canonicalCountryName(mUi->primaryAddressCountry->text()));
}

void ContactDetails::slotOtherAddressCountryEditingFinished()
{
    mUi->altAddressCountry->setText(KDCRMUtils::canonicalCountryName(mUi->altAddressCountry->text()));
}

void ContactDetails::setItemsTreeModel(ItemsTreeModel *model)
{
    QStringList titleList;
    titleList.reserve(2500);
    for (int row = 0; row < model->rowCount(); ++row) {
        const QModelIndex index = model->index(row, 0);
        const Akonadi::Item item = index.data(Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();
        const KContacts::Addressee addressee = item.payload<KContacts::Addressee>();
        const QString addresseeTitle = addressee.title();
        if (!addresseeTitle.isEmpty() && !titleList.contains(addresseeTitle))
            titleList.append(addresseeTitle);
    }

    QCompleter *titlesCompleter = new QCompleter(titleList, this);
    titlesCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    mUi->title->setCompleter(titlesCompleter);
    Details::setItemsTreeModel(model);
}

void ContactDetails::slotEnableMailToPrimary()
{
    mUi->buttonMailToPrimary->setEnabled(!mUi->email1->text().isEmpty());
}

void ContactDetails::slotEnableMailToOther()
{
    mUi->buttonMailToOther->setEnabled(!mUi->email2->text().isEmpty());
}

void ContactDetails::slotMailToPrimary()
{
    QDesktopServices::openUrl(QUrl("mailto:" + mUi->email1->text()));
}

void ContactDetails::slotMailToOther()
{
    QDesktopServices::openUrl(QUrl("mailto:" + mUi->email2->text()));
}

void ContactDetails::slotLinkedItemsModified(const QString &contactId)
{
    if (contactId == id()) {
        updateLinkedItemsButtons();
    }
}
