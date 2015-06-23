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

#include "leadshandler.h"
#include "sugarsession.h"
#include "sugarsoap.h"
using namespace KDSoapGenerated;

#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/sugarlead.h"

#include <akonadi/abstractdifferencesreporter.h> //krazy:exclude=camelcase
#include <Akonadi/Collection>

#include <KABC/Address>

#include <KLocale>

#include <QHash>

typedef QString(SugarLead::*valueGetter)() const;
typedef void (SugarLead::*valueSetter)(const QString &);

class LeadAccessorPair
{
public:
    LeadAccessorPair(valueGetter get, valueSetter set, const QString &name)
        : getter(get), setter(set), diffName(name)
    {}

public:
    valueGetter getter;
    valueSetter setter;
    const QString diffName;
};

LeadsHandler::LeadsHandler(SugarSession *session)
    : ModuleHandler(QLatin1String("Leads"), session),
      mAccessors(new LeadsAccessorHash)
{
    mAccessors->insert(QLatin1String("id"),
                       new LeadAccessorPair(&SugarLead::id, &SugarLead::setId, QString()));
    mAccessors->insert(QLatin1String("date_entered"),
                       new LeadAccessorPair(&SugarLead::dateEntered, &SugarLead::setDateEntered, QString()));
    mAccessors->insert(QLatin1String("date_modified"),
                       new LeadAccessorPair(&SugarLead::dateModified, &SugarLead::setDateModified, QString()));
    mAccessors->insert(QLatin1String("modified_user_id"),
                       new LeadAccessorPair(&SugarLead::modifiedUserId, &SugarLead::setModifiedUserId, QString()));
    mAccessors->insert(QLatin1String("modified_by_name"),
                       new LeadAccessorPair(&SugarLead::modifiedByName, &SugarLead::setModifiedByName, QString()));
    mAccessors->insert(QLatin1String("created_by"),
                       new LeadAccessorPair(&SugarLead::createdBy, &SugarLead::setCreatedBy, QString()));
    mAccessors->insert(QLatin1String("created_by_name"),
                       new LeadAccessorPair(&SugarLead::createdByName, &SugarLead::setCreatedByName, QString()));
    mAccessors->insert(QLatin1String("description"),
                       new LeadAccessorPair(&SugarLead::description, &SugarLead::setDescription,
                                        i18nc("@item:intable", "Description")));
    mAccessors->insert(QLatin1String("deleted"),
                       new LeadAccessorPair(&SugarLead::deleted, &SugarLead::setDeleted, QString()));
    mAccessors->insert(QLatin1String("assigned_user_id"),
                       new LeadAccessorPair(&SugarLead::assignedUserId, &SugarLead::setAssignedUserId, QString()));
    mAccessors->insert(QLatin1String("assigned_user_name"),
                       new LeadAccessorPair(&SugarLead::assignedUserName, &SugarLead::setAssignedUserName,
                                        i18nc("@item:intable", "Assigned To")));
    mAccessors->insert(QLatin1String("salutation"),
                       new LeadAccessorPair(&SugarLead::salutation, &SugarLead::setSalutation,
                                        i18nc("@item:intable", "Salutation")));
    mAccessors->insert(QLatin1String("first_name"),
                       new LeadAccessorPair(&SugarLead::firstName, &SugarLead::setFirstName,
                                        i18nc("@item:intable", "First Name")));
    mAccessors->insert(QLatin1String("last_name"),
                       new LeadAccessorPair(&SugarLead::lastName, &SugarLead::setLastName,
                                        i18nc("@item:intable", "Last Name")));
    mAccessors->insert(QLatin1String("title"),
                       new LeadAccessorPair(&SugarLead::title, &SugarLead::setTitle,
                                        i18nc("@item:intable job title", "Title")));
    mAccessors->insert(QLatin1String("department"),
                       new LeadAccessorPair(&SugarLead::department, &SugarLead::setDepartment,
                                        i18nc("@item:intable", "Department")));
    mAccessors->insert(QLatin1String("do_not_call"),
                       new LeadAccessorPair(&SugarLead::doNotCall, &SugarLead::setDoNotCall, QString()));
    mAccessors->insert(QLatin1String("phone_home"),
                       new LeadAccessorPair(&SugarLead::phoneHome, &SugarLead::setPhoneHome,
                                        i18nc("@item:intable", "Phone (Home)")));
    mAccessors->insert(QLatin1String("phone_mobile"),
                       new LeadAccessorPair(&SugarLead::phoneMobile, &SugarLead::setPhoneMobile,
                                        i18nc("@item:intable", "Phone (Mobile)")));
    mAccessors->insert(QLatin1String("phone_work"),
                       new LeadAccessorPair(&SugarLead::phoneWork, &SugarLead::setPhoneWork,
                                        i18nc("@item:intable", "Phone (Office)")));
    mAccessors->insert(QLatin1String("phone_other"),
                       new LeadAccessorPair(&SugarLead::phoneOther, &SugarLead::setPhoneOther,
                                        i18nc("@item:intable", "Phone (Other)")));
    mAccessors->insert(QLatin1String("phone_fax"),
                       new LeadAccessorPair(&SugarLead::phoneFax, &SugarLead::setPhoneFax,
                                        i18nc("@item:intable", "Fax")));
    mAccessors->insert(QLatin1String("email1"),
                       new LeadAccessorPair(&SugarLead::email1, &SugarLead::setEmail1,
                                        i18nc("@item:intable", "Primary Email")));
    mAccessors->insert(QLatin1String("email2"),
                       new LeadAccessorPair(&SugarLead::email2, &SugarLead::setEmail2,
                                        i18nc("@item:intable", "Other Email")));
    mAccessors->insert(QLatin1String("primary_address_street"),
                       new LeadAccessorPair(&SugarLead::primaryAddressStreet, &SugarLead::setPrimaryAddressStreet, QString()));
    mAccessors->insert(QLatin1String("primary_address_city"),
                       new LeadAccessorPair(&SugarLead::primaryAddressCity, &SugarLead::setPrimaryAddressCity, QString()));
    mAccessors->insert(QLatin1String("primary_address_state"),
                       new LeadAccessorPair(&SugarLead::primaryAddressState, &SugarLead::setPrimaryAddressState, QString()));
    mAccessors->insert(QLatin1String("primary_address_postalcode"),
                       new LeadAccessorPair(&SugarLead::primaryAddressPostalcode, &SugarLead::setPrimaryAddressPostalcode, QString()));
    mAccessors->insert(QLatin1String("primary_address_country"),
                       new LeadAccessorPair(&SugarLead::primaryAddressCountry, &SugarLead::setPrimaryAddressCountry, QString()));
    mAccessors->insert(QLatin1String("alt_address_street"),
                       new LeadAccessorPair(&SugarLead::altAddressStreet, &SugarLead::setAltAddressStreet, QString()));
    mAccessors->insert(QLatin1String("alt_address_city"),
                       new LeadAccessorPair(&SugarLead::altAddressCity, &SugarLead::setAltAddressCity, QString()));
    mAccessors->insert(QLatin1String("alt_address_state"),
                       new LeadAccessorPair(&SugarLead::altAddressState, &SugarLead::setAltAddressState, QString()));
    mAccessors->insert(QLatin1String("alt_address_postalcode"),
                       new LeadAccessorPair(&SugarLead::altAddressPostalcode, &SugarLead::setAltAddressPostalcode, QString()));
    mAccessors->insert(QLatin1String("alt_address_country"),
                       new LeadAccessorPair(&SugarLead::altAddressCountry, &SugarLead::setAltAddressCountry, QString()));
    mAccessors->insert(QLatin1String("assistant"),
                       new LeadAccessorPair(&SugarLead::assistant, &SugarLead::setAssistant,
                                        i18nc("@item:intable", "Assistant")));
    mAccessors->insert(QLatin1String("assistant_phone"),
                       new LeadAccessorPair(&SugarLead::assistantPhone, &SugarLead::setAssistantPhone,
                                        i18nc("@item:intable", "Assistant Phone")));
    mAccessors->insert(QLatin1String("converted"),
                       new LeadAccessorPair(&SugarLead::converted, &SugarLead::setConverted, QString()));
    mAccessors->insert(QLatin1String("refered_by"),
                       new LeadAccessorPair(&SugarLead::referedBy, &SugarLead::setReferedBy,
                                        i18nc("@item:intable", "Referred By")));
    mAccessors->insert(QLatin1String("lead_source"),
                       new LeadAccessorPair(&SugarLead::leadSource, &SugarLead::setLeadSource,
                                        i18nc("@item:intable", "Lead Source")));
    mAccessors->insert(QLatin1String("lead_source_description"),
                       new LeadAccessorPair(&SugarLead::leadSourceDescription, &SugarLead::setLeadSourceDescription,
                                        i18nc("@item:intable", "Lead Source Description")));
    mAccessors->insert(QLatin1String("status"),
                       new LeadAccessorPair(&SugarLead::status, &SugarLead::setStatus,
                                        i18nc("@item:intable", "Status")));
    mAccessors->insert(QLatin1String("status_description"),
                       new LeadAccessorPair(&SugarLead::statusDescription, &SugarLead::setStatusDescription,
                                        i18nc("@item:intable", "Status Description")));
    mAccessors->insert(QLatin1String("reports_to_id"),
                       new LeadAccessorPair(&SugarLead::reportsToId, &SugarLead::setReportsToId, QString()));
    mAccessors->insert(QLatin1String("report_to_name"),
                       new LeadAccessorPair(&SugarLead::reportToName, &SugarLead::setReportToName,
                                        i18nc("@item:intable", "Reports To")));
    mAccessors->insert(QLatin1String("account_name"),
                       new LeadAccessorPair(&SugarLead::accountName, &SugarLead::setAccountName,
                                        i18nc("@item:intable", "Account")));
    mAccessors->insert(QLatin1String("account_description"),
                       new LeadAccessorPair(&SugarLead::accountDescription, &SugarLead::setAccountDescription,
                                        i18nc("@item:intable", "Account Description")));
    mAccessors->insert(QLatin1String("contact_id"),
                       new LeadAccessorPair(&SugarLead::contactId, &SugarLead::setContactId, QString()));
    mAccessors->insert(QLatin1String("account_id"),
                       new LeadAccessorPair(&SugarLead::accountId, &SugarLead::setAccountId, QString()));
    mAccessors->insert(QLatin1String("opportunity_id"),
                       new LeadAccessorPair(&SugarLead::opportunityId, &SugarLead::setOpportunityId, QString()));
    mAccessors->insert(QLatin1String("opportunity_name"),
                       new LeadAccessorPair(&SugarLead::opportunityName, &SugarLead::setOpportunityName,
                                        i18nc("@item:intable", "Opportunity")));
    mAccessors->insert(QLatin1String("opportunity_amount"),
                       new LeadAccessorPair(&SugarLead::opportunityAmount, &SugarLead::setOpportunityAmount,
                                        i18nc("@item:intable", "Opportunity Amount")));
    mAccessors->insert(QLatin1String("campaign_id"),
                       new LeadAccessorPair(&SugarLead::campaignId, &SugarLead::setCampaignId, QString()));
    mAccessors->insert(QLatin1String("campaign_name"),
                       new LeadAccessorPair(&SugarLead::campaignName, &SugarLead::setCampaignName,
                                        i18nc("@item:intable", "Campaign")));
    mAccessors->insert(QLatin1String("c_accept_status_fields"),
                       new LeadAccessorPair(&SugarLead::cAcceptStatusFields, &SugarLead::setCAcceptStatusFields, QString()));
    mAccessors->insert(QLatin1String("m_accept_status_fields"),
                       new LeadAccessorPair(&SugarLead::mAcceptStatusFields, &SugarLead::setMAcceptStatusFields, QString()));
    mAccessors->insert(QLatin1String("birthdate"),
                       new LeadAccessorPair(&SugarLead::birthdate, &SugarLead::setBirthdate,
                                        i18nc("@item:intable", "Birthdate")));
    mAccessors->insert(QLatin1String("portal_name"),
                       new LeadAccessorPair(&SugarLead::portalName, &SugarLead::setPortalName,
                                        i18nc("@item:intable", "Portal")));
    mAccessors->insert(QLatin1String("portal_app"),
                       new LeadAccessorPair(&SugarLead::portalApp, &SugarLead::setPortalApp,
                                        i18nc("@item:intable", "Portal Application")));
}

LeadsHandler::~LeadsHandler()
{
    qDeleteAll(*mAccessors);
    delete mAccessors;
}

Akonadi::Collection LeadsHandler::handlerCollection() const
{
    Akonadi::Collection leadCollection;
    leadCollection.setContentMimeTypes(QStringList() << SugarLead::mimeType());
    leadCollection.setName(i18nc("@item folder name", "Leads"));
    leadCollection.setRights(Akonadi::Collection::CanChangeItem |
                             Akonadi::Collection::CanCreateItem |
                             Akonadi::Collection::CanDeleteItem);

    return leadCollection;
}

QString LeadsHandler::orderByForListing() const
{
    return QLatin1String("leads.last_name");
}

QStringList LeadsHandler::supportedSugarFields() const
{
    return mAccessors->keys();
}

bool LeadsHandler::setEntry(const Akonadi::Item &item)
{
    if (!item.hasPayload<SugarLead>()) {
        kError() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Lead payload";
        return false;
    }

    QList<KDSoapGenerated::TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the lead being added
    if (!item.remoteId().isEmpty()) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(QLatin1String("id"));
        field.setValue(item.remoteId());
        itemList << field;
    }

    const SugarLead lead = item.payload<SugarLead>();
    LeadsAccessorHash::const_iterator it    = mAccessors->constBegin();
    LeadsAccessorHash::const_iterator endIt = mAccessors->constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }
        const valueGetter getter = (*it)->getter;
        KDSoapGenerated::TNS__Name_value field;
        field.setName(it.key());
        field.setValue(KDCRMUtils::encodeXML((lead.*getter)()));

        itemList << field;
    }

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);
    soap()->asyncSet_entry(sessionId(), moduleName(), valueList);

    return true;
}

Akonadi::Item LeadsHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection)
{
    Akonadi::Item item;

    const QList<KDSoapGenerated::TNS__Name_value> valueList = entry.name_value_list().items();
    if (valueList.isEmpty()) {
        kWarning() << "Leads entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId(entry.id());
    item.setParentCollection(parentCollection);
    item.setMimeType(SugarLead::mimeType());

    SugarLead lead;
    lead.setId(entry.id());
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList) {
        const LeadsAccessorHash::const_iterator accessIt = mAccessors->constFind(namedValue.name());
        if (accessIt == mAccessors->constEnd()) {
            // no accessor for field
            continue;
        }

        (lead.*(accessIt.value()->setter))(KDCRMUtils::decodeXML(namedValue.value()));
    }
    item.setPayload<SugarLead>(lead);
    item.setRemoteRevision(lead.dateModified());

    return item;
}

void LeadsHandler::compare(Akonadi::AbstractDifferencesReporter *reporter,
                           const Akonadi::Item &leftItem, const Akonadi::Item &rightItem)
{
    Q_ASSERT(leftItem.hasPayload<SugarLead>());
    Q_ASSERT(rightItem.hasPayload<SugarLead>());

    const SugarLead leftLead = leftItem.payload<SugarLead>();
    const SugarLead rightLead = rightItem.payload<SugarLead>();

    const QString modifiedBy = mSession->userName();
    const QString modifiedOn = formatDate(rightLead.dateModified());

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "Local Lead"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Serverside Lead: modified by %1 on %2",
              modifiedBy, modifiedOn));

    bool seenPrimaryAddress = false;
    bool seenOtherAddress = false;
    LeadsAccessorHash::const_iterator it    = mAccessors->constBegin();
    LeadsAccessorHash::const_iterator endIt = mAccessors->constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }

        const valueGetter getter = (*it)->getter;
        QString leftValue = (leftLead.*getter)();
        QString rightValue = (rightLead.*getter)();

        QString diffName = (*it)->diffName;
        if (diffName.isEmpty()) {
            // check for special fields
            if (isAddressValue(it.key())) {
                if (isPrimaryAddressValue(it.key())) {
                    if (!seenPrimaryAddress) {
                        seenPrimaryAddress = true;
                        diffName = i18nc("@item:intable", "Primary Address");

                        KABC::Address leftAddress(KABC::Address::Work | KABC::Address::Pref);
                        leftAddress.setStreet(leftLead.primaryAddressStreet());
                        leftAddress.setLocality(leftLead.primaryAddressCity());
                        leftAddress.setRegion(leftLead.primaryAddressState());
                        leftAddress.setCountry(leftLead.primaryAddressCountry());
                        leftAddress.setPostalCode(leftLead.primaryAddressPostalcode());

                        KABC::Address rightAddress(KABC::Address::Work | KABC::Address::Pref);
                        rightAddress.setStreet(rightLead.primaryAddressStreet());
                        rightAddress.setLocality(rightLead.primaryAddressCity());
                        rightAddress.setRegion(rightLead.primaryAddressState());
                        rightAddress.setCountry(rightLead.primaryAddressCountry());
                        rightAddress.setPostalCode(rightLead.primaryAddressPostalcode());

                        leftValue = leftAddress.formattedAddress();
                        rightValue = rightAddress.formattedAddress();
                    } else {
                        // already printed, skip
                        continue;
                    }
                } else {
                    if (!seenOtherAddress) {
                        seenOtherAddress = true;
                        diffName = i18nc("@item:intable", "Other Address");

                        KABC::Address leftAddress(KABC::Address::Home);
                        leftAddress.setStreet(leftLead.altAddressStreet());
                        leftAddress.setLocality(leftLead.altAddressCity());
                        leftAddress.setRegion(leftLead.altAddressState());
                        leftAddress.setCountry(leftLead.altAddressCountry());
                        leftAddress.setPostalCode(leftLead.altAddressPostalcode());

                        KABC::Address rightAddress(KABC::Address::Home);
                        rightAddress.setStreet(rightLead.altAddressStreet());
                        rightAddress.setLocality(rightLead.altAddressCity());
                        rightAddress.setRegion(rightLead.altAddressState());
                        rightAddress.setCountry(rightLead.altAddressCountry());
                        rightAddress.setPostalCode(rightLead.altAddressPostalcode());

                        leftValue = leftAddress.formattedAddress();
                        rightValue = rightAddress.formattedAddress();
                    } else {
                        // already printed, skip
                        continue;
                    }
                }
            } else if (it.key() == "do_not_call") {
                diffName = i18nc("@item:intable", "Do Not Call");
                leftValue = leftLead.doNotCall() == QLatin1String("1")
                            ? QLatin1String("Yes") : QLatin1String("No");
                rightValue = rightLead.doNotCall() == QLatin1String("1")
                             ? QLatin1String("Yes") : QLatin1String("No");
            } else if (it.key() == "converted") {
                diffName = i18nc("@item:intable", "Converted");
                leftValue = leftLead.converted() == QLatin1String("1")
                            ? QLatin1String("Yes") : QLatin1String("No");
                rightValue = rightLead.converted() == QLatin1String("1")
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
