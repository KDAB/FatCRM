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

#include "accountdetails.h"

#include "ui_accountdetails.h"
#include "referenceddatamodel.h"

#include "kdcrmdata/sugaraccount.h"
#include "kdcrmdata/kdcrmfields.h"

AccountDetails::AccountDetails(QWidget *parent)
    : Details(Account, parent), mUi(new Ui::AccountDetails)
{
    mUi->setupUi(this);
    mUi->urllabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    initialize();
}

AccountDetails::~AccountDetails()
{
    delete mUi;
}

void AccountDetails::initialize()
{
    setObjectName("accountDetails");

    ReferencedDataModel::setModelForCombo(mUi->parent_id, AccountRef);
    //ReferencedDataModel::setModelForCombo(mUi->campaign_id, CampaignRef);
    ReferencedDataModel::setModelForCombo(mUi->assigned_user_id, AssignedToRef);
}

QMap<QString, QString> AccountDetails::data(const Akonadi::Item &item) const
{
    Q_ASSERT(item.hasPayload<SugarAccount>());
    SugarAccount account = item.payload<SugarAccount>();
    return account.data();
}

void AccountDetails::updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const
{
    SugarAccount account;
    if (item.hasPayload<SugarAccount>()) {
        account = item.payload<SugarAccount>();
    }
    account.setData(data);

    item.setMimeType(SugarAccount::mimeType());
    item.setPayload<SugarAccount>(account);
}

void AccountDetails::setDataInternal(const QMap<QString, QString> &) const
{
    fillComboBox(mUi->industry, KDCRMFields::industry());
    fillComboBox(mUi->account_type, KDCRMFields::accountType());

    const QString baseUrl = resourceBaseUrl();
    if (!baseUrl.isEmpty() && !id().isEmpty()) {
        const QString url = baseUrl + "?action=DetailView&module=Accounts&record=" + id();
        mUi->urllabel->setText(QString("<a href=\"%1\">Open Account in Web Browser</a>").arg(url));
    }
}
