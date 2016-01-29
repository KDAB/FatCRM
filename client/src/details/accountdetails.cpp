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

    mUi->name->setObjectName(KDCRMFields::name());
    mUi->website->setObjectName(KDCRMFields::website());
    mUi->ticker_symbol->setObjectName(KDCRMFields::tickerSymbol());
    mUi->parent_id->setObjectName(KDCRMFields::parentId());
    mUi->ownership->setObjectName(KDCRMFields::ownership());
    mUi->industry->setObjectName(KDCRMFields::industry());
    mUi->account_type->setObjectName(KDCRMFields::accountType());
    mUi->assigned_user_id->setObjectName(KDCRMFields::assignedUserId());
    mUi->phone_office->setObjectName(KDCRMFields::accountPhoneWork());
    mUi->phone_fax->setObjectName(KDCRMFields::phoneFax());
    mUi->phone_alternate->setObjectName(KDCRMFields::accountPhoneOther());
    mUi->employees->setObjectName(KDCRMFields::employees());
    mUi->rating->setObjectName(KDCRMFields::rating());
    mUi->sic_code->setObjectName(KDCRMFields::sicCode());
    mUi->annual_revenue->setObjectName(KDCRMFields::annualRevenue());
    mUi->email1->setObjectName(KDCRMFields::email1());
    mUi->visma_id_c->setObjectName(KDCRMFields::vismaId());
    mUi->accounting_c->setObjectName(KDCRMFields::accounting());
    mUi->vat_no_c->setObjectName(KDCRMFields::vatNo());
    mUi->billing_address_street->setObjectName(KDCRMFields::billingAddressStreet());
    mUi->billing_address_city->setObjectName(KDCRMFields::billingAddressCity());
    mUi->billing_address_state->setObjectName(KDCRMFields::billingAddressState());
    mUi->billing_address_postalcode->setObjectName(KDCRMFields::billingAddressPostalcode());
    mUi->billing_address_country->setObjectName(KDCRMFields::billingAddressCountry());
    mUi->shipping_address_street->setObjectName(KDCRMFields::shippingAddressStreet());
    mUi->shipping_address_city->setObjectName(KDCRMFields::shippingAddressCity());
    mUi->shipping_address_state->setObjectName(KDCRMFields::shippingAddressState());
    mUi->shipping_address_postalcode->setObjectName(KDCRMFields::shippingAddressPostalcode());
    mUi->shipping_address_country->setObjectName(KDCRMFields::shippingAddressCountry());

    initialize();
}

AccountDetails::~AccountDetails()
{
    delete mUi;
}

QString AccountDetails::idForItem(const Akonadi::Item &item) const
{
    if (item.hasPayload<SugarAccount>()) {
        const SugarAccount account = item.payload<SugarAccount>();
        return account.id();
    }
    return QString();
}

QUrl AccountDetails::itemUrl() const
{
    const QString baseUrl = resourceBaseUrl();
    if (!baseUrl.isEmpty() && !id().isEmpty()) {
        const QString url = baseUrl + "?action=DetailView&module=Accounts&record=" + id();
        return QUrl(url);
    }
    return QUrl();
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

    const QUrl url = itemUrl();
    if (url.isValid())
        mUi->urllabel->setText(QString("<a href=\"%1\">Open Account in Web Browser</a>").arg(url.toString()));
}
