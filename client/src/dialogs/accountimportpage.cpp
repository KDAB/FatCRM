/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "accountimportpage.h"
#include "ui_accountimportpage.h"
#include "accountrepository.h"
#include "fatcrm_client_debug.h"

#include <AkonadiCore/ItemCreateJob>

#include <KJobUiDelegate>
#include <KLocalizedString>

#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QTextDocument>

AccountImportPage::AccountImportPage(QWidget *parent) :
    QWizardPage(parent),
    mUi(new Ui::AccountImportPage)
{
    mUi->setupUi(this);
    connect(&mLineEditMapper, SIGNAL(mapped(QWidget*)), this, SLOT(slotTextChanged(QWidget*)));

    connect(AccountRepository::instance(), &AccountRepository::accountAdded,
            this, &AccountImportPage::slotAccountAdded);
}

AccountImportPage::~AccountImportPage()
{
    delete mUi;
}

void AccountImportPage::setAccountCollection(const Akonadi::Collection &collection)
{
    mAccountCollection = collection;
}

static QString location(const SugarAccount &account)
{
    QString location = !account.shippingAddressCity().isEmpty() ? account.shippingAddressCity() : account.billingAddressCity();
    if (location.isEmpty())
        location = i18n("<missing city>");
    if (!account.shippingAddressCountry().isEmpty()) {
        location = i18n("%1, %2", location, account.shippingAddressCountry());
    } else if (!account.billingAddressCountry().isEmpty()) {
        location = i18n("%1, %2", location, account.billingAddressCountry());
    } else {
        location = i18n("%1, <missing country>", location);
    }
    return location;
}

static QString accountNameAndLocation(const SugarAccount &account)
{
    return QStringLiteral("%1 (%2)").arg(account.name(), location(account));
}

void AccountImportPage::fillSimilarAccounts(int row)
{
    PendingAccount &pa = mPendingAccounts[row];

    QGroupBox *container = pa.groupBox;
    auto *buttonsLayout = qobject_cast<QGridLayout *>(container->layout());
    Q_ASSERT(buttonsLayout);

    QButtonGroup *group = pa.buttonGroup;

    //bool foundMatch = false;
    const SugarAccount& newAccount = pa.contactsSet.account;
    const QList<SugarAccount> similarAccounts = AccountRepository::instance()->similarAccounts(newAccount);

    int buttonRow = 0;
    int buttonCol = 0;
    foreach(const SugarAccount &account, similarAccounts) {
        const bool perfectMatch = account.isSameAccount(newAccount);
        QString text = accountNameAndLocation(account);
        QRadioButton *button = new QRadioButton(text.replace('&', QLatin1String("&&")), container);
        if (perfectMatch) {
            //foundMatch = true;
            button->setChecked(true);
        }
        buttonsLayout->addWidget(button, buttonRow, buttonCol++);
        if (buttonCol == 3) {
            buttonCol = 0;
            ++buttonRow;
        }
        button->setProperty("accountId", account.id());
        group->addButton(button);
    }

    QPushButton *createButton = new QPushButton(i18n("Create account"), container);
    createButton->setProperty("accountRowNumber", row);
    group->addButton(createButton);
    pa.createButton = createButton;
    buttonsLayout->addWidget(createButton, buttonRow, buttonCol);
}

void AccountImportPage::setImportedContacts(const QVector<ContactsSet> &contacts)
{
    mUi->mainLabel->setText(i18n("%1 accounts found in import file. For each account, check if one of the suggested accounts should be used, or if a new account should be created.", contacts.count()));

    for (int row = 0; row < contacts.count(); ++row) {

        const SugarAccount& newAccount = contacts.at(row).account;

        QLineEdit *accountLineEdit = new QLineEdit(newAccount.name(), this);
        accountLineEdit->setMinimumWidth(accountLineEdit->sizeHint().width()); // show full account name
        mUi->gridLayout->addWidget(accountLineEdit, row, 0);
        mLineEditMapper.setMapping(accountLineEdit, accountLineEdit);
        connect(accountLineEdit, SIGNAL(textChanged(QString)), &mLineEditMapper, SLOT(map()));

        QLabel *locationLabel = new QLabel(location(newAccount), this);
        mUi->gridLayout->addWidget(locationLabel, row, 1);

        auto *container = new QGroupBox(this);
        mUi->gridLayout->addWidget(container, row, 2);

        accountLineEdit->setProperty("row", row);
        auto *buttonGroup = new QButtonGroup(this);
        connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slotButtonClicked(QAbstractButton*)));

        QString contactsInfo;
        foreach (const KContacts::Addressee &addressee, contacts.at(row).addressees) {
            qCDebug(FATCRM_CLIENT_LOG) << addressee.familyName() << " " << addressee.fullEmail();
            contactsInfo += addressee.fullEmail().toHtmlEscaped() + '\n';
        }
        if (!contactsInfo.isEmpty()) {
            accountLineEdit->setToolTip(contactsInfo);
        }

        PendingAccount pendingAccount;
        pendingAccount.buttonGroup = buttonGroup;
        pendingAccount.groupBox = container;
        pendingAccount.createButton = nullptr;
        pendingAccount.contactsSet = contacts.at(row);
        pendingAccount.idBeingCreated = 0;
        mPendingAccounts.append(pendingAccount);
        new QGridLayout(container); // used by fillSimilarAccounts
        fillSimilarAccounts(row);
    }

    emit completeChanged();
    QMetaObject::invokeMethod(this, "adjustPageSize", Qt::QueuedConnection);
}

void AccountImportPage::adjustPageSize()
{
    setMinimumWidth(mUi->scrollArea->widget()->width() + 40);
    emit layoutChanged();
}

QVector<ContactsSet> AccountImportPage::chosenContacts() const
{
    QVector<ContactsSet> ret;
    ret.reserve(mPendingAccounts.count());
    for (int row = 0 ; row < mPendingAccounts.count(); ++row) {
        QButtonGroup *buttonGroup = mPendingAccounts.at(row).buttonGroup;
        QAbstractButton *radio = buttonGroup->checkedButton();
        const QVariant accountIdVar = radio->property("accountId");
        Q_ASSERT(accountIdVar.isValid()); // the OK button is disabled if this isn't the case
        if (accountIdVar.isValid()) {
            ContactsSet contactsSet = mPendingAccounts.at(row).contactsSet;
            contactsSet.account = AccountRepository::instance()->accountById(accountIdVar.toString());
            Q_ASSERT(!contactsSet.account.isEmpty());
            ret.append(contactsSet);
        }
    }
    return ret;
}

void AccountImportPage::cleanup()
{
    // kill any running account-creation jobs...
    foreach (KJob *job, mAccountCreationJobs) {
        job->kill();
    }
    mAccountCreationJobs.clear();
}

bool AccountImportPage::validatePage()
{
    emit chosenContactsAvailable(chosenContacts());

    return QWizardPage::validatePage();
}

Q_DECLARE_METATYPE(QGroupBox *)
Q_DECLARE_METATYPE(QAbstractButton *)

void AccountImportPage::slotTextChanged(QWidget *obj)
{
    auto *lineEdit = qobject_cast<QLineEdit *>(obj);
    Q_ASSERT(lineEdit);
    const int row = lineEdit->property("row").toInt();
    PendingAccount &pa = mPendingAccounts[row];
    QButtonGroup *buttonGroup = pa.buttonGroup;
    qDeleteAll(buttonGroup->buttons());
    pa.contactsSet.account.setName(lineEdit->text());
    fillSimilarAccounts(row);

    emit completeChanged();
}

// called when clicking on a create pushbutton or on any radiobutton
void AccountImportPage::slotButtonClicked(QAbstractButton *button)
{
    const QVariant accountRowNumber = button->property("accountRowNumber");
    if (accountRowNumber.isValid()) {
        // create new account
        Akonadi::Item item;
        item.setMimeType(SugarAccount::mimeType());
        const SugarAccount account = mPendingAccounts.at(accountRowNumber.toInt()).contactsSet.account;
        qDebug() << "Creating account id=" << account.id() << "name=" << account.name();
        item.setPayload<SugarAccount>(account);
        Akonadi::Job *job = new Akonadi::ItemCreateJob(item, mAccountCollection, this);
        job->setProperty("jobAccountRow", accountRowNumber);
        mAccountCreationJobs.append(job);
        button->setEnabled(false);
        button->setText(i18n("Creating account '%1'...", account.name()));
        connect(job, &KJob::result, this, &AccountImportPage::slotCreateAccountResult);
    } else {
        // radiobutton selected
        emit completeChanged();
    }
}

void AccountImportPage::slotCreateAccountResult(KJob *job)
{
    mAccountCreationJobs.removeAll(job);
    const QVariant accountRowNumber = job->property("jobAccountRow");
    Q_ASSERT(accountRowNumber.isValid());
    const int row = accountRowNumber.toInt();
    PendingAccount &pa = mPendingAccounts[row];
    const SugarAccount &account = pa.contactsSet.account;
    QAbstractButton *button = pa.createButton;
    if (job->error()) {
        button->setText(i18n("Error creating account %1", account.name()));
        button->setToolTip(job->errorString());
        mUi->statusLabel->setText(job->errorString());
        job->uiDelegate()->showErrorMessage();
    } else {
        // Account created in akonadi, but no ID yet (this needs the resource to be online and sync it)
        // We need the ID to associate the contact to the account, so let's wait for AccountRepository.

        button->setText(i18n("Account created, waiting for server..."));
        auto *createJob = static_cast<Akonadi::ItemCreateJob *>(job);
        qDebug() << "OK, created. id=" << createJob->item().id();
        pa.idBeingCreated = createJob->item().id();
    }
}

void AccountImportPage::slotAccountAdded(const QString &id, Akonadi::Item::Id akonadiId)
{
    for (int row = 0 ; row < mPendingAccounts.count(); ++row) {
        if (mPendingAccounts.at(row).idBeingCreated == akonadiId) {
            accountCreated(row, id);
        }
    }
}

void AccountImportPage::accountCreated(int row, const QString &id)
{
    const SugarAccount account = AccountRepository::instance()->accountById(id);

    // replace the pushbutton with a radiobutton
    QGroupBox *container = mPendingAccounts.at(row).groupBox;
    QPushButton *createButton = mPendingAccounts.at(row).createButton;
    auto *buttonsLayout = qobject_cast<QGridLayout *>(container->layout());
    int buttonRow;
    int buttonCol;
    int rowSpan, colSpan;
    buttonsLayout->getItemPosition(buttonsLayout->indexOf(createButton), &buttonRow, &buttonCol, &rowSpan, &colSpan);

    delete createButton;
    QRadioButton *button = new QRadioButton(accountNameAndLocation(account), container);
    mPendingAccounts.at(row).buttonGroup->addButton(button);
    button->setChecked(true); // select the radiobutton that we just created (which is why we don't just call fillSimilarAccounts)
    buttonsLayout->addWidget(button, buttonRow, buttonCol);
    button->setProperty("accountId", account.id());
    button->show();

    emit completeChanged();
}

bool AccountImportPage::isComplete() const
{
    for (int row = 0 ; row < mPendingAccounts.count(); ++row) {
        const QButtonGroup *buttonGroup = mPendingAccounts.at(row).buttonGroup;
        const QAbstractButton *radio = buttonGroup->checkedButton();
        if (!radio || !radio->property("accountId").isValid()) {
            return false;
        }
    }

    return true;
}
