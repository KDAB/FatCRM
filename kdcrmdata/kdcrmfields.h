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

#ifndef KDCRMFIELDS_H
#define KDCRMFIELDS_H

#include "kdcrmdata_export.h"
#include <QString>

// Name of the elements in the XML serialization, and in the GUI (i.e. in the QMap<QString,QString> data)
// Not necesssarily the same as in the sugar soap, although right now AccessorPair doesn't support
// a different naming between soap and xml (could be useful later though, e.g. for salesforce support).
// So don't use this as the soap name (the key in the AccessorPair)
namespace KDCRMFields
{
    KDCRMDATA_EXPORT QString assignedUserName();
    KDCRMDATA_EXPORT QString dateModified();
    KDCRMDATA_EXPORT QString dateEntered();
    KDCRMDATA_EXPORT QString id();
    KDCRMDATA_EXPORT QString name();
    KDCRMDATA_EXPORT QString modifiedUserId();
    KDCRMDATA_EXPORT QString modifiedByName();
    KDCRMDATA_EXPORT QString createdBy();
    KDCRMDATA_EXPORT QString createdByName();
    KDCRMDATA_EXPORT QString assignedUserId();
    KDCRMDATA_EXPORT QString accountName();
    KDCRMDATA_EXPORT QString campaignName();
    KDCRMDATA_EXPORT QString campaignType();
    KDCRMDATA_EXPORT QString campaignId();
    KDCRMDATA_EXPORT QString status();
    KDCRMDATA_EXPORT QString parentName();
    KDCRMDATA_EXPORT QString tickerSymbol();
    KDCRMDATA_EXPORT QString campaign();
    KDCRMDATA_EXPORT QString reportsTo();
    KDCRMDATA_EXPORT QString parentId();
    KDCRMDATA_EXPORT QString accountId();
    KDCRMDATA_EXPORT QString reportsToId();
    KDCRMDATA_EXPORT QString opportunityType();
    KDCRMDATA_EXPORT QString description();
    KDCRMDATA_EXPORT QString descriptionHtml();
    KDCRMDATA_EXPORT QString deleted();
    KDCRMDATA_EXPORT QString content();
    KDCRMDATA_EXPORT QString leadSource();
    KDCRMDATA_EXPORT QString amount();
    KDCRMDATA_EXPORT QString amountUsDollar();
    KDCRMDATA_EXPORT QString contactId();
    KDCRMDATA_EXPORT QString currencyId();
    KDCRMDATA_EXPORT QString currencyName();
    KDCRMDATA_EXPORT QString currencySymbol();
    KDCRMDATA_EXPORT QString dateClosed();
    KDCRMDATA_EXPORT QString nextStep();
    KDCRMDATA_EXPORT QString salesStage();
    KDCRMDATA_EXPORT QString probability();
    KDCRMDATA_EXPORT QString nextCallDate();
    KDCRMDATA_EXPORT QString billingAddressStreet();
    KDCRMDATA_EXPORT QString billingAddressCity();
    KDCRMDATA_EXPORT QString billingAddressState();
    KDCRMDATA_EXPORT QString billingAddressCountry();
    KDCRMDATA_EXPORT QString billingAddressPostalcode();
    KDCRMDATA_EXPORT QString shippingAddressStreet();
    KDCRMDATA_EXPORT QString shippingAddressCity();
    KDCRMDATA_EXPORT QString shippingAddressState();
    KDCRMDATA_EXPORT QString shippingAddressCountry();
    KDCRMDATA_EXPORT QString shippingAddressPostalcode();
    KDCRMDATA_EXPORT QString industry();
    KDCRMDATA_EXPORT QString accountType();
    KDCRMDATA_EXPORT QString opportunityPriority();
    KDCRMDATA_EXPORT QString opportunitySize();
    KDCRMDATA_EXPORT QString primaryContactId();

    // for Contacts
    KDCRMDATA_EXPORT QString primaryAddressStreet();
    KDCRMDATA_EXPORT QString primaryAddressCity();
    KDCRMDATA_EXPORT QString primaryAddressState();
    KDCRMDATA_EXPORT QString primaryAddressPostalcode();
    KDCRMDATA_EXPORT QString primaryAddressCountry();
    KDCRMDATA_EXPORT QString altAddressStreet();
    KDCRMDATA_EXPORT QString altAddressCity();
    KDCRMDATA_EXPORT QString altAddressState();
    KDCRMDATA_EXPORT QString altAddressPostalcode();
    KDCRMDATA_EXPORT QString altAddressCountry();
    KDCRMDATA_EXPORT QString firstName();
    KDCRMDATA_EXPORT QString lastName();
    KDCRMDATA_EXPORT QString title();
    KDCRMDATA_EXPORT QString department();
    KDCRMDATA_EXPORT QString email1();
    KDCRMDATA_EXPORT QString email2();
    KDCRMDATA_EXPORT QString phoneHome();
    KDCRMDATA_EXPORT QString phoneMobile();
    KDCRMDATA_EXPORT QString phoneWork();
    KDCRMDATA_EXPORT QString phoneOther();
    KDCRMDATA_EXPORT QString phoneFax();
    KDCRMDATA_EXPORT QString birthdate();
    KDCRMDATA_EXPORT QString assistant();
    KDCRMDATA_EXPORT QString phoneAssistant();
    KDCRMDATA_EXPORT QString salutation();
    KDCRMDATA_EXPORT QString doNotCall();
    KDCRMDATA_EXPORT QString invalidEmail();
    KDCRMDATA_EXPORT QString cAcceptStatusFields();
    KDCRMDATA_EXPORT QString mAcceptStatusFields();
    KDCRMDATA_EXPORT QString opportunityRoleFields();

    KDCRMDATA_EXPORT QString accountDescription();
    KDCRMDATA_EXPORT QString accounting();
    KDCRMDATA_EXPORT QString accountPhoneOther();
    KDCRMDATA_EXPORT QString accountPhoneWork();
    KDCRMDATA_EXPORT QString actualCost();
    KDCRMDATA_EXPORT QString annualRevenue();
    KDCRMDATA_EXPORT QString budget();
    KDCRMDATA_EXPORT QString ccAddrsNames();
    KDCRMDATA_EXPORT QString contactName();
    KDCRMDATA_EXPORT QString converted();
    KDCRMDATA_EXPORT QString dateDue();
    KDCRMDATA_EXPORT QString dateDueFlag();
    KDCRMDATA_EXPORT QString dateSent();
    KDCRMDATA_EXPORT QString dateStart();
    KDCRMDATA_EXPORT QString dateStartFlag();
    KDCRMDATA_EXPORT QString employees();
    KDCRMDATA_EXPORT QString endDate();
    KDCRMDATA_EXPORT QString expectedCost();
    KDCRMDATA_EXPORT QString expectedRevenue();
    KDCRMDATA_EXPORT QString fileMimeType();
    KDCRMDATA_EXPORT QString fileName();
    KDCRMDATA_EXPORT QString frequency();
    KDCRMDATA_EXPORT QString fromAddrName();
    KDCRMDATA_EXPORT QString impressions();
    KDCRMDATA_EXPORT QString leadSourceDescription();
    KDCRMDATA_EXPORT QString messageId();
    KDCRMDATA_EXPORT QString objective();
    KDCRMDATA_EXPORT QString opportunityAmount();
    KDCRMDATA_EXPORT QString opportunityId();
    KDCRMDATA_EXPORT QString opportunityName();
    KDCRMDATA_EXPORT QString ownership();
    KDCRMDATA_EXPORT QString parentType();
    KDCRMDATA_EXPORT QString portalApp();
    KDCRMDATA_EXPORT QString portalName();
    KDCRMDATA_EXPORT QString priority();
    KDCRMDATA_EXPORT QString rating();
    KDCRMDATA_EXPORT QString referedBy();
    KDCRMDATA_EXPORT QString referUrl();
    KDCRMDATA_EXPORT QString sicCode();
    KDCRMDATA_EXPORT QString startDate();
    KDCRMDATA_EXPORT QString statusDescription();
    KDCRMDATA_EXPORT QString trackerCount();
    KDCRMDATA_EXPORT QString trackerKey();
    KDCRMDATA_EXPORT QString trackerText();
    KDCRMDATA_EXPORT QString toAddrsNames();
    KDCRMDATA_EXPORT QString vatNo();
    KDCRMDATA_EXPORT QString vismaId();
    KDCRMDATA_EXPORT QString website();

    KDCRMDATA_EXPORT QString documentName();
    KDCRMDATA_EXPORT QString docId();
    KDCRMDATA_EXPORT QString docType();
    KDCRMDATA_EXPORT QString docUrl();
    KDCRMDATA_EXPORT QString activeDate();
    KDCRMDATA_EXPORT QString expDate();
    KDCRMDATA_EXPORT QString categoryId();
    KDCRMDATA_EXPORT QString subcategoryId();
    KDCRMDATA_EXPORT QString statusId();
    KDCRMDATA_EXPORT QString documentRevisionId();
    KDCRMDATA_EXPORT QString relatedDocId();
    KDCRMDATA_EXPORT QString relatedDocName();
    KDCRMDATA_EXPORT QString relatedDocRevId();
    KDCRMDATA_EXPORT QString isTemplate();
    KDCRMDATA_EXPORT QString templateType();
}

#endif // KDCRMFIELDS_H

