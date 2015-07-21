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
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

AccountImportDialog::AccountImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AccountImportDialog)
{
    ui->setupUi(this);
}

AccountImportDialog::~AccountImportDialog()
{
    delete ui;
}

static QString location(const SugarAccount &account)
{
    QString location = account.billingAddressCity();
    if (!account.billingAddressCountry().isEmpty()) {
        location += ", " + account.billingAddressCountry();
    }
    return location;
}

static QString accountNameAndLocation(const SugarAccount &account)
{
    return QString("%1 (%2)").arg(account.name(), location(account));
}

void AccountImportDialog::setImportedAccounts(const QVector<SugarAccount> &accounts)
{
    ui->mainLabel->setText(i18n("%1 accounts parsed", accounts.count()));

    for (int row = 0; row < accounts.count(); ++row) {
        const SugarAccount& newAccount = accounts.at(row);

        QLineEdit *accountLineEdit = new QLineEdit(newAccount.name(), this);
        ui->gridLayout->addWidget(accountLineEdit, row, 0);

        QLabel *locationLabel = new QLabel(location(newAccount), this);
        ui->gridLayout->addWidget(locationLabel, row, 1);

        bool foundMatch = false;
        const QList<SugarAccount> similarAccounts = AccountRepository::instance()->similarAccounts(newAccount);

        QGridLayout *buttonsLayout = new QGridLayout(this);
        ui->gridLayout->addLayout(buttonsLayout, row, 2);

        int buttonRow = 0;
        int buttonCol = 0;
        foreach(const SugarAccount &account, similarAccounts) {
            const bool perfectMatch = account.isSameAccount(newAccount);
            QPushButton *button = new QPushButton(accountNameAndLocation(account));
            button->setCheckable(true);
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

        QCheckBox *createCheckBox = new QCheckBox(i18n("Create"), this);
        if (!foundMatch)
            createCheckBox->setChecked(true);
        ui->gridLayout->addWidget(createCheckBox, row, 3);
    }
}

void AccountImportDialog::accept()
{
    QDialog::accept();
}
