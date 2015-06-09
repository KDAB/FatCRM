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
#include "kdcrmfields.h"

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
    ReferencedDataModel::setModelForCombo(mUi->account_name, AccountRef);
    mUi->lead_source->addItems(sourceItems());
    ReferencedDataModel::setModelForCombo(mUi->campaign, CampaignRef);
    ReferencedDataModel::setModelForCombo(mUi->reports_to, ReportsToRef);
    ReferencedDataModel::setModelForCombo(mUi->assigned_to, AssignedToRef);

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
    data[KDCRMFields::salutation()] = addressee.custom("FATCRM", "X-Salutation");
    data[KDCRMFields::firstName()] = addressee.givenName();
    data[KDCRMFields::lastName()] = addressee.familyName();
    data[KDCRMFields::title()] = addressee.title();
    data[KDCRMFields::department()] = addressee.department();
    data[KDCRMFields::accountName()] = addressee.organization();
    data[KDCRMFields::email1()] = addressee.preferredEmail();
    QStringList emails = addressee.emails();
    emails.removeAll(addressee.preferredEmail());
    if (emails.count() > 0) {
        data[KDCRMFields::email2()] = emails.at(0);
    }
    data[KDCRMFields::phoneHome()] = addressee.phoneNumber(KABC::PhoneNumber::Home).number();
    data[KDCRMFields::phoneMobile()] = addressee.phoneNumber(KABC::PhoneNumber::Cell).number();
    data[KDCRMFields::phoneWork()] = addressee.phoneNumber(KABC::PhoneNumber::Work).number();
    data[KDCRMFields::phoneOther()] = addressee.phoneNumber(KABC::PhoneNumber::Car).number();
    data[KDCRMFields::phoneFax()] = addressee.phoneNumber(KABC::PhoneNumber::Fax).number();

    const KABC::Address address = addressee.address(KABC::Address::Work | KABC::Address::Pref);
    data.insert(KDCRMFields::primaryAddressStreet(), address.street());
    data.insert(KDCRMFields::primaryAddressCity(), address.locality());
    data.insert(KDCRMFields::primaryAddressState(), address.region());
    data.insert(KDCRMFields::primaryAddressPostalcode(), address.postalCode());
    data.insert(KDCRMFields::primaryAddressCountry(), address.country());

    const KABC::Address other = addressee.address(KABC::Address::Home);
    data.insert(KDCRMFields::altAddressStreet(), other.street());
    data.insert(KDCRMFields::altAddressCity(), other.locality());
    data.insert(KDCRMFields::altAddressState(), other.region());
    data.insert(KDCRMFields::altAddressPostalcode(), other.postalCode());
    data.insert(KDCRMFields::altAddressCountry(), other.country());
    data[KDCRMFields::birthdate()] = KDCRMUtils::dateToString(addressee.birthday().date());
    data[KDCRMFields::assistant()] = addressee.custom("KADDRESSBOOK", "X-AssistantsName");
    data[KDCRMFields::phoneAssistant()] = addressee.custom("FATCRM", "X-AssistantsPhone");
    data[KDCRMFields::leadSource()] = addressee.custom("FATCRM", "X-LeadSourceName");
    data[KDCRMFields::campaign()] = addressee.custom("FATCRM", "X-CampaignName");
    data[KDCRMFields::assignedTo()] = addressee.custom("FATCRM", "X-AssignedUserName");
    data[KDCRMFields::reportsTo()] = addressee.custom("FATCRM", "X-ReportsToUserName");
    data[KDCRMFields::doNotCall()] = addressee.custom("FATCRM", "X-DoNotCall");
    data[KDCRMFields::description()] = addressee.note();
    data[KDCRMFields::modifiedByName()] = addressee.custom("FATCRM", "X-ModifiedByName");
    data[KDCRMFields::dateModified()] = addressee.custom("FATCRM", "X-DateModified");
    data[KDCRMFields::dateEntered()] = addressee.custom("FATCRM", "X-DateCreated");
    data[KDCRMFields::createdByName()] = addressee.custom("FATCRM", "X-CreatedByName");
    data[KDCRMFields::modifiedUserId()] = addressee.custom("FATCRM", "X-ModifiedUserId");
    data[KDCRMFields::id()] = addressee.custom("FATCRM", "X-ContactId");
    data[KDCRMFields::opportunityRoleFields()] = addressee.custom("FATCRM", "X-OpportunityRoleFields");
    data[KDCRMFields::cAcceptStatusFields()] = addressee.custom("FATCRM", "X-CacceptStatusFields");
    data[KDCRMFields::mAcceptStatusFields()] = addressee.custom("FATCRM", "X-MacceptStatusFields");
    data[KDCRMFields::deleted()] = addressee.custom("FATCRM", "X-Deleted");
    data[KDCRMFields::createdBy()] = addressee.custom("FATCRM", "X-CreatedById");
    return data;
}

void ContactDetails::updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const
{
    KABC::Addressee addressee;
    if (item.hasPayload<KABC::Addressee>()) {
        addressee = item.payload<KABC::Addressee>();
        foreach (const KABC::Address &addr, addressee.addresses())
            addressee.removeAddress(addr);
    }

    addressee.setGivenName(data.value(KDCRMFields::firstName()));
    addressee.setFamilyName(data.value(KDCRMFields::lastName()));
    addressee.setTitle(data.value(KDCRMFields::title()));
    addressee.setDepartment(data.value(KDCRMFields::department()));
    addressee.setOrganization(data.value(KDCRMFields::accountName()));
    addressee.insertCustom("FATCRM", "X-AccountId", data.value(KDCRMFields::accountId()));
    addressee.insertEmail(data.value(KDCRMFields::email1()), true);
    addressee.insertEmail(data.value(KDCRMFields::email2()));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value(KDCRMFields::phoneHome()), KABC::PhoneNumber::Home));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value(KDCRMFields::phoneMobile()), KABC::PhoneNumber::Cell));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value(KDCRMFields::phoneWork()), KABC::PhoneNumber::Work));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value(KDCRMFields::phoneOther()), KABC::PhoneNumber::Car));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value(KDCRMFields::phoneFax()), KABC::PhoneNumber::Work | KABC::PhoneNumber::Fax));

    KABC::Address primaryAddress;
    primaryAddress.setType(KABC::Address::Work | KABC::Address::Pref);
    primaryAddress.setStreet(data.value(KDCRMFields::primaryAddressStreet()));
    primaryAddress.setLocality(data.value(KDCRMFields::primaryAddressCity()));
    primaryAddress.setRegion(data.value(KDCRMFields::primaryAddressState()));
    primaryAddress.setPostalCode(data.value(KDCRMFields::primaryAddressPostalcode()));
    primaryAddress.setCountry(data.value(KDCRMFields::primaryAddressCountry()));
    addressee.insertAddress(primaryAddress);

    KABC::Address otherAddress;
    otherAddress.setType(KABC::Address::Home);
    otherAddress.setStreet(data.value(KDCRMFields::altAddressStreet()));
    otherAddress.setLocality(data.value(KDCRMFields::altAddressCity()));
    otherAddress.setRegion(data.value(KDCRMFields::altAddressState()));
    otherAddress.setPostalCode(data.value(KDCRMFields::altAddressPostalcode()));
    otherAddress.setCountry(data.value(KDCRMFields::altAddressCountry()));
    addressee.insertAddress(otherAddress);

    addressee.setBirthday(QDateTime(KDCRMUtils::dateFromString(data.value(KDCRMFields::birthdate()))));

    addressee.setNote(data.value(KDCRMFields::description()));
    addressee.insertCustom("KADDRESSBOOK", "X-AssistantsName", data.value(KDCRMFields::assistant()));
    addressee.insertCustom("FATCRM", "X-AssistantsPhone", data.value(KDCRMFields::phoneAssistant()));
    addressee.insertCustom("FATCRM", "X-LeadSourceName", data.value(KDCRMFields::leadSource()));
    addressee.insertCustom("FATCRM", "X-CampaignName", data.value(KDCRMFields::campaign()));
    addressee.insertCustom("FATCRM", "X-CampaignId", data.value(KDCRMFields::campaignId()));
    addressee.insertCustom("FATCRM", "X-CacceptStatusFields", data.value(KDCRMFields::cAcceptStatusFields()));
    addressee.insertCustom("FATCRM", "X-MacceptStatusFields", data.value(KDCRMFields::mAcceptStatusFields()));
    addressee.insertCustom("FATCRM", "X-AssignedUserName", data.value(KDCRMFields::assignedTo()));
    addressee.insertCustom("FATCRM", "X-AssignedUserId", data.value(KDCRMFields::assignedToId()));
    addressee.insertCustom("FATCRM", "X-ReportsToUserName", data.value(KDCRMFields::reportsTo()));
    addressee.insertCustom("FATCRM", "X-ReportsToUserId", data.value(KDCRMFields::reportsToId()));
    addressee.insertCustom("FATCRM", "X-OpportunityRoleFields", data.value(KDCRMFields::opportunityRoleFields()));
    addressee.insertCustom("FATCRM", "X-ModifiedByName", data.value(KDCRMFields::modifiedByName()));
    addressee.insertCustom("FATCRM", "X-DateModified", data.value(KDCRMFields::dateModified()));
    addressee.insertCustom("FATCRM", "X-ModifiedUserId", data.value(KDCRMFields::modifiedUserId()));
    addressee.insertCustom("FATCRM", "X-DateCreated", data.value(KDCRMFields::dateEntered()));
    addressee.insertCustom("FATCRM", "X-CreatedByName", data.value(KDCRMFields::createdByName()));
    addressee.insertCustom("FATCRM", "X-CreatedById", data.value(KDCRMFields::createdBy()));
    addressee.insertCustom("FATCRM", "X-ContactId", data.value(KDCRMFields::id()));
    addressee.insertCustom("FATCRM", "X-Salutation", data.value(KDCRMFields::salutation()));
    addressee.insertCustom("FATCRM", "X-Deleted", data.value(KDCRMFields::deleted()));
    addressee.insertCustom("FATCRM", "X-DoNotCall", data.value(KDCRMFields::doNotCall()));

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

void ContactDetails::getDataInternal(QMap<QString, QString> &currentData) const
{
    currentData[KDCRMFields::assignedToId()] = currentAssignedToId();
    currentData[KDCRMFields::reportsToId()] = currentReportsToId();
    currentData[KDCRMFields::accountId()] = currentAccountId();
    currentData[KDCRMFields::campaignId()] = currentCampaignId();

}
