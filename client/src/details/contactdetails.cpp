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

#include "contactdetails.h"

#include "editcalendarbutton.h"
#include "ui_contactdetails.h"
#include "referenceddatamodel.h"
#include "kdcrmutils.h"

#include <kabc/address.h>
#include <kabc/addressee.h>

ContactDetails::ContactDetails(QWidget *parent)
    : Details(Contact, parent), mUi(new Ui::ContactDetails)
{
    mUi->setupUi(this);
    mUi->urllabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    initialize();
}

ContactDetails::~ContactDetails()
{
    delete mUi;
}

void ContactDetails::initialize()
{
    mUi->salutation->addItems(salutationItems());
    ReferencedDataModel::setModelForCombo(mUi->accountName, AccountRef);
    mUi->leadSource->addItems(sourceItems());
    ReferencedDataModel::setModelForCombo(mUi->campaign, CampaignRef);
    ReferencedDataModel::setModelForCombo(mUi->reportsTo, ReportsToRef);
    ReferencedDataModel::setModelForCombo(mUi->assignedTo, AssignedToRef);

    connect(mUi->clearDateButton, SIGNAL(clicked()), this, SLOT(slotClearDate()));
    connect(mUi->calendarButton->calendarWidget(), SIGNAL(clicked(QDate)),
            this, SLOT(slotSetBirthday()));
}

void ContactDetails::slotSetBirthday()
{
    // TODO FIXME: use QDateEdit [or remove - who cares about birthdays...]
    mUi->birthdate->setText(KDCRMUtils::dateToString(mUi->calendarButton->calendarWidget()->selectedDate()));
    mUi->calendarButton->calendarWidget()->setSelectedDate(QDate::currentDate());
    mUi->calendarButton->calendarDialog()->close();
}

void ContactDetails::slotClearDate()
{
    mUi->birthdate->clear();
}

QMap<QString, QString> ContactDetails::data(const Akonadi::Item &item) const
{
    KABC::Addressee contact = item.payload<KABC::Addressee>();
    return contactData(contact);
}

QMap<QString, QString> ContactDetails::contactData(const KABC::Addressee &addressee) const
{
    QMap<QString, QString> data;
    data["salutation"] = addressee.custom("FATCRM", "X-Salutation");
    data["firstName"] = addressee.givenName();
    data["lastName"] = addressee.familyName();
    data["title"] = addressee.title();
    data["department"] = addressee.department();
    data["accountName"] = addressee.organization();
    data["email1"] = addressee.preferredEmail();
    QStringList emails = addressee.emails();
    emails.removeAll(addressee.preferredEmail());
    if (emails.count() > 0) {
        data["email2"] = emails.at(0);
    }
    data["phoneHome"] = addressee.phoneNumber(KABC::PhoneNumber::Home).number();
    data["phoneMobile"] = addressee.phoneNumber(KABC::PhoneNumber::Cell).number();
    data["phoneWork"] = addressee.phoneNumber(KABC::PhoneNumber::Work).number();
    data["phoneOther"] = addressee.phoneNumber(KABC::PhoneNumber::Car).number();
    data["phoneFax"] = addressee.phoneNumber(KABC::PhoneNumber::Fax).number();

    const KABC::Address address = addressee.address(KABC::Address::Work | KABC::Address::Pref);
    data["primaryAddressStreet"] = address.street();
    data["primaryAddressCity"] = address.locality();
    data["primaryAddressState"] = address.region();
    data["primaryAddressPostalcode"] = address.postalCode();
    data["primaryAddressCountry"] = address.country();

    const KABC::Address other = addressee.address(KABC::Address::Home);
    data["altAddressStreet"] = other.street();
    data["altAddressCity"] = other.locality();
    data["altAddressState"] = other.region();
    data["altAddressPostalcode"] = other.postalCode();
    data["altAddressCountry"] = other.country();
    data["birthdate"] = KDCRMUtils::dateToString(addressee.birthday().date());
    data["assistant"] = addressee.custom("KADDRESSBOOK", "X-AssistantsName");
    data["phoneAssistant"] = addressee.custom("FATCRM", "X-AssistantsPhone");
    data["leadSource"] = addressee.custom("FATCRM", "X-LeadSourceName");
    data["campaign"] = addressee.custom("FATCRM", "X-CampaignName");
    data["assignedTo"] = addressee.custom("FATCRM", "X-AssignedUserName");
    data["reportsTo"] = addressee.custom("FATCRM", "X-ReportsToUserName");
    data["doNotCall"] = addressee.custom("FATCRM", "X-DoNotCall");
    data["description"] = addressee.note();
    data["modifiedBy"] = addressee.custom("FATCRM", "X-ModifiedByName");
    data["dateModified"] = addressee.custom("FATCRM", "X-DateModified");
    data["dateEntered"] = addressee.custom("FATCRM", "X-DateCreated");
    data["createdBy"] = addressee.custom("FATCRM", "X-CreatedByName");
    data["modifiedUserId"] = addressee.custom("FATCRM", "X-ModifiedUserId");
    data["modifiedUserName"] = addressee.custom("FATCRM", "X-ModifiedUserName");
    data["id"] = addressee.custom("FATCRM", "X-ContactId");
    data["opportunityRoleFields"] = addressee.custom("FATCRM", "X-OpportunityRoleFields");
    data["cAcceptStatusFields"] = addressee.custom("FATCRM", "X-CacceptStatusFields");
    data["mAcceptStatusFields"] = addressee.custom("FATCRM", "X-MacceptStatusFields");
    data["deleted"] = addressee.custom("FATCRM", "X-Deleted");
    data["createdById"] = addressee.custom("FATCRM", "X-CreatedById");
    return data;
}

void ContactDetails::updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const
{
    KABC::Addressee addressee;
    if (item.hasPayload<KABC::Addressee>()) {
        addressee = item.payload<KABC::Addressee>();
    }

    addressee.setGivenName(data.value("firstName"));
    addressee.setFamilyName(data.value("lastName"));
    addressee.setTitle(data.value("title"));
    addressee.setDepartment(data.value("department"));
    addressee.setOrganization(data.value("accountName"));
    addressee.insertCustom("FATCRM", "X-AccountId", data.value("accountId"));
    addressee.insertEmail(data.value("email1"), true);
    addressee.insertEmail(data.value("email2"));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value("phoneHome"), KABC::PhoneNumber::Home));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value("phoneMobile"), KABC::PhoneNumber::Cell));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value("phoneWork"), KABC::PhoneNumber::Work));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value("phoneOther"), KABC::PhoneNumber::Car));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value("phoneFax"), KABC::PhoneNumber::Work | KABC::PhoneNumber::Fax));

    KABC::Address primaryAddress;
    primaryAddress.setType(KABC::Address::Work | KABC::Address::Pref);
    primaryAddress.setStreet(data.value("primaryAddressStreet"));
    primaryAddress.setLocality(data.value("primaryAddressCity"));
    primaryAddress.setRegion(data.value("primaryAddressState"));
    primaryAddress.setPostalCode(data.value("primaryAddressPostalcode"));
    primaryAddress.setCountry(data.value("primaryAddressCountry"));
    addressee.insertAddress(primaryAddress);

    KABC::Address otherAddress;
    otherAddress.setType(KABC::Address::Home);
    otherAddress.setStreet(data.value("altAddressStreet"));
    otherAddress.setLocality(data.value("altAddressCity"));
    otherAddress.setRegion(data.value("altAddressState"));
    otherAddress.setPostalCode(data.value("altAddressPostalcode"));
    otherAddress.setCountry(data.value("altAddressCountry"));
    addressee.insertAddress(otherAddress);

    addressee.setBirthday(QDateTime(KDCRMUtils::dateFromString(data.value("birthdate"))));

    addressee.setNote(data.value("description"));
    addressee.insertCustom("KADDRESSBOOK", "X-AssistantsName", data.value("assistant"));
    addressee.insertCustom("FATCRM", "X-AssistantsPhone", data.value("phoneAssistant"));
    addressee.insertCustom("FATCRM", "X-LeadSourceName", data.value("leadSource"));
    addressee.insertCustom("FATCRM", "X-CampaignName", data.value("campaign"));
    addressee.insertCustom("FATCRM", "X-CampaignId", data.value("campaignId"));
    addressee.insertCustom("FATCRM", "X-CacceptStatusFields", data.value("cAcceptStatusFields"));
    addressee.insertCustom("FATCRM", "X-MacceptStatusFields", data.value("mAcceptStatusFields"));
    addressee.insertCustom("FATCRM", "X-AssignedUserName", data.value("assignedTo"));
    addressee.insertCustom("FATCRM", "X-AssignedUserId", data.value("assignedToId"));
    addressee.insertCustom("FATCRM", "X-ReportsToUserName", data.value("reportsTo"));
    addressee.insertCustom("FATCRM", "X-ReportsToUserId", data.value("reportsToId"));
    addressee.insertCustom("FATCRM", "X-OpportunityRoleFields", data.value("opportunityRoleFields"));
    addressee.insertCustom("FATCRM", "X-ModifiedByName", data.value("modifiedBy"));
    addressee.insertCustom("FATCRM", "X-DateModified", data.value("modifiedDate"));
    addressee.insertCustom("FATCRM", "X-ModifiedUserId", data.value("modifiedUserId"));
    addressee.insertCustom("FATCRM", "X-ModifiedUserName", data.value("modifiedUserName"));
    addressee.insertCustom("FATCRM", "X-DateCreated", data.value("createdDate"));
    addressee.insertCustom("FATCRM", "X-CreatedByName", data.value("createdBy"));
    addressee.insertCustom("FATCRM", "X-CreatedById", data.value("createdById"));
    addressee.insertCustom("FATCRM", "X-ContactId", data.value("id"));
    addressee.insertCustom("FATCRM", "X-Salutation", data.value("salutation"));
    addressee.insertCustom("FATCRM", "X-Deleted", data.value("deleted"));
    addressee.insertCustom("FATCRM", "X-DoNotCall", data.value("doNotCall"));

    item.setMimeType(KABC::Addressee::mimeType());
    item.setPayload<KABC::Addressee>(addressee);
}

void ContactDetails::setDataInternal(const QMap<QString, QString> &) const
{
    const QString baseUrl = resourceBaseUrl();
    if (!baseUrl.isEmpty() && !id().isEmpty()) {
        const QString url = baseUrl + "?action=DetailView&module=Contacts&record=" + id();
        mUi->urllabel->setText(QString("<a href=\"%1\">Open Contact in Web Browser</a>").arg(url));
    }
}
