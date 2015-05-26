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

#include "sugaraccountio.h"
#include "sugaraccount.h"

#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QDebug>

SugarAccountIO::SugarAccountIO()
{
}

bool SugarAccountIO::readSugarAccount(QIODevice *device, SugarAccount &account)
{
    if (device == 0 || !device->isReadable()) {
        return false;
    }

    account = SugarAccount();
    xml.setDevice(device);
    if (xml.readNextStartElement()) {
        if (xml.name() == "sugarAccount"
                && xml.attributes().value("version") == "1.0") {
            readAccount(account);
        } else {
            xml.raiseError(QObject::tr("It is not a sugarAccount version 1.0 data."));
        }

    }
    return !xml.error();
}

QString SugarAccountIO::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
           .arg(xml.errorString())
           .arg(xml.lineNumber())
           .arg(xml.columnNumber());
}

void SugarAccountIO::readAccount(SugarAccount &account)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "sugarAccount");

    while (xml.readNextStartElement()) {
        if (xml.name() == "id") {
            account.setId(xml.readElementText());
        } else if (xml.name() == "name") {
            account.setName(xml.readElementText());
        } else if (xml.name() == "date_entered") {
            account.setDateEntered(xml.readElementText());
        } else if (xml.name() == "date_modified") {
            account.setDateModified(xml.readElementText());
        } else if (xml.name() == "modified_user_id") {
            account.setModifiedUserId(xml.readElementText());
        } else if (xml.name() == "modified_by_name") {
            account.setModifiedByName(xml.readElementText());
        } else if (xml.name() == "created_by") {
            account.setCreatedBy(xml.readElementText());
        } else if (xml.name() == "created_by_name") {
            account.setCreatedByName(xml.readElementText());
        } else if (xml.name() == "description") {
            account.setDescription(xml.readElementText());
        } else if (xml.name() == "deleted") {
            account.setDeleted(xml.readElementText());
        } else if (xml.name() == "assigned_user_id") {
            account.setAssignedUserId(xml.readElementText());
        } else if (xml.name() == "assigned_user_name") {
            account.setAssignedUserName(xml.readElementText());
        } else if (xml.name() == "account_type") {
            account.setAccountType(xml.readElementText());
        } else if (xml.name() == "industry") {
            account.setIndustry(xml.readElementText());
        } else if (xml.name() == "annual_revenue") {
            account.setAnnualRevenue(xml.readElementText());
        } else if (xml.name() == "phone_fax") {
            account.setPhoneFax(xml.readElementText());
        } else if (xml.name() == "billing_address_street") {
            account.setBillingAddressStreet(xml.readElementText());
        } else if (xml.name() == "billing_address_city") {
            account.setBillingAddressCity(xml.readElementText());
        } else if (xml.name() == "billing_address_state") {
            account.setBillingAddressState(xml.readElementText());
        } else if (xml.name() == "billing_address_postalcode") {
            account.setBillingAddressPostalcode(xml.readElementText());
        } else if (xml.name() == "billing_address_country") {
            account.setBillingAddressCountry(xml.readElementText());
        } else if (xml.name() == "rating") {
            account.setRating(xml.readElementText());
        } else if (xml.name() == "phone_office") {
            account.setPhoneOffice(xml.readElementText());
        } else if (xml.name() == "phone_alternate") {
            account.setPhoneAlternate(xml.readElementText());
        } else if (xml.name() == "website") {
            account.setWebsite(xml.readElementText());
        } else if (xml.name() == "ownership") {
            account.setOwnership(xml.readElementText());
        } else if (xml.name() == "employees") {
            account.setEmployees(xml.readElementText());
        } else if (xml.name() == "ticker_symbol") {
            account.setTickerSymbol(xml.readElementText());
        } else if (xml.name() == "shipping_address_street") {
            account.setShippingAddressStreet(xml.readElementText());
        } else if (xml.name() == "shipping_address_city") {
            account.setShippingAddressCity(xml.readElementText());
        } else if (xml.name() == "shipping_address_state") {
            account.setShippingAddressState(xml.readElementText());
        } else if (xml.name() == "shipping_address_postalcode") {
            account.setShippingAddressPostalcode(xml.readElementText());
        } else if (xml.name() == "shipping_address_country") {
            account.setShippingAddressCountry(xml.readElementText());
        } else if (xml.name() == "email1") {
            account.setEmail1(xml.readElementText());
        } else if (xml.name() == "parent_id") {
            account.setParentId(xml.readElementText());
        } else if (xml.name() == "sic_code") {
            account.setSicCode(xml.readElementText());
        } else if (xml.name() == "parent_name") {
            account.setParentName(xml.readElementText());
        } else if (xml.name() == "campaign_id") {
            account.setCampaignId(xml.readElementText());
        } else if (xml.name() == "campaign_name") {
            account.setCampaignName(xml.readElementText());
        } else {
            xml.skipCurrentElement();
        }
    }
}

bool SugarAccountIO::writeSugarAccount(const SugarAccount &account, QIODevice *device)
{
    if (device == 0 || !device->isWritable()) {
        return false;
    }

    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeDTD("<!DOCTYPE sugarAccount>");
    writer.writeStartElement("sugarAccount");
    writer.writeAttribute("version", "1.0");
    writer.writeTextElement(QString("id"), account.id());
    writer.writeTextElement(QString("name"), account.name());
    writer.writeTextElement(QString("date_entered"), account.dateEntered());
    writer.writeTextElement(QString("date_modified"), account.dateModified());
    writer.writeTextElement(QString("modified_user_id"), account.modifiedUserId());
    writer.writeTextElement(QString("modified_by_name"), account.modifiedByName());
    writer.writeTextElement(QString("created_by"), account.createdBy());
    writer.writeTextElement(QString("created_by_name"), account.createdByName());
    writer.writeTextElement(QString("description"), account.description());
    writer.writeTextElement(QString("deleted"), account.deleted());
    writer.writeTextElement(QString("assigned_user_id"), account.assignedUserId());
    writer.writeTextElement(QString("assigned_user_name"), account.assignedUserName());
    writer.writeTextElement(QString("account_type"), account.accountType());
    writer.writeTextElement(QString("industry"), account.industry());
    writer.writeTextElement(QString("annual_revenue"), account.annualRevenue());
    writer.writeTextElement(QString("phone_fax"), account.phoneFax());
    writer.writeTextElement(QString("billing_address_street"), account.billingAddressStreet());
    writer.writeTextElement(QString("billing_address_city"), account.billingAddressCity());
    writer.writeTextElement(QString("billing_address_state"), account.billingAddressState());
    writer.writeTextElement(QString("billing_address_postalcode"), account.billingAddressPostalcode());
    writer.writeTextElement(QString("billing_address_country"), account.billingAddressCountry());
    writer.writeTextElement(QString("rating"), account.rating());
    writer.writeTextElement(QString("phone_office"), account.phoneOffice());
    writer.writeTextElement(QString("phone_alternate"), account.phoneAlternate());
    writer.writeTextElement(QString("website"), account.website());
    writer.writeTextElement(QString("ownership"), account.ownership());
    writer.writeTextElement(QString("employees"), account.employees());
    writer.writeTextElement(QString("ticker_symbol"), account.tickerSymbol());
    writer.writeTextElement(QString("shipping_address_street"), account.shippingAddressStreet());
    writer.writeTextElement(QString("shipping_address_city"), account.shippingAddressCity());
    writer.writeTextElement(QString("shipping_address_state"), account.shippingAddressState());
    writer.writeTextElement(QString("shipping_address_postalcode"), account.shippingAddressPostalcode());
    writer.writeTextElement(QString("shipping_address_country"), account.shippingAddressCountry());
    writer.writeTextElement(QString("email1"), account.email1());
    writer.writeTextElement(QString("parent_id"), account.parentId());
    writer.writeTextElement(QString("sic_code"), account.sicCode());
    writer.writeTextElement(QString("parent_name"), account.parentName());
    writer.writeTextElement(QString("campaign_id"), account.campaignId());
    writer.writeTextElement(QString("campaign_name"), account.campaignName());
    writer.writeEndDocument();

    return true;
}

