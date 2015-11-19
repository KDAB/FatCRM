/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

AccountImportDialog::AccountImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AccountImportDialog)
{
    ui->setupUi(this);
    connect(&m_lineEditMapper, SIGNAL(mapped(QWidget*)), this, SLOT(slotTextChanged(QWidget*)));
}

AccountImportDialog::~AccountImportDialog()
{
    delete ui;
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

void AccountImportDialog::fillSimilarAccounts(QGroupBox *container, const SugarAccount& newAccount)
{
    QGridLayout *buttonsLayout = new QGridLayout(container);

    bool foundMatch = false;
    const QList<SugarAccount> similarAccounts = AccountRepository::instance()->similarAccounts(newAccount);

    int buttonRow = 0;
    int buttonCol = 0;
    foreach(const SugarAccount &account, similarAccounts) {
        const bool perfectMatch = account.isSameAccount(newAccount);
        QRadioButton *button = new QRadioButton(accountNameAndLocation(account), container);
        if (perfectMatch) {
            foundMatch = true;
            button->setChecked(true);
        }
        buttonsLayout->addWidget(button, buttonRow, buttonCol++);
        if (buttonCol == 4) {
            buttonCol = 0;
            ++buttonRow;
        }
    }

    QRadioButton *createButton = new QRadioButton(i18n("Create account"), container);
    if (!foundMatch)
        createButton->setChecked(true);
    buttonsLayout->addWidget(createButton, buttonRow, buttonCol);
}

void AccountImportDialog::setImportedAccounts(const QVector<SugarAccount> &accounts)
{
    m_accounts = accounts;
    ui->mainLabel->setText(i18n("%1 accounts found in import file. For each account, check if one of the suggested accounts should be used, or if a new account should be created.", accounts.count()));

    int layoutRow = 0;
    for (int row = 0; row < accounts.count(); ++row, ++layoutRow) {

        const SugarAccount& newAccount = accounts.at(row);

        QLineEdit *accountLineEdit = new QLineEdit(newAccount.name(), this);
        accountLineEdit->setMinimumWidth(accountLineEdit->sizeHint().width()); // show full account name
        ui->gridLayout->addWidget(accountLineEdit, layoutRow, 0);
        m_lineEditMapper.setMapping(accountLineEdit, accountLineEdit);
        connect(accountLineEdit, SIGNAL(textChanged(QString)), &m_lineEditMapper, SLOT(map()));

        QLabel *locationLabel = new QLabel(location(newAccount), this);
        ui->gridLayout->addWidget(locationLabel, layoutRow, 1);

        QGroupBox *container = new QGroupBox(this);
        ui->gridLayout->addWidget(container, layoutRow, 2);

        accountLineEdit->setProperty("row", row);
        accountLineEdit->setProperty("container", QVariant::fromValue(container));
        fillSimilarAccounts(container, newAccount);

        //QFrame *hLine = new QFrame(this);
        //hLine->setFrameStyle(QFrame::HLine);
        //++layoutRow;
        //ui->gridLayout->addWidget(hLine, layoutRow, 0, 1, 4);
    }
}

void AccountImportDialog::accept()
{
    QDialog::accept();
}

Q_DECLARE_METATYPE(QGroupBox *)

void AccountImportDialog::slotTextChanged(QWidget *obj)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);
    Q_ASSERT(lineEdit);
    QGroupBox *container = lineEdit->property("container").value<QGroupBox *>();
    Q_ASSERT(container);
    qDeleteAll(container->children());
    int row = lineEdit->property("row").toInt();
    m_accounts[row].setName(lineEdit->text());
    fillSimilarAccounts(container, m_accounts.at(row));

}
