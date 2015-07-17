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

#include "contactshandler.h"

#include "sugarsession.h"
#include "sugarsoap.h"
#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/kdcrmfields.h"

using namespace KDSoapGenerated;
#include <Akonadi/Contact/ContactParts>

#include <AkonadiCore/abstractdifferencesreporter.h> //krazy:exclude=camelcase
#include <AkonadiCore/Collection>

#include <KContacts/Addressee>
#include <KContacts/Address>

#include <KLocale>

#include <QHash>

typedef QString(*valueGetter)(const KContacts::Addressee &);
typedef void (*valueSetter)(const QString &, KContacts::Addressee &);
typedef void (*addressSetter)(const QString &, KContacts::Address &);

static QString getFirstName(const KContacts::Addressee &addressee)
{
    return addressee.givenName();
}

static void setFirstName(const QString &value, KContacts::Addressee &addressee)
{
    addressee.setGivenName(value);
}

static QString getLastName(const KContacts::Addressee &addressee)
{
    return addressee.familyName();
}

static void setLastName(const QString &value, KContacts::Addressee &addressee)
{
    addressee.setFamilyName(value);
}

static QString getTitle(const KContacts::Addressee &addressee)
{
    return addressee.title();
}

static void setTitle(const QString &value, KContacts::Addressee &addressee)
{
    addressee.setTitle(value);
}

static QString getDepartment(const KContacts::Addressee &addressee)
{
    return addressee.department();
}

static void setDepartment(const QString &value, KContacts::Addressee &addressee)
{
    addressee.setDepartment(value);
}

static QString getAccountName(const KContacts::Addressee &addressee)
{
    return addressee.organization();
}

static void setAccountName(const QString &value, KContacts::Addressee &addressee)
{
    addressee.setOrganization(value);
}

static QString getAccountId(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-AccountId");
}

static void setAccountId(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-AccountId", value);
}

static QString getAssistantName(const KContacts::Addressee &addressee)
{
    return addressee.custom("KADDRESSBOOK", "X-AssistantsName");
}

static void setAssistantName(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("KADDRESSBOOK", "X-AssistantsName", value);
}

static QString getAssistantPhone(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-AssistantsPhone");
}

static void setAssistantPhone(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-AssistantsPhone", value);
}

static QString getLeadSourceName(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-LeadSourceName");
}

static void setLeadSourceName(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-LeadSourceName", value);
}

static QString getCampaignName(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-CampaignName");
}

static void setCampaignName(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-CampaignName", value);
}

static QString getCampaignId(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-CampaignId");
}

static void setCampaignId(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-CampaignId", value);
}

static QString getAssignedUserName(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-AssignedUserName");
}

static void setAssignedUserName(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-AssignedUserName", value);
}

static QString getAssignedUserId(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-AssignedUserId");
}

static void setAssignedUserId(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-AssignedUserId", value);
}

static QString getReportsToUserName(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-ReportsToUserName");
}

static void setReportsToUserName(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-ReportsToUserName", value);
}

static QString getReportsToUserId(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-ReportsToUserId");
}

static void setReportsToUserId(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-ReportsToUserId", value);
}

static QString getModifiedByName(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-ModifiedByName");
}

static void setModifiedByName(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-ModifiedByName", value);
}

static QString getDateModified(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-DateModified");
}

static void setDateModified(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-DateModified", value);
}

static QString getModifiedUserId(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-ModifiedUserId");
}

static void setModifiedUserId(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-ModifiedUserId", value);
}

static QString getDateCreated(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-DateCreated");
}

static void setDateCreated(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-DateCreated", value);
}

static void setContactId(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-ContactId", value);
}

static QString getCreatedByName(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-CreatedByName");
}

static void setCreatedByName(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-CreatedByName", value);
}

static QString getCreatedById(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-CreatedById");
}

static void setCreatedById(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-CreatedById", value);
}

static QString getSalutation(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-Salutation");
}

static void setSalutation(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-Salutation", value);
}

static QString getOpportunityRoleFields(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-OpportunityRoleFields");
}

static void setOpportunityRoleFields(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-OpportunityRoleFields", value);
}

static QString getCAcceptStatusFields(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-CacceptStatusFields");
}

static void setCAcceptStatusFields(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-CacceptStatusFields", value);
}

static QString getMAcceptStatusFields(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-MacceptStatusFields");
}

static void setMAcceptStatusFields(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-MacceptStatusFields", value);
}

static QString getDeleted(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-Deleted");
}

static void setDeleted(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-Deleted", value);
}

static QString getDoNotCall(const KContacts::Addressee &addressee)
{
    return addressee.custom("FATCRM", "X-DoNotCall");
}

static void setDoNotCall(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertCustom("FATCRM", "X-DoNotCall", value);
}

static QString getNote(const KContacts::Addressee &addressee)
{
    return addressee.note();
}

static void setNote(const QString &value, KContacts::Addressee &addressee)
{
    addressee.setNote(value);
}

static QString getBirthday(const KContacts::Addressee &addressee)
{
    return KDCRMUtils::dateToString(addressee.birthday().date());
}

static void setBirthday(const QString &value, KContacts::Addressee &addressee)
{
    addressee.setBirthday(QDateTime(KDCRMUtils::dateFromString(value)));
}

static QString getEmail1(const KContacts::Addressee &addressee)
{
    return addressee.preferredEmail();
}

static void setEmail1(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertEmail(value, true);
}

static QString getEmail2(const KContacts::Addressee &addressee)
{
    // preferred might not be the first one, so remove it and take the first of
    // the remaining instead of always taking the second one
    QStringList emails = addressee.emails();
    emails.removeAll(addressee.preferredEmail());
    if (emails.count() >= 1) {
        return emails[ 0 ];
    }

    return QString();
}

static void setEmail2(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertEmail(value, false);
}

static QString getHomePhone(const KContacts::Addressee &addressee)
{
    return addressee.phoneNumber(KContacts::PhoneNumber::Home).number();
}

static void setHomePhone(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Home));
}

static QString getWorkPhone(const KContacts::Addressee &addressee)
{
    return addressee.phoneNumber(KContacts::PhoneNumber::Work).number();
}

static void setWorkPhone(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Work));
}

static QString getMobilePhone(const KContacts::Addressee &addressee)
{
    return addressee.phoneNumber(KContacts::PhoneNumber::Cell).number();
}

static void setMobilePhone(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Cell));
}

// Pending (michel)
// we need to decide what other phone will be
static QString getOtherPhone(const KContacts::Addressee &addressee)
{
    return addressee.phoneNumber(KContacts::PhoneNumber::Car).number();
}

static void setOtherPhone(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Car));
}

static QString getFaxPhone(const KContacts::Addressee &addressee)
{
    return addressee.phoneNumber(KContacts::PhoneNumber::Work | KContacts::PhoneNumber::Fax).number();
}

static void setFaxPhone(const QString &value, KContacts::Addressee &addressee)
{
    addressee.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Work | KContacts::PhoneNumber::Fax));
}

static QString getPrimaryStreet(const KContacts::Addressee &addressee)
{
    return addressee.address(KContacts::Address::Work).street();
}

static void setPrimaryStreet(const QString &value, KContacts::Address &address)
{
    address.setStreet(value);
}

static QString getPrimaryCity(const KContacts::Addressee &addressee)
{
    return addressee.address(KContacts::Address::Work | KContacts::Address::Pref).locality();
}

static void setPrimaryCity(const QString &value, KContacts::Address &address)
{
    address.setLocality(value);
}

static QString getPrimaryState(const KContacts::Addressee &addressee)
{
    return addressee.address(KContacts::Address::Work | KContacts::Address::Pref).region();
}

static void setPrimaryState(const QString &value, KContacts::Address &address)
{
    address.setRegion(value);
}

static QString getPrimaryPostalcode(const KContacts::Addressee &addressee)
{
    return addressee.address(KContacts::Address::Work | KContacts::Address::Pref).postalCode();
}

static void setPrimaryPostalcode(const QString &value, KContacts::Address &address)
{
    address.setPostalCode(value);
}

static QString getPrimaryCountry(const KContacts::Addressee &addressee)
{
    return addressee.address(KContacts::Address::Work | KContacts::Address::Pref).country();
}

static void setPrimaryCountry(const QString &value, KContacts::Address &address)
{
    address.setCountry(value);
}

static QString getOtherStreet(const KContacts::Addressee &addressee)
{
    return addressee.address(KContacts::Address::Home).street();
}

static void setOtherStreet(const QString &value, KContacts::Address &address)
{
    address.setStreet(value);
}

static QString getOtherCity(const KContacts::Addressee &addressee)
{
    return addressee.address(KContacts::Address::Home).locality();
}

static void setOtherCity(const QString &value, KContacts::Address &address)
{
    address.setLocality(value);
}

static QString getOtherState(const KContacts::Addressee &addressee)
{
    return addressee.address(KContacts::Address::Home).region();
}

static void setOtherState(const QString &value, KContacts::Address &address)
{
    address.setRegion(value);
}

static QString getOtherPostalcode(const KContacts::Addressee &addressee)
{
    return addressee.address(KContacts::Address::Home).postalCode();
}

static void setOtherPostalcode(const QString &value, KContacts::Address &address)
{
    address.setPostalCode(value);
}

static QString getOtherCountry(const KContacts::Addressee &addressee)
{
    return addressee.address(KContacts::Address::Home).country();
}

static void setOtherCountry(const QString &value, KContacts::Address &address)
{
    address.setCountry(value);
}

class ContactAccessorPair
{
public:
    ContactAccessorPair(valueGetter get, valueSetter set, const QString &name)
        : getter(get), diffName(name)
    {
        setter.vSetter = set;
    }

    ContactAccessorPair(valueGetter get, addressSetter set, const QString &name)
        : getter(get), diffName(name)
    {
        setter.aSetter = set;
    }

public:
    valueGetter getter;
    union tsetter {
        valueSetter vSetter;
        addressSetter aSetter;
    } setter;
    const QString diffName;
};

ContactsHandler::ContactsHandler(SugarSession *session)
    : ModuleHandler(QLatin1String("Contacts"), session),
      mAccessors(new ContactAccessorHash)
{
    mAccessors->insert(QLatin1String("first_name"),
                       new ContactAccessorPair(getFirstName, setFirstName,
                                        i18nc("@item:intable", "First Name")));
    mAccessors->insert(QLatin1String("last_name"),
                       new ContactAccessorPair(getLastName, setLastName,
                                        i18nc("@item:intable", "Last Name")));
    mAccessors->insert(QLatin1String("email1"),
                       new ContactAccessorPair(getEmail1, setEmail1,
                                        i18nc("@item:intable", "Primary Email")));
    mAccessors->insert(QLatin1String("email2"),
                       new ContactAccessorPair(getEmail2, setEmail2,
                                        i18nc("@item:intable", "Other Email")));
    mAccessors->insert(QLatin1String("title"),
                       new ContactAccessorPair(getTitle, setTitle,
                                        i18nc("@item:intable job title", "Title")));
    mAccessors->insert(QLatin1String("department"),
                       new ContactAccessorPair(getDepartment, setDepartment,
                                        i18nc("@item:intable", "Department")));
    mAccessors->insert(QLatin1String("account_name"),
                       new ContactAccessorPair(getAccountName, setAccountName,
                                        i18nc("@item:intable", "Account Name")));
    mAccessors->insert(QLatin1String("account_id"),
                       new ContactAccessorPair(getAccountId, setAccountId, QString()));
    mAccessors->insert(QLatin1String("phone_home"),
                       new ContactAccessorPair(getHomePhone, setHomePhone,
                                        i18nc("@item:intable", "Phone (Home)")));
    mAccessors->insert(QLatin1String("phone_work"),
                       new ContactAccessorPair(getWorkPhone, setWorkPhone,
                                        i18nc("@item:intable", "Phone (Office)")));
    mAccessors->insert(QLatin1String("phone_mobile"),
                       new ContactAccessorPair(getMobilePhone, setMobilePhone,
                                        i18nc("@item:intable", "Phone (Mobile)")));
    mAccessors->insert(QLatin1String("phone_other"),
                       new ContactAccessorPair(getOtherPhone, setOtherPhone,
                                        i18nc("@item:intable", "Phone (Other)")));
    mAccessors->insert(QLatin1String("phone_fax"),
                       new ContactAccessorPair(getFaxPhone, setFaxPhone,
                                        i18nc("@item:intable", "Fax")));
    mAccessors->insert(QLatin1String("primary_address_street"),
                       new ContactAccessorPair(getPrimaryStreet, setPrimaryStreet, QString()));
    mAccessors->insert(QLatin1String("primary_address_city"),
                       new ContactAccessorPair(getPrimaryCity, setPrimaryCity, QString()));
    mAccessors->insert(QLatin1String("primary_address_state"),
                       new ContactAccessorPair(getPrimaryState, setPrimaryState, QString()));
    mAccessors->insert(QLatin1String("primary_address_postalcode"),
                       new ContactAccessorPair(getPrimaryPostalcode, setPrimaryPostalcode, QString()));
    mAccessors->insert(QLatin1String("primary_address_country"),
                       new ContactAccessorPair(getPrimaryCountry, setPrimaryCountry, QString()));
    mAccessors->insert(QLatin1String("alt_address_street"),
                       new ContactAccessorPair(getOtherStreet, setOtherStreet, QString()));
    mAccessors->insert(QLatin1String("alt_address_city"),
                       new ContactAccessorPair(getOtherCity, setOtherCity, QString()));
    mAccessors->insert(QLatin1String("alt_address_state"),
                       new ContactAccessorPair(getOtherState, setOtherState, QString()));
    mAccessors->insert(QLatin1String("alt_address_postalcode"),
                       new ContactAccessorPair(getOtherPostalcode, setOtherPostalcode, QString()));
    mAccessors->insert(QLatin1String("alt_address_country"),
                       new ContactAccessorPair(getOtherCountry, setOtherCountry, QString()));
    mAccessors->insert(QLatin1String("birthdate"),
                       new ContactAccessorPair(getBirthday, setBirthday,
                                        i18nc("@item:intable", "Birthdate")));
    mAccessors->insert(QLatin1String("description"),
                       new ContactAccessorPair(getNote, setNote,
                                        i18nc("@item:intable", "Description")));
    mAccessors->insert(QLatin1String("assistant"),
                       new ContactAccessorPair(getAssistantName, setAssistantName,
                                        i18nc("@item:intable", "Assistant")));
    mAccessors->insert(QLatin1String("assistant_phone"),
                       new ContactAccessorPair(getAssistantPhone, setAssistantPhone,
                                        i18nc("@item:intable", "Assistant Phone")));
    mAccessors->insert(QLatin1String("lead_source"),
                       new ContactAccessorPair(getLeadSourceName, setLeadSourceName,
                                        i18nc("@item:intable", "Lead Source")));
    mAccessors->insert(QLatin1String("campaign_name"),
                       new ContactAccessorPair(getCampaignName, setCampaignName,
                                        i18nc("@item:intable", "Campaign")));
    mAccessors->insert(QLatin1String("campaign_id"),
                       new ContactAccessorPair(getCampaignId, setCampaignId, QString()));
    mAccessors->insert(QLatin1String("assigned_user_name"),
                       new ContactAccessorPair(getAssignedUserName, setAssignedUserName, QString()));
    mAccessors->insert(QLatin1String("assigned_user_id"),
                       new ContactAccessorPair(getAssignedUserId, setAssignedUserId, QString()));
    mAccessors->insert(QLatin1String("report_to_name"),
                       new ContactAccessorPair(getReportsToUserName, setReportsToUserName,
                                        i18nc("@item:intable", "Reports To")));
    mAccessors->insert(QLatin1String("reports_to_id"),
                       new ContactAccessorPair(getReportsToUserId, setReportsToUserId, QString()));
    mAccessors->insert(QLatin1String("modified_by_name"),
                       new ContactAccessorPair(getModifiedByName, setModifiedByName, QString()));
    mAccessors->insert(QLatin1String("date_modified"),
                       new ContactAccessorPair(getDateModified, setDateModified, QString()));
    mAccessors->insert(QLatin1String("modified_user_id"),
                       new ContactAccessorPair(getModifiedUserId, setModifiedUserId, QString()));
    mAccessors->insert(QLatin1String("date_entered"),
                       new ContactAccessorPair(getDateCreated, setDateCreated, QString()));
    mAccessors->insert(QLatin1String("id"),
                       new ContactAccessorPair(0, setContactId, QString()));
    mAccessors->insert(QLatin1String("created_by_name"),
                       new ContactAccessorPair(getCreatedByName, setCreatedByName, QString()));
    mAccessors->insert(QLatin1String("created_by"),
                       new ContactAccessorPair(getCreatedById, setCreatedById, QString()));
    mAccessors->insert(QLatin1String("salutation"),
                       new ContactAccessorPair(getSalutation, setSalutation,
                                        i18nc("@item:intable", "Salutation")));
    mAccessors->insert(QLatin1String("opportunity_role_fields"),
                       new ContactAccessorPair(getOpportunityRoleFields, setOpportunityRoleFields, QString()));
    mAccessors->insert(QLatin1String("c_accept_status_fields"),
                       new ContactAccessorPair(getCAcceptStatusFields, setCAcceptStatusFields, QString()));
    mAccessors->insert(QLatin1String("m_accept_status_fields"),
                       new ContactAccessorPair(getMAcceptStatusFields, setMAcceptStatusFields, QString()));
    mAccessors->insert(QLatin1String("deleted"),
                       new ContactAccessorPair(getDeleted, setDeleted, QString()));
    mAccessors->insert(QLatin1String("do_not_call"),
                       new ContactAccessorPair(getDoNotCall, setDoNotCall, QString()));
}

ContactsHandler::~ContactsHandler()
{
    qDeleteAll(*mAccessors);
    delete mAccessors;
}

Akonadi::Collection ContactsHandler::handlerCollection() const
{
    Akonadi::Collection contactCollection;
    contactCollection.setRemoteId(moduleName());
    contactCollection.setContentMimeTypes(QStringList() << KContacts::Addressee::mimeType());
    contactCollection.setName(i18nc("@item folder name", "Contacts"));
    contactCollection.setRights(Akonadi::Collection::CanChangeItem |
                                Akonadi::Collection::CanCreateItem |
                                Akonadi::Collection::CanDeleteItem);

    return contactCollection;
}

bool ContactsHandler::setEntry(const Akonadi::Item &item)
{
    if (!item.hasPayload<KContacts::Addressee>()) {
        qCritical() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Addressee payload";
        return false;
    }

    QList<KDSoapGenerated::TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the contact being added
    if (!item.remoteId().isEmpty()) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(QLatin1String("id"));
        field.setValue(item.remoteId());

        itemList << field;
    }

    const KContacts::Addressee addressee = item.payload<KContacts::Addressee>();
    ContactAccessorHash::const_iterator it    = mAccessors->constBegin();
    ContactAccessorHash::const_iterator endIt = mAccessors->constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if ((*it)->getter == 0) {
            continue;
        }
        KDSoapGenerated::TNS__Name_value field;
        field.setName(it.key());
        const QString value = KDCRMUtils::encodeXML((*it)->getter(addressee));
        field.setValue(value);

        itemList << field;
    }

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);
    soap()->asyncSet_entry(sessionId(), moduleName(), valueList);

    return true;
}

QString ContactsHandler::orderByForListing() const
{
    return QLatin1String("contacts.last_name");
}

QStringList ContactsHandler::supportedSugarFields() const
{
    return mAccessors->keys();
}

QStringList ContactsHandler::supportedCRMFields() const
{
    QStringList ret;
    // This comes from ContactDetails...

    ret.append(KDCRMFields::accountName());
    ret.append(KDCRMFields::accountId());

    ret.append(KDCRMFields::salutation());
    ret.append(KDCRMFields::firstName());
    ret.append(KDCRMFields::lastName());
    ret.append(KDCRMFields::title());
    ret.append(KDCRMFields::department());
    ret.append(KDCRMFields::accountName());
    ret.append(KDCRMFields::email1());
    ret.append(KDCRMFields::email2());
    ret.append(KDCRMFields::phoneHome());
    ret.append(KDCRMFields::phoneMobile());
    ret.append(KDCRMFields::phoneWork());
    ret.append(KDCRMFields::phoneOther());
    ret.append(KDCRMFields::phoneFax());
    ret.append(KDCRMFields::primaryAddressStreet());
    ret.append(KDCRMFields::primaryAddressCity());
    ret.append(KDCRMFields::primaryAddressState());
    ret.append(KDCRMFields::primaryAddressPostalcode());
    ret.append(KDCRMFields::primaryAddressCountry());
    ret.append(KDCRMFields::altAddressStreet());
    ret.append(KDCRMFields::altAddressCity());
    ret.append(KDCRMFields::altAddressState());
    ret.append(KDCRMFields::altAddressPostalcode());
    ret.append(KDCRMFields::altAddressCountry());
    ret.append(KDCRMFields::birthdate());
    ret.append(KDCRMFields::assistant());
    ret.append(KDCRMFields::phoneAssistant());
    ret.append(KDCRMFields::leadSource());
    ret.append(KDCRMFields::campaign());
    ret.append(KDCRMFields::assignedUserId());
    ret.append(KDCRMFields::assignedUserName());
    ret.append(KDCRMFields::reportsTo());
    ret.append(KDCRMFields::reportsToId());
    ret.append(KDCRMFields::doNotCall());
    ret.append(KDCRMFields::description());
    ret.append(KDCRMFields::modifiedByName());
    ret.append(KDCRMFields::dateModified());
    ret.append(KDCRMFields::dateEntered());
    ret.append(KDCRMFields::createdByName());
    ret.append(KDCRMFields::modifiedUserId());
    ret.append(KDCRMFields::id());
    ret.append(KDCRMFields::opportunityRoleFields());
    ret.append(KDCRMFields::cAcceptStatusFields());
    ret.append(KDCRMFields::mAcceptStatusFields());
    ret.append(KDCRMFields::deleted());
    ret.append(KDCRMFields::createdBy());

    return ret;
}

int ContactsHandler::expectedContentsVersion() const
{
    // version 1 = decodeXML/encodeXML added to solve special chars
    return 1;
}

Akonadi::Item ContactsHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection)
{
    Akonadi::Item item;

    const QList<KDSoapGenerated::TNS__Name_value> valueList = entry.name_value_list().items();
    if (valueList.isEmpty()) {
        qWarning() << "Contacts entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId(entry.id());
    item.setParentCollection(parentCollection);
    item.setMimeType(KContacts::Addressee::mimeType());

    KContacts::Addressee addressee;
    addressee.setUid(entry.id());
    KContacts::Address workAddress,  homeAddress;
    workAddress.setType(KContacts::Address::Work | KContacts::Address::Pref);
    homeAddress.setType(KContacts::Address::Home);

    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList) {
        const ContactAccessorHash::const_iterator accessIt = mAccessors->constFind(namedValue.name());
        if (accessIt == mAccessors->constEnd()) {
            // no accessor for field
            continue;
        }

        const QString value = KDCRMUtils::decodeXML(namedValue.value());

        if (isAddressValue(namedValue.name())) {
            KContacts::Address &address =
                isPrimaryAddressValue(namedValue.name()) ? workAddress : homeAddress;
            (*accessIt)->setter.aSetter(value, address);
        } else {
            (*accessIt)->setter.vSetter(value, addressee);
        }
    }
    addressee.insertAddress(workAddress);
    addressee.insertAddress(homeAddress);
    item.setPayload<KContacts::Addressee>(addressee);
    item.setRemoteRevision(getDateModified(addressee));

    return item;
}

bool ContactsHandler::needBackendChange(const Akonadi::Item &item, const QSet<QByteArray> &modifiedParts) const
{
    if (ModuleHandler::needBackendChange(item, modifiedParts)) {
        return true;
    }

    return modifiedParts.contains(partIdFromPayloadPart(Akonadi::ContactPart::Lookup)) ||
           modifiedParts.contains(partIdFromPayloadPart(Akonadi::ContactPart::Standard));
}

void ContactsHandler::compare(Akonadi::AbstractDifferencesReporter *reporter,
                              const Akonadi::Item &leftItem, const Akonadi::Item &rightItem)
{
    Q_ASSERT(leftItem.hasPayload<KContacts::Addressee>());
    Q_ASSERT(rightItem.hasPayload<KContacts::Addressee>());

    const KContacts::Addressee leftContact = leftItem.payload<KContacts::Addressee>();
    const KContacts::Addressee rightContact = rightItem.payload<KContacts::Addressee>();

    const QString modifiedBy = mSession->userName();
    const QString modifiedOn = formatDate(getDateModified(rightContact));

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "Local Contact"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Serverside Contact: modified by %1 on %2",
              modifiedBy, modifiedOn));

    bool seenPrimaryAddress = false;
    bool seenOtherAddress = false;
    ContactAccessorHash::const_iterator it    = mAccessors->constBegin();
    ContactAccessorHash::const_iterator endIt = mAccessors->constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if ((*it)->getter == 0) {
            continue;
        }

        QString leftValue = (*it)->getter(leftContact);
        QString rightValue = (*it)->getter(rightContact);

        QString diffName = (*it)->diffName;
        if (diffName.isEmpty()) {
            // check for special fields
            if (isAddressValue(it.key())) {
                if (isPrimaryAddressValue(it.key())) {
                    if (!seenPrimaryAddress) {
                        diffName = i18nc("item:intable", "Primary Address");
                        seenPrimaryAddress = true;
                        const KContacts::Address leftAddress =
                            leftContact.address(KContacts::Address::Work | KContacts::Address::Pref);
                        const KContacts::Address rightAddress =
                            rightContact.address(KContacts::Address::Work | KContacts::Address::Pref);

                        leftValue = leftAddress.formattedAddress();
                        rightValue = rightAddress.formattedAddress();
                    } else {
                        // already printed, skip
                        continue;
                    }
                } else {
                    if (!seenOtherAddress) {
                        seenOtherAddress = true;
                        diffName = i18nc("item:intable", "Other Address");
                        const KContacts::Address leftAddress =
                            leftContact.address(KContacts::Address::Home);
                        const KContacts::Address rightAddress =
                            rightContact.address(KContacts::Address::Home);

                        leftValue = leftAddress.formattedAddress();
                        rightValue = rightAddress.formattedAddress();
                    } else {
                        // already printed, skip
                        continue;
                    }
                }
            } else if (it.key() == "do_not_call") {
                diffName = i18nc("@item:intable", "Do Not Call");
                leftValue = getDoNotCall(leftContact) == QLatin1String("1")
                            ? QLatin1String("Yes") : QLatin1String("No");
                rightValue = getDoNotCall(rightContact) == QLatin1String("1")
                             ? QLatin1String("Yes") : QLatin1String("No");
            } else {
                // internal field, skip
                continue;
            }
        }

        if (leftValue.isEmpty() && rightValue.isEmpty()) {
            continue;
        }

        if (leftValue.isEmpty()) {
            reporter->addProperty(Akonadi::AbstractDifferencesReporter::AdditionalRightMode,
                                  diffName, leftValue, rightValue);
        } else if (rightValue.isEmpty()) {
            reporter->addProperty(Akonadi::AbstractDifferencesReporter::AdditionalLeftMode,
                                  diffName, leftValue, rightValue);
        } else if (leftValue != rightValue) {
            reporter->addProperty(Akonadi::AbstractDifferencesReporter::ConflictMode,
                                  diffName, leftValue, rightValue);
        }
    }
}
