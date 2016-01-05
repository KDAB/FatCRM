/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "accountimportdialog.h"
#include "ui_accountimportdialog.h"
#include "accountrepository.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

#include <KJobUiDelegate>
#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>

AccountImportDialog::AccountImportDialog(QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::AccountImportDialog)
{
    mUi->setupUi(this);
    connect(&mLineEditMapper, SIGNAL(mapped(QWidget*)), this, SLOT(slotTextChanged(QWidget*)));
    connect(mUi->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(mUi->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

AccountImportDialog::~AccountImportDialog()
{
    delete mUi;
}

void AccountImportDialog::setAccountCollection(const Akonadi::Collection &collection)
{
    mAccountCollection = collection;
}

static QString location(const SugarAccount &account)
{
    QString location = account.billingAddressCity();
    if (location.isEmpty())
        location = i18n("<missing city>");
    if (!account.billingAddressCountry().isEmpty()) {
        location = i18n("%1, %2", location, account.billingAddressCountry());
    } else {
        location = i18n("%1, <missing country>", location);
    }
    return location;
}

static QString accountNameAndLocation(const SugarAccount &account)
{
    return QString("%1 (%2)").arg(account.name(), location(account));
}

void AccountImportDialog::fillSimilarAccounts(int row)
{
    QGroupBox *container = mGroupBoxes.at(row);
    QGridLayout *buttonsLayout = qobject_cast<QGridLayout *>(container->layout());
    Q_ASSERT(buttonsLayout);

    QButtonGroup *group = mButtonGroups.at(row);

    //bool foundMatch = false;
    const SugarAccount& newAccount = mAccounts.at(row);
    const QList<SugarAccount> similarAccounts = AccountRepository::instance()->similarAccounts(newAccount);

    int buttonRow = 0;
    int buttonCol = 0;
    foreach(const SugarAccount &account, similarAccounts) {
        const bool perfectMatch = account.isSameAccount(newAccount);
        QRadioButton *button = new QRadioButton(accountNameAndLocation(account), container);
        if (perfectMatch) {
            //foundMatch = true;
            button->setChecked(true);
        }
        buttonsLayout->addWidget(button, buttonRow, buttonCol++);
        if (buttonCol == 4) {
            buttonCol = 0;
            ++buttonRow;
        }
        button->setProperty("accountId", account.id());
        group->addButton(button);
    }

    QPushButton *createButton = new QPushButton(i18n("Create account"), container);
    createButton->setProperty("accountRowNumber", row);
    group->addButton(createButton);
    buttonsLayout->addWidget(createButton, buttonRow, buttonCol);
}

void AccountImportDialog::setImportedAccounts(const QVector<SugarAccount> &accounts)
{
    mAccounts = accounts;
    mUi->mainLabel->setText(i18n("%1 accounts found in import file. For each account, check if one of the suggested accounts should be used, or if a new account should be created.", accounts.count()));

    for (int row = 0; row < accounts.count(); ++row) {

        const SugarAccount& newAccount = accounts.at(row);

        QLineEdit *accountLineEdit = new QLineEdit(newAccount.name(), this);
        accountLineEdit->setMinimumWidth(accountLineEdit->sizeHint().width()); // show full account name
        mUi->gridLayout->addWidget(accountLineEdit, row, 0);
        mLineEditMapper.setMapping(accountLineEdit, accountLineEdit);
        connect(accountLineEdit, SIGNAL(textChanged(QString)), &mLineEditMapper, SLOT(map()));

        QLabel *locationLabel = new QLabel(location(newAccount), this);
        mUi->gridLayout->addWidget(locationLabel, row, 1);

        QGroupBox *container = new QGroupBox(this);
        mUi->gridLayout->addWidget(container, row, 2);

        accountLineEdit->setProperty("row", row);
        QButtonGroup *buttonGroup = new QButtonGroup(this);
        connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slotButtonClicked(QAbstractButton *)));
        mButtonGroups.append(buttonGroup);
        new QGridLayout(container); // used by fillSimilarAccounts
        mGroupBoxes.append(container);
        fillSimilarAccounts(row);
    }
    updateOKButton();
}

QVector<SugarAccount> AccountImportDialog::chosenAccounts() const
{
    return mAccounts;
}

void AccountImportDialog::accept()
{
    for (int row = 0 ; row < mButtonGroups.count(); ++row) {
        QButtonGroup *buttonGroup = mButtonGroups.at(row);
        QAbstractButton *radio = buttonGroup->checkedButton();
        const QVariant accountIdVar = radio->property("accountId");
        if (accountIdVar.isValid()) {
            // use existing account
            const SugarAccount account = AccountRepository::instance()->accountById(accountIdVar.toString());
            Q_ASSERT(!account.isEmpty());
            mAccounts[row] = account;
        }
        // else .... no account selected? account being created? TODO
    }
    QDialog::accept();
}

void AccountImportDialog::reject()
{
    // kill any running account-creation jobs...
    foreach (KJob *job, mAccountCreationJobs) {
        job->kill();
    }
    mAccountCreationJobs.clear();
    QDialog::reject();
}

Q_DECLARE_METATYPE(QGroupBox *)
Q_DECLARE_METATYPE(QAbstractButton *)

void AccountImportDialog::slotTextChanged(QWidget *obj)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);
    Q_ASSERT(lineEdit);
    const int row = lineEdit->property("row").toInt();
    QButtonGroup *buttonGroup = mButtonGroups.at(row);
    qDeleteAll(buttonGroup->buttons());
    mAccounts[row].setName(lineEdit->text());
    fillSimilarAccounts(row);
    updateOKButton();
}

// called when clicking on a create pushbutton or on any radiobutton
void AccountImportDialog::slotButtonClicked(QAbstractButton *button)
{
    const QVariant accountRowNumber = button->property("accountRowNumber");
    if (accountRowNumber.isValid()) {
        // create new account
        Akonadi::Item item;
        item.setMimeType(SugarAccount::mimeType());
        const SugarAccount account = mAccounts.at(accountRowNumber.toInt());
        qDebug() << "Creating account id=" << account.id() << "name=" << account.name();
        item.setPayload<SugarAccount>(account);
        Akonadi::Job *job = new Akonadi::ItemCreateJob(item, mAccountCollection, this);
        job->setProperty("jobAccountRow", accountRowNumber);
        job->setProperty("jobButton", QVariant::fromValue(button));
        mAccountCreationJobs.append(job);
        button->setEnabled(false);
        button->setText(i18n("Creating account '%1'...", account.name()));
        connect(job, SIGNAL(result(KJob*)), this, SLOT(slotCreateAccountResult(KJob*)));
    } else {
        // radiobutton selected
        updateOKButton();
    }
}

void AccountImportDialog::slotCreateAccountResult(KJob *job)
{
    mAccountCreationJobs.removeAll(job);
    const QVariant accountRowNumber = job->property("jobAccountRow");
    Q_ASSERT(accountRowNumber.isValid());
    const int row = accountRowNumber.toInt();
    const SugarAccount &account = mAccounts.at(row);
    QAbstractButton *button = job->property("jobButton").value<QAbstractButton *>();
    if (job->error()) {
        button->setText(i18n("Error creating account %1", account.name()));
        button->setToolTip(job->errorString());
        mUi->statusLabel->setText(job->errorString());
        job->uiDelegate()->showErrorMessage();
    } else {
        // TODO wait for Monitor / AccountRepository::addAcount called
        // replace the pushbutton with a radiobutton // TOO EARLY
        QGroupBox *container = mGroupBoxes.at(row);
        QGridLayout *buttonsLayout = qobject_cast<QGridLayout *>(container->layout());
        int buttonRow;
        int buttonCol;
        int rowSpan, colSpan;
        buttonsLayout->getItemPosition(buttonsLayout->indexOf(button), &buttonRow, &buttonCol, &rowSpan, &colSpan);

        delete button;
        QRadioButton *button = new QRadioButton(accountNameAndLocation(account), container);
        mButtonGroups.at(row)->addButton(button);
        button->setChecked(true); // select the radiobutton that we just created (which is why we don't just call fillSimilarAccounts)
        buttonsLayout->addWidget(button, buttonRow, buttonCol);
        button->setProperty("accountId", account.id());
        button->show();

        Akonadi::ItemCreateJob *createJob = static_cast<Akonadi::ItemCreateJob *>(job);
        qDebug() << "OK, created. id=" << createJob->item().id() << " remoteId=" << createJob->item().remoteId();

        updateOKButton();
    }
}

void AccountImportDialog::updateOKButton()
{
    QPushButton* okButton = mUi->buttonBox->button(QDialogButtonBox::Ok);
    for (int row = 0 ; row < mButtonGroups.count(); ++row) {
        QButtonGroup *buttonGroup = mButtonGroups.at(row);
        QAbstractButton *radio = buttonGroup->checkedButton();
        if (!radio || !radio->property("accountId").isValid()) {
            okButton->setEnabled(false);
            return;
        }
    }
    okButton->setEnabled(true);
}

