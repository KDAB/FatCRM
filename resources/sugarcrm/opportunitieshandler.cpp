/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>
           Jeremy Entressangle <jeremy.entressangle@kdab.com>

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

#include "opportunitieshandler.h"

#include "sugarsession.h"
#include "wsdl_sugar41.h"
#include "kdcrmutils.h"
#include "kdcrmfields.h"
#include "sugarjob.h"

using namespace KDSoapGenerated;

#include <Akonadi/AbstractDifferencesReporter>
#include <Akonadi/Collection>

#include <KLocalizedString>
#include "sugarcrmresource_debug.h"

OpportunitiesHandler::OpportunitiesHandler(SugarSession *session)
    : ModuleHandler(Module::Opportunities, session)
{
}

OpportunitiesHandler::~OpportunitiesHandler()
{
}

Akonadi::Collection OpportunitiesHandler::handlerCollection() const
{
    Akonadi::Collection myCollection;
    myCollection.setContentMimeTypes(QStringList() << SugarOpportunity::mimeType());
    myCollection.setName(i18nc("@item folder name", "Opportunities"));
    myCollection.setRights(Akonadi::Collection::CanChangeItem |
                                Akonadi::Collection::CanCreateItem |
                                Akonadi::Collection::CanDeleteItem);

    return myCollection;
}

KDSoapGenerated::TNS__Name_value_list OpportunitiesHandler::sugarOpportunityToNameValueList(const SugarOpportunity &opp, QList<KDSoapGenerated::TNS__Name_value> itemList)
{
    const SugarOpportunity::AccessorHash accessors = SugarOpportunity::accessorHash();
    for (auto it = accessors.constBegin(); it != accessors.constEnd(); ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }
        const SugarOpportunity::valueGetter getter = (*it).getter;
        KDSoapGenerated::TNS__Name_value field;
        field.setName(sugarFieldFromCrmField(it.key()));
        field.setValue(KDCRMUtils::encodeXML((opp.*getter)()));

        itemList << field;
    }

    // plus custom fields
    const QMap<QString, QString> customFields = opp.customFields();
    for (auto cit = customFields.constBegin(); cit != customFields.constEnd(); ++cit ) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(customSugarFieldFromCrmField(cit.key()));
        field.setValue(KDCRMUtils::encodeXML(cit.value()));
        itemList << field;
    }

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);

    return valueList;
}

int OpportunitiesHandler::setEntry(const Akonadi::Item &item, QString &newId, QString &errorMessage)
{
    if (!item.hasPayload<SugarOpportunity>()) {
        qCCritical(FATCRM_SUGARCRMRESOURCE_LOG) << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Opportunity payload";
        return SugarJob::InvalidContextError;
    }

    QList<KDSoapGenerated::TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the account being added
    if (!item.remoteId().isEmpty()) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(QStringLiteral("id"));
        field.setValue(item.remoteId());

        itemList << field;
    }

    const SugarOpportunity opp = item.payload<SugarOpportunity>();

    KDSoapGenerated::TNS__Name_value_list valueList = sugarOpportunityToNameValueList(opp, itemList);

    return mSession->protocol()->setEntry(module(), valueList, newId, errorMessage);
}

int OpportunitiesHandler::saveExtraInformation(const Akonadi::Item &item, const QString &id, QString &errorMessage)
{
    const SugarOpportunity opp = item.payload<SugarOpportunity>();

    // set_relationship can either add or remove relationships, but not just replace
    // So we need to fetch existing, compare, delete, and then add (!)
    const QStringList oldLinkedContacts = fetchLinkedContacts(opp);
    const QString oldPrimaryContactId = oldLinkedContacts.isEmpty() ? QString() : oldLinkedContacts.at(0);
    const QString newPrimaryContactId = opp.primaryContactId();
    if (oldPrimaryContactId != newPrimaryContactId) {
        // API docs: https://support.sugarcrm.com/Documentation/Sugar_Developer/Sugar_Developer_Guide_10.0/Integration/Web_Services/Legacy_API/Methods/set_relationship
        if (!oldLinkedContacts.isEmpty()) {
            //qDebug() << "removing old linked contacts" << oldLinkedContacts;
            mSession->protocol()->setRelationship(id, Opportunities, oldLinkedContacts, Contacts, true /*delete*/, errorMessage);
        }
        if (!newPrimaryContactId.isEmpty()) {
            //qDebug() << "saving primary contact" << newPrimaryContactId << "for opp" << id;
            return mSession->protocol()->setRelationship(id, Opportunities, {newPrimaryContactId}, Contacts, false, errorMessage);
        }
    }
    // Nothing to do
    return KJob::NoError;
}

int OpportunitiesHandler::expectedContentsVersion() const
{
    // version 1 = account_name is resolved to account_id upon loading
    // version 2 = changed custom fields storage names
    // version 3 = fix list of supported crm fields
    // version 4 = new custom field opportunity_size_c
    return 4;
}

QString OpportunitiesHandler::orderByForListing() const
{
    return QStringLiteral("opportunities.name");
}

QStringList OpportunitiesHandler::supportedSugarFields() const
{
    return availableFields();
}

QStringList OpportunitiesHandler::supportedCRMFields() const
{
    return sugarFieldsToCrmFields(availableFields()) << KDCRMFields::primaryContactId() /* stored as extra information (relationship) */;
}

SugarOpportunity OpportunitiesHandler::nameValueListToSugarOpportunity(const KDSoapGenerated::TNS__Name_value_list & valueList, const QString &id)
{
    SugarOpportunity opportunity;
    opportunity.setId(id);
    const SugarOpportunity::AccessorHash accessors = SugarOpportunity::accessorHash();
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList.items()) {
        const QString crmFieldName = sugarFieldToCrmField(namedValue.name());
        const QString value = KDCRMUtils::decodeXML(namedValue.value());
        const SugarOpportunity::AccessorHash::const_iterator accessIt = accessors.constFind(crmFieldName);
        if (accessIt == accessors.constEnd()) {
            const QString customCrmFieldName = customSugarFieldToCrmField(namedValue.name());
            opportunity.setCustomField(customCrmFieldName, value);
            continue;
        }

        (opportunity.*(accessIt.value().setter))(value);
    }
    return opportunity;
}

bool OpportunitiesHandler::needsExtraInformation() const
{
    return true;
}

QStringList OpportunitiesHandler::fetchLinkedContacts(const SugarOpportunity &opp)
{
    auto result = mSession->protocol()->getRelationships(opp.id(), module(), Module::Contacts);
    return result.ids;
}

void OpportunitiesHandler::getExtraInformation(Akonadi::Item::List &items)
{
    for (int pos = 0; pos < items.count(); ++pos) {
        Akonadi::Item &item = items[pos];

        if (!item.hasPayload<SugarOpportunity>()) {
            qCCritical(FATCRM_SUGARCRMRESOURCE_LOG) << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                                                    << ", mime=" << item.mimeType() << ") is missing Opportunity payload";
            continue;
        }

        SugarOpportunity opp = item.payload<SugarOpportunity>();
        bool update = false;
        const QStringList linkedContacts = fetchLinkedContacts(opp);
        if (!linkedContacts.isEmpty()) {
            // The GUI is only interested in the first linked contact
            // We store that as the primary contact (so that it fits with the getData/setData mechanism in FatCRM)
            const QString primaryContactId = linkedContacts.at(0);
            opp.setPrimaryContactId(primaryContactId);
            update = true;
        }

        if (update) {
            item.setPayload<SugarOpportunity>(opp);
        }
    }
}

Akonadi::Item OpportunitiesHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection, bool &deleted)
{
    Akonadi::Item item;

    const QList<KDSoapGenerated::TNS__Name_value> valueList = entry.name_value_list().items();
    if (valueList.isEmpty()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "Opportunities entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId(entry.id());
    item.setParentCollection(parentCollection);
    item.setMimeType(SugarOpportunity::mimeType());

    const SugarOpportunity opportunity = nameValueListToSugarOpportunity(entry.name_value_list(), entry.id());
    item.setPayload<SugarOpportunity>(opportunity);
    item.setRemoteRevision(opportunity.dateModifiedRaw());

    deleted = opportunity.deleted() == QLatin1String("1");

    return item;
}

void OpportunitiesHandler::compare(Akonadi::AbstractDifferencesReporter *reporter,
                                   const Akonadi::Item &leftItem, const Akonadi::Item &rightItem)
{
    Q_ASSERT(leftItem.hasPayload<SugarOpportunity>());
    Q_ASSERT(rightItem.hasPayload<SugarOpportunity>());

    const SugarOpportunity leftOpportunity = leftItem.payload<SugarOpportunity>();
    const SugarOpportunity rightOpportunity = rightItem.payload<SugarOpportunity>();

    const QString modifiedBy = mSession->userName();
    const QString modifiedOn = KDCRMUtils::formatDateTime(rightOpportunity.dateModified());

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "My version of the Opportunity"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Their version of the Opportunity: modified by %1 on %2",
              modifiedBy, modifiedOn));

    const SugarOpportunity::AccessorHash accessors = SugarOpportunity::accessorHash();
    for (auto it = accessors.constBegin(); it != accessors.constEnd(); ++it) {
        // check if this is a read-only field
        if (it.key() == QLatin1String("id")) {
            continue;
        }

        const SugarOpportunity::valueGetter getter = (*it).getter;
        QString leftValue = (leftOpportunity.*getter)();
        QString rightValue = (rightOpportunity.*getter)();

        QString diffName = (*it).diffName;
        if (diffName.isEmpty()) {
            if (it.key() == KDCRMFields::dateClosed()) {
                diffName = i18nc("@item:intable", "Expected Close Date");
                leftValue = formatDate(leftOpportunity.dateClosed());
                rightValue = formatDate(rightOpportunity.dateClosed());
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
