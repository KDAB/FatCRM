/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sugarconfigdialog.h"

#include "passwordhandler.h"
#include "settings.h"

SugarConfigDialog::SugarConfigDialog(PasswordHandler *passwordHandler, const QString &accountName, QWidget *parent)
    : QDialog(parent)
{
    mUi.setupUi(this);

    mUi.accountName->setText(accountName);
    mUi.host->setText(Settings::host());
    mUi.user->setText(Settings::user());
    mUi.password->setText(passwordHandler->password());

    mUi.checkIntervalSpinbox->setValue(Settings::intervalCheckTime());
}

SugarConfigDialog::~SugarConfigDialog()
{
}

QString SugarConfigDialog::accountName() const
{
    return mUi.accountName->text();
}

QString SugarConfigDialog::host() const
{
    return mUi.host->text();
}

QString SugarConfigDialog::user() const
{
    return mUi.user->text();
}

QString SugarConfigDialog::password() const
{
    return mUi.password->text();
}

int SugarConfigDialog::intervalCheckTime() const
{
    const int val = mUi.checkIntervalSpinbox->value();
    return val == mUi.checkIntervalSpinbox->minimum() ? -1 : val;
}

