/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "sugarcrmresource_debug.h"
#include "sugarsession.h"
#include "wsdl_sugar41.h"
#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/kdcrmfields.h"
#include "sugarjob.h"

using namespace KDSoapGenerated;
#include <Akonadi/Contact/ContactParts>

#include <Akonadi/AbstractDifferencesReporter>
#include <Akonadi/Collection>

#include <KContacts/Addressee>
#include <KContacts/Address>

#include <KLocalizedString>

#include <QHash>
#include <sugarcontactwrapper.h>

using valueGetter = QString (*)(const KContacts::Addressee &);
using valueSetter = void (*)(const QString &, KContacts::Addressee &);
using addressSetter = void (*)(const QString &, KContacts::Address &);

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
    return SugarContactWrapper(addressee).accountId();
}

static void setAccountId(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setAccountId(value);
}

static QString getAssistantName(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).assistant();
}

static void setAssistantName(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setAssistant(value);
}

static QString getAssistantPhone(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).phoneAssistant();
}

static void setAssistantPhone(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setPhoneAssistant(value);
}

static QString getLeadSourceName(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).leadSource();
}

static void setLeadSourceName(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setLeadSource(value);
}

static QString getCampaignName(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).campaign();
}

static void setCampaignName(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setCampaign(value);
}

static QString getCampaignId(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).campaignId();
}

static void setCampaignId(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setCampaignId(value);
}

static QString getAssignedUserName(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).assignedUserName();
}

static void setAssignedUserName(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setAssignedUserName(value);
}

static QString getAssignedUserId(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).assignedUserId();
}

static void setAssignedUserId(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setAssignedUserId(value);
}

static QString getReportsToUserName(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).reportsTo();
}

static void setReportsToUserName(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setReportsTo(value);
}

static QString getReportsToUserId(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).reportsToId();
}

static void setReportsToUserId(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setReportsToId(value);
}

static QString getModifiedByName(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).modifiedByName();
}

static void setModifiedByName(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setModifiedByName(value);
}

static QString getDateModified(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).dateModified();
}

static void setDateModified(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setDateModified(value);
}

static QString getModifiedUserId(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).modifiedUserId();
}

static void setModifiedUserId(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setModifiedUserId(value);
}

static QString getDateCreated(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).dateCreated();
}

static void setDateCreated(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setDateCreated(value);
}

static void setContactId(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setId(value);
}

static QString getCreatedByName(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).createdByName();
}

static void setCreatedByName(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setCreatedByName(value);
}

static QString getCreatedById(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).createdBy();
}

static void setCreatedById(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setCreatedBy(value);
}

static QString getSalutation(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).salutation();
}

static void setSalutation(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setSalutation(value);
}

static QString getOpportunityRoleFields(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).opportunityRoleFields();
}

static void setOpportunityRoleFields(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setOpportunityRoleFields(value);
}

static QString getCAcceptStatusFields(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).cAcceptStatusFields();
}

static void setCAcceptStatusFields(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setCAcceptStatusFields(value);
}

static QString getMAcceptStatusFields(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).mAcceptStatusFields();
}

static void setMAcceptStatusFields(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setMAcceptStatusFields(value);
}

static QString getDeleted(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).deleted();
}

static void setDeleted(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setDeleted(value);
}

static QString getDoNotCall(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).doNotCall();
}

static void setDoNotCall(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setDoNotCall(value);
}

static QString getInvalidEmail(const KContacts::Addressee &addressee)
{
    return SugarContactWrapper(addressee).invalidEmail();
}

static void setInvalidEmail(const QString &value, KContacts::Addressee &addressee)
{
    SugarContactWrapper(addressee).setInvalidEmail(value);
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
    addressee.setBirthday(KDCRMUtils::dateFromString(value));
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
    if (!emails.isEmpty()) {
        return emails.at( 0 );
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
    QString diffName;
};

typedef QHash<QString, ContactAccessorPair> AccessorHash;
Q_GLOBAL_STATIC(AccessorHash, s_accessors)
static AccessorHash accessorHash()
{
    AccessorHash &accessors = *s_accessors();
    if (accessors.isEmpty()) {
        accessors.insert(KDCRMFields::firstName(),
                           ContactAccessorPair(getFirstName, setFirstName,
                                            i18nc("@item:intable", "First Name")));
        accessors.insert(KDCRMFields::lastName(),
                           ContactAccessorPair(getLastName, setLastName,
                                            i18nc("@item:intable", "Last Name")));
        accessors.insert(KDCRMFields::email1(),
                           ContactAccessorPair(getEmail1, setEmail1,
                                            i18nc("@item:intable", "Primary Email")));
        accessors.insert(KDCRMFields::email2(),
                            ContactAccessorPair(getEmail2, setEmail2,
                                            i18nc("@item:intable", "Other Email")));
        accessors.insert(KDCRMFields::title(),
                            ContactAccessorPair(getTitle, setTitle,
                                            i18nc("@item:intable job title", "Title")));
        accessors.insert(KDCRMFields::department(),
                            ContactAccessorPair(getDepartment, setDepartment,
                                            i18nc("@item:intable", "Department")));
        accessors.insert(KDCRMFields::accountName(),
                            ContactAccessorPair(getAccountName, setAccountName,
                                            i18nc("@item:intable", "Account Name")));
        accessors.insert(KDCRMFields::accountId(),
                            ContactAccessorPair(getAccountId, setAccountId, QString()));
        accessors.insert(KDCRMFields::phoneHome(),
                            ContactAccessorPair(getHomePhone, setHomePhone,
                                            i18nc("@item:intable", "Phone (Home)")));
        accessors.insert(KDCRMFields::phoneWork(),
                            ContactAccessorPair(getWorkPhone, setWorkPhone,
                                            i18nc("@item:intable", "Phone (Office)")));
        accessors.insert(KDCRMFields::phoneMobile(),
                            ContactAccessorPair(getMobilePhone, setMobilePhone,
                                            i18nc("@item:intable", "Phone (Mobile)")));
        accessors.insert(KDCRMFields::phoneOther(),
                            ContactAccessorPair(getOtherPhone, setOtherPhone,
                                            i18nc("@item:intable", "Phone (Other)")));
        accessors.insert(KDCRMFields::phoneFax(),
                            ContactAccessorPair(getFaxPhone, setFaxPhone,
                                            i18nc("@item:intable", "Fax")));
        accessors.insert(KDCRMFields::primaryAddressStreet(),
                            ContactAccessorPair(getPrimaryStreet, setPrimaryStreet, QString()));
        accessors.insert(KDCRMFields::primaryAddressCity(),
                            ContactAccessorPair(getPrimaryCity, setPrimaryCity, QString()));
        accessors.insert(KDCRMFields::primaryAddressState(),
                            ContactAccessorPair(getPrimaryState, setPrimaryState, QString()));
        accessors.insert(KDCRMFields::primaryAddressPostalcode(),
                            ContactAccessorPair(getPrimaryPostalcode, setPrimaryPostalcode, QString()));
        accessors.insert(KDCRMFields::primaryAddressCountry(),
                            ContactAccessorPair(getPrimaryCountry, setPrimaryCountry, QString()));
        accessors.insert(KDCRMFields::altAddressStreet(),
                            ContactAccessorPair(getOtherStreet, setOtherStreet, QString()));
        accessors.insert(KDCRMFields::altAddressCity(),
                            ContactAccessorPair(getOtherCity, setOtherCity, QString()));
        accessors.insert(KDCRMFields::altAddressState(),
                            ContactAccessorPair(getOtherState, setOtherState, QString()));
        accessors.insert(KDCRMFields::altAddressPostalcode(),
                            ContactAccessorPair(getOtherPostalcode, setOtherPostalcode, QString()));
        accessors.insert(KDCRMFields::altAddressCountry(),
                            ContactAccessorPair(getOtherCountry, setOtherCountry, QString()));
        accessors.insert(KDCRMFields::birthdate(),
                            ContactAccessorPair(getBirthday, setBirthday,
                                            i18nc("@item:intable", "Birthdate")));
        accessors.insert(KDCRMFields::description(),
                            ContactAccessorPair(getNote, setNote,
                                            i18nc("@item:intable", "Description")));
        accessors.insert(KDCRMFields::assistant(),
                            ContactAccessorPair(getAssistantName, setAssistantName,
                                            i18nc("@item:intable", "Assistant")));
        accessors.insert(KDCRMFields::phoneAssistant(),
                            ContactAccessorPair(getAssistantPhone, setAssistantPhone,
                                            i18nc("@item:intable", "Assistant Phone")));
        accessors.insert(KDCRMFields::leadSource(),
                            ContactAccessorPair(getLeadSourceName, setLeadSourceName,
                                            i18nc("@item:intable", "Lead Source")));
        accessors.insert(KDCRMFields::campaignName(),
                            ContactAccessorPair(getCampaignName, setCampaignName,
                                            i18nc("@item:intable", "Campaign")));
        accessors.insert(KDCRMFields::campaignId(),
                            ContactAccessorPair(getCampaignId, setCampaignId, QString()));
        accessors.insert(KDCRMFields::assignedUserName(),
                            ContactAccessorPair(getAssignedUserName, setAssignedUserName, QString()));
        accessors.insert(KDCRMFields::assignedUserId(),
                            ContactAccessorPair(getAssignedUserId, setAssignedUserId, QString()));
        accessors.insert(KDCRMFields::reportsTo(),
                            ContactAccessorPair(getReportsToUserName, setReportsToUserName,
                                            i18nc("@item:intable", "Reports To")));
        accessors.insert(KDCRMFields::reportsToId(),
                            ContactAccessorPair(getReportsToUserId, setReportsToUserId, QString()));
        accessors.insert(KDCRMFields::modifiedByName(),
                            ContactAccessorPair(getModifiedByName, setModifiedByName, QString()));
        accessors.insert(KDCRMFields::dateModified(),
                            ContactAccessorPair(getDateModified, setDateModified, QString()));
        accessors.insert(KDCRMFields::modifiedUserId(),
                            ContactAccessorPair(getModifiedUserId, setModifiedUserId, QString()));
        accessors.insert(KDCRMFields::dateEntered(),
                            ContactAccessorPair(getDateCreated, setDateCreated, QString()));
        accessors.insert(KDCRMFields::id(),
                            ContactAccessorPair(nullptr, setContactId, QString()));
        accessors.insert(KDCRMFields::createdByName(),
                            ContactAccessorPair(getCreatedByName, setCreatedByName, QString()));
        accessors.insert(KDCRMFields::createdBy(),
                            ContactAccessorPair(getCreatedById, setCreatedById, QString()));
        accessors.insert(KDCRMFields::salutation(),
                            ContactAccessorPair(getSalutation, setSalutation,
                                            i18nc("@item:intable", "Salutation")));
        accessors.insert(KDCRMFields::opportunityRoleFields(),
                            ContactAccessorPair(getOpportunityRoleFields, setOpportunityRoleFields, QString()));
        accessors.insert(KDCRMFields::cAcceptStatusFields(),
                            ContactAccessorPair(getCAcceptStatusFields, setCAcceptStatusFields, QString()));
        accessors.insert(KDCRMFields::mAcceptStatusFields(),
                            ContactAccessorPair(getMAcceptStatusFields, setMAcceptStatusFields, QString()));
        accessors.insert(KDCRMFields::deleted(),
                            ContactAccessorPair(getDeleted, setDeleted, QString()));
        accessors.insert(KDCRMFields::doNotCall(),
                            ContactAccessorPair(getDoNotCall, setDoNotCall, QString()));
        accessors.insert(KDCRMFields::invalidEmail(),
                            ContactAccessorPair(getInvalidEmail, setInvalidEmail, QString()));
    }
    return accessors;
}

ContactsHandler::ContactsHandler(SugarSession *session)
    : ModuleHandler(Module::Contacts, session)
{
}

ContactsHandler::~ContactsHandler()
{
}

Akonadi::Collection ContactsHandler::handlerCollection() const
{
    Akonadi::Collection contactCollection;
    contactCollection.setRemoteId(moduleToName(module()));
    contactCollection.setContentMimeTypes(QStringList() << KContacts::Addressee::mimeType());
    contactCollection.setName(i18nc("@item folder name", "Contacts"));
    contactCollection.setRights(Akonadi::Collection::CanChangeItem |
                                Akonadi::Collection::CanCreateItem |
                                Akonadi::Collection::CanDeleteItem);

    return contactCollection;
}


KDSoapGenerated::TNS__Name_value_list ContactsHandler::addresseeToNameValueList(const KContacts::Addressee &addressee, QList<KDSoapGenerated::TNS__Name_value> itemList)
{
    const AccessorHash accessors = accessorHash();
    for (auto it = accessors.constBegin(); it != accessors.constEnd(); ++it) {
        // check if this is a read-only field
        if ((*it).getter == nullptr) {
            continue;
        }
        KDSoapGenerated::TNS__Name_value field;
        field.setName(sugarFieldFromCrmField(it.key()));
        const QString value = KDCRMUtils::encodeXML((*it).getter(addressee));
        field.setValue(value);

        itemList << field;
    }

    // add custom sugar fields
    const QStringList customAddresseeFields = addressee.customs();

    const static QString customFieldPrefix("FATCRM-X-Custom-");

    QStringList customSugarFields;
    std::copy_if(customAddresseeFields.begin(), customAddresseeFields.end(), std::back_inserter(customSugarFields),
                 [](const QString &custom) { return custom.startsWith(customFieldPrefix); });

    for (const QString &custom : qAsConst(customSugarFields)) {
        const int pos = custom.indexOf(':');
        if (pos == -1)
            continue;

        const QString name = custom.mid(customFieldPrefix.size(), pos - customFieldPrefix.size());
        const QString value = custom.mid(pos + 1);

        KDSoapGenerated::TNS__Name_value field;
        field.setName(customSugarFieldFromCrmField(name));
        field.setValue(KDCRMUtils::encodeXML(value));

        itemList << field;
    }

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);

    return  valueList;
}


int ContactsHandler::setEntry(const Akonadi::Item &item, QString &newId, QString &errorMessage)
{
    if (!item.hasPayload<KContacts::Addressee>()) {
        qCCritical(FATCRM_SUGARCRMRESOURCE_LOG) << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Addressee payload";
        return SugarJob::InvalidContextError;
    }

    QList<KDSoapGenerated::TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the contact being added
    if (!item.remoteId().isEmpty()) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(QStringLiteral("id"));
        field.setValue(item.remoteId());

        itemList << field;
    }

    // add regular sugar fields
    const KContacts::Addressee addressee = item.payload<KContacts::Addressee>();

    KDSoapGenerated::TNS__Name_value_list valueList = addresseeToNameValueList(addressee, itemList);

    return mSession->protocol()->setEntry(module(), valueList, newId, errorMessage);
}

QString ContactsHandler::orderByForListing() const
{
    return QStringLiteral("contacts.last_name");
}

QStringList ContactsHandler::supportedSugarFields() const
{
    // get everything, given that KContacts::Addressee has support for custom fields
    return availableFields();
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
    ret.append(KDCRMFields::invalidEmail());
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
    // version 2 = support for custom fields added
    // version 3 = changed custom fields storage names
    // version 4 = added support for invalid_email
    return 4;
}

Akonadi::Item ContactsHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection, bool &deleted)
{
    Akonadi::Item item;

    const QList<KDSoapGenerated::TNS__Name_value> valueList = entry.name_value_list().items();
    if (valueList.isEmpty()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "Contacts entry for id=" << entry.id() << "has no values";
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

    const AccessorHash accessors = accessorHash();
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList) {
        const QString crmFieldName = sugarFieldToCrmField(namedValue.name());

        const AccessorHash::const_iterator accessIt = accessors.constFind(crmFieldName);
        if (accessIt == accessors.constEnd()) { // no accessor for regular field
            const QString customCrmFieldName = customSugarFieldToCrmField(namedValue.name());
            addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-Custom-%1").arg(customCrmFieldName), KDCRMUtils::decodeXML(namedValue.value()));

            continue;
        }

        const QString value = KDCRMUtils::decodeXML(namedValue.value());

        if (isAddressValue(crmFieldName)) {
            KContacts::Address &address =
                isPrimaryAddressValue(crmFieldName) ? workAddress : homeAddress;
            (*accessIt).setter.aSetter(value, address);
        } else {
            (*accessIt).setter.vSetter(value, addressee);
        }
    }
    addressee.insertAddress(workAddress);
    addressee.insertAddress(homeAddress);
    item.setPayload<KContacts::Addressee>(addressee);
    item.setRemoteRevision(getDateModified(addressee));

    deleted = getDeleted(addressee) == QLatin1String("1");

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

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "My Contact"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Their Contact: modified by %1 on %2",
              modifiedBy, modifiedOn));

    bool seenPrimaryAddress = false;
    bool seenOtherAddress = false;
    const AccessorHash accessors = accessorHash();
    for (auto it = accessors.constBegin(); it != accessors.constEnd(); ++it) {
        // check if this is a read-only field
        if ((*it).getter == nullptr) {
            continue;
        }

        QString leftValue = (*it).getter(leftContact);
        QString rightValue = (*it).getter(rightContact);

        QString diffName = (*it).diffName;
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
            } else if (it.key() == QLatin1String("do_not_call")) {
                diffName = i18nc("@item:intable", "Do Not Call");
                leftValue = getDoNotCall(leftContact) == QLatin1String("1")
                            ? QStringLiteral("Yes") : QStringLiteral("No");
                rightValue = getDoNotCall(rightContact) == QLatin1String("1")
                             ? QStringLiteral("Yes") : QStringLiteral("No");
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
