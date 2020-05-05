/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "modulehandler.h"
#include "sugarcrmresource_debug.h"

#include "sugarsession.h"
#include "listentriesscope.h"
#include "listentriesjob.h"
using namespace KDSoapGenerated;

#include "kdcrmdata/kdcrmfields.h"
#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/enumdefinitionattribute.h"

#include "currency.h"
#include "sugarprotocolbase.h"

#include <AkonadiCore/AgentManager>
#include <AkonadiCore/AttributeFactory>
#include <AkonadiCore/CollectionFetchJob>
#include <AkonadiCore/CollectionModifyJob>

#include <KLocalizedString>

#include <QVector>

ModuleHandler::ModuleHandler(const Module moduleName, SugarSession *session)
    : mSession(session),
      mModule(moduleName),
      mParsedEnumDefinitions(false),
      mHasEnumDefinitions(false)
{
    Q_ASSERT(mSession != nullptr);

    static bool initDone = false;
    if (!initDone) {
        Akonadi::AttributeFactory::registerAttribute<EnumDefinitionAttribute>();
        initDone = true;
    }
}

ModuleHandler::~ModuleHandler()
{
}

Module ModuleHandler::module() const
{
    return mModule;
}

void ModuleHandler::initialCheck()
{
    Akonadi::CollectionFetchJob *job = new Akonadi::CollectionFetchJob(collection(), Akonadi::CollectionFetchJob::Base, this);
    connect(job, &Akonadi::CollectionFetchJob::collectionsReceived, this, &ModuleHandler::slotCollectionsReceived);
}

QString ModuleHandler::latestTimestamp() const
{
    return mLatestTimestamp;
}

Akonadi::Collection ModuleHandler::collection()
{
    if (!mCollection.isValid()) {
        mCollection = handlerCollection();
        mCollection.setRemoteId(moduleToName(module()));
    }
    return mCollection;
}

void ModuleHandler::modifyCollection(const Akonadi::Collection &collection)
{
    auto *modJob = new Akonadi::CollectionModifyJob(collection, this);
    connect(modJob, &KJob::result, this, &ModuleHandler::slotCollectionModifyResult);
    modJob->exec();
    mCollection = modJob->collection();
}

int ModuleHandler::getEntriesCount(const ListEntriesScope &scope, int &entriesCount, QString &errorMessage)
{
    const QString query = scope.query(queryStringForListing(), mModule);
    return mSession->protocol()->getEntriesCount(scope, module(), query, entriesCount, errorMessage);
}

int ModuleHandler::listEntries(const ListEntriesScope &scope, EntriesListResult &entriesListResult, QString &errorMessage)
{
    const QString query = scope.query(queryStringForListing(), mModule);
    const QString orderBy = orderByForListing();

    return mSession->protocol()->listEntries(scope, module(), query, orderBy, supportedSugarFields(), entriesListResult, errorMessage);
}

QStringList ModuleHandler::availableFields() const
{
    if (mAvailableFields.isEmpty()) {
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Available Fields for " << mModule
                 << "not fetched yet, getting them now";

        mAvailableFields = listAvailableFieldNames(mSession, moduleToName(mModule));
    }

    return mAvailableFields;
}

// static (also used by debug handler for modules without a handler)
QStringList ModuleHandler::listAvailableFieldNames(SugarSession *session, const QString &module)
{
    const KDSoapGenerated::TNS__Field_list fieldList = listAvailableFields(session, module);
    const auto items = fieldList.items();
    QStringList availableFields;
    availableFields.reserve(items.count());
    for (const auto &field : items) {
        availableFields << field.name();
    }
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Fields (SOAP v4.1) :" << availableFields;

    return availableFields;
}

// static (also used by debug handler for modules without a handler)
KDSoapGenerated::TNS__Field_list ModuleHandler::listAvailableFields(SugarSession *session, const QString &module)
{
    KDSoapGenerated::TNS__Field_list fields;
    const QString sessionId = session->sessionId();
    if (sessionId.isEmpty()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "No session! Need to login first.";
        return fields;
    }

    QString errorMessage;
    int error = session->protocol()->getModuleFields(module, fields, errorMessage);
    if (error != KJob::NoError) {
       qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << errorMessage;
    }

    return fields;
}

int ModuleHandler::getEntry(const Akonadi::Item &item, KDSoapGenerated::TNS__Entry_value &entryValue, QString &errorMessage)
{
    if (item.remoteId().isEmpty()) {
        qCCritical(FATCRM_SUGARCRMRESOURCE_LOG) << "Item remoteId is empty. id=" << item.id();
        return SugarJob::InvalidContextError;
    }

    return mSession->protocol()->getEntry(mModule, item.remoteId(), supportedSugarFields(), entryValue, errorMessage);
}


bool ModuleHandler::hasEnumDefinitions() const
{
    return mHasEnumDefinitions;
}

EnumDefinitions ModuleHandler::enumDefinitions() const
{
    return mEnumDefinitions;
}

bool ModuleHandler::parseFieldList(Akonadi::Collection &collection, const TNS__Field_list &fields)
{
    if (!mParsedEnumDefinitions) {
        mParsedEnumDefinitions = true;
        foreach (const KDSoapGenerated::TNS__Field &field, fields.items()) {
            const QString fieldName = field.name();
            //qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << fieldName << "TYPE" << field.type();
            if (field.type() == QLatin1String("enum")) {
                //qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << moduleName() << "enum" << fieldName;
                QString crmFieldName = sugarFieldToCrmField(fieldName);
                if (crmFieldName.isEmpty())
                    crmFieldName = customSugarFieldToCrmField(fieldName);
                EnumDefinitions::Enum definition(crmFieldName);
                foreach (const KDSoapGenerated::TNS__Name_value &nameValue, field.options().items()) {
                    // In general, name==value except for some like
                    // name="QtonAndroidFreeSessions" value="Qt on Android Free Sessions"
                    //qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << nameValue.name() << nameValue.value();
                    definition.mEnumValues.append(EnumDefinitions::KeyValue{nameValue.name(), nameValue.value()});
                }
                mEnumDefinitions.append(definition);
            }
        }

        // Also store currencies in there (for opps)
        if (module() == Opportunities) {
            // Grab currencies (typically there are only 2 or 3)
            // The fields can be checked using
            // qdbus org.freedesktop.Akonadi.Resource.akonadi_sugarcrm_resource_0 /CRMDebug availableFields Currencies
            QString errorMessage;
            EntriesListResult entriesListResult;
            const int result = mSession->protocol()->listEntries({}, Currencies, QString(), QString(), {"id", "symbol", "iso4217"}, entriesListResult, errorMessage);
            if (result == KJob::NoError) {
                EnumDefinitions::Enum definition(KDCRMFields::currencyId());
                // The magic id -99 for EUR is in the SuiteCRM source code...
                definition.mEnumValues.append(EnumDefinitions::KeyValue{QString("-99"), QString::fromUtf8("€")});
                const auto items = entriesListResult.entryList.items();
                for (const KDSoapGenerated::TNS__Entry_value &entry : items) {
                    const Currency cur(entry);
                    definition.mEnumValues.append(EnumDefinitions::KeyValue{cur.id(), cur.symbol()});
                }
                mEnumDefinitions.append(definition);
            }
        }

        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << module() << "found enum definitions:" << mEnumDefinitions.toString();
        // Accounts: account_type, industry
        // Contacts: salutation, lead_source, portal_user_type
        // Opportunities: opportunity_type, lead_source, sales_stage  (+ we add currency_symbol by hand)
        // Emails: type, status
        // Notes: <none>
        auto *attr = collection.attribute<EnumDefinitionAttribute>(Akonadi::Collection::AddIfMissing);
        const QString serialized = mEnumDefinitions.toString();
        if (attr->value() != serialized) {
            attr->setValue(serialized);
            return true;
        }
    }
    return false; // no change
}

Akonadi::Item::List ModuleHandler::itemsFromListEntriesResponse(const KDSoapGenerated::TNS__Entry_list &entryList, const Akonadi::Collection &parentCollection, Akonadi::Item::List &deletedItems, QString *lastTimestamp)
{
    Akonadi::Item::List items;
    items.reserve(entryList.items().size());

    Q_FOREACH (const KDSoapGenerated::TNS__Entry_value &entry, entryList.items()) {
        bool deleted = false;
        const Akonadi::Item item = itemFromEntry(entry, parentCollection, deleted);
        if (!item.remoteId().isEmpty()) {
            if (deleted) {
                deletedItems << item;
            } else {
                items << item;
            }
            if (lastTimestamp->isEmpty() || item.remoteRevision() > *lastTimestamp) {
                *lastTimestamp = item.remoteRevision();
            }
        }
    }

    return items;
}

bool ModuleHandler::needBackendChange(const Akonadi::Item &item, const QSet<QByteArray> &modifiedParts) const
{
    Q_UNUSED(item);

    return modifiedParts.contains(partIdFromPayloadPart(Akonadi::Item::FullPayload));
}

QString ModuleHandler::formatDate(const QString &dateString)
{
    return KDCRMUtils::formatTimestamp(dateString);
}

QByteArray ModuleHandler::partIdFromPayloadPart(const char *part)
{
    return QByteArray("PLD:") + part;
}

QString ModuleHandler::sugarFieldToCrmField(const QString &sugarFieldName)
{
    const QMap<QString, QString> &map = fieldNamesMapping();
    return map.key(sugarFieldName, QString());
}

QString ModuleHandler::customSugarFieldToCrmField(const QString &sugarFieldName)
{
    if (sugarFieldName.endsWith(QLatin1String("_c")))
        return sugarFieldName.left(sugarFieldName.size() - 2);

    return sugarFieldName;
}

QStringList ModuleHandler::sugarFieldsToCrmFields(const QStringList &sugarFieldNames) const
{
    QStringList crmFieldNames;

    foreach (const QString &sugarFieldName, sugarFieldNames) {
        QString crmFieldName = sugarFieldToCrmField(sugarFieldName);
        if (crmFieldName.isEmpty()) {
            crmFieldName = customSugarFieldToCrmField(sugarFieldName);
        }

        if (!crmFieldName.isEmpty())
            crmFieldNames.append(crmFieldName);
    }

    return crmFieldNames;
}

QString ModuleHandler::sugarFieldFromCrmField(const QString &crmFieldName)
{
    const QMap<QString, QString> &map = fieldNamesMapping();
    return map.value(crmFieldName, QString());
}

QString ModuleHandler::customSugarFieldFromCrmField(const QString &crmFieldName)
{
    return crmFieldName + QStringLiteral("_c");
}

QStringList ModuleHandler::sugarFieldsFromCrmFields(const QStringList &crmFieldNames) const
{
    QStringList sugarFieldNames;

    foreach (const QString &crmFieldName, crmFieldNames) {
        QString sugarFieldName = sugarFieldFromCrmField(crmFieldName);
        if (sugarFieldName.isEmpty()) {
            sugarFieldName = customSugarFieldFromCrmField(crmFieldName);
        }

        if (!sugarFieldName.isEmpty())
            sugarFieldNames.append(sugarFieldName);
    }

    return sugarFieldNames;
}

const QMap<QString, QString>& ModuleHandler::fieldNamesMapping()
{
    static QMap<QString, QString> mapping;
    if (mapping.isEmpty()) {
        //                      CRM field name , Sugar field name
        mapping.insert(KDCRMFields::accountDescription(), QStringLiteral("account_description"));
        mapping.insert(KDCRMFields::accountId(), QStringLiteral("account_id"));
        mapping.insert(KDCRMFields::accountName(), QStringLiteral("account_name"));
        mapping.insert(KDCRMFields::accountType(), QStringLiteral("account_type"));
        mapping.insert(KDCRMFields::accountPhoneOther(), QStringLiteral("phone_alternate"));
        mapping.insert(KDCRMFields::accountPhoneWork(), QStringLiteral("phone_office"));
        mapping.insert(KDCRMFields::actualCost(), QStringLiteral("actual_cost"));
        mapping.insert(KDCRMFields::altAddressCity(), QStringLiteral("alt_address_city"));
        mapping.insert(KDCRMFields::altAddressCountry(), QStringLiteral("alt_address_country"));
        mapping.insert(KDCRMFields::altAddressPostalcode(), QStringLiteral("alt_address_postalcode"));
        mapping.insert(KDCRMFields::altAddressState(), QStringLiteral("alt_address_state"));
        mapping.insert(KDCRMFields::altAddressStreet(), QStringLiteral("alt_address_street"));
        mapping.insert(KDCRMFields::amount(), QStringLiteral("amount"));
        mapping.insert(KDCRMFields::amountUsDollar(), QStringLiteral("amount_usdollar"));
        mapping.insert(KDCRMFields::annualRevenue(), QStringLiteral("annual_revenue"));
        mapping.insert(KDCRMFields::assignedUserId(), QStringLiteral("assigned_user_id"));
        mapping.insert(KDCRMFields::assignedUserName(), QStringLiteral("assigned_user_name"));
        mapping.insert(KDCRMFields::assistant(), QStringLiteral("assistant"));
        mapping.insert(KDCRMFields::billingAddressCity(), QStringLiteral("billing_address_city"));
        mapping.insert(KDCRMFields::billingAddressCountry(), QStringLiteral("billing_address_country"));
        mapping.insert(KDCRMFields::billingAddressPostalcode(), QStringLiteral("billing_address_postalcode"));
        mapping.insert(KDCRMFields::billingAddressState(), QStringLiteral("billing_address_state"));
        mapping.insert(KDCRMFields::billingAddressStreet(), QStringLiteral("billing_address_street"));
        mapping.insert(KDCRMFields::birthdate(), QStringLiteral("birthdate"));
        mapping.insert(KDCRMFields::budget(), QStringLiteral("budget"));
        mapping.insert(KDCRMFields::cAcceptStatusFields(), QStringLiteral("c_accept_status_fields"));
        mapping.insert(KDCRMFields::campaign(), QStringLiteral("campaign"));
        mapping.insert(KDCRMFields::campaignId(), QStringLiteral("campaign_id"));
        mapping.insert(KDCRMFields::campaignName(), QStringLiteral("campaign_name"));
        mapping.insert(KDCRMFields::campaignType(), QStringLiteral("campaign_type"));
        mapping.insert(KDCRMFields::ccAddrsNames(), QStringLiteral("cc_addrs_names"));
        mapping.insert(KDCRMFields::contactId(), QStringLiteral("contact_id"));
        mapping.insert(KDCRMFields::contactName(), QStringLiteral("contact_name"));
        mapping.insert(KDCRMFields::content(), QStringLiteral("content"));
        mapping.insert(KDCRMFields::converted(), QStringLiteral("converted"));
        mapping.insert(KDCRMFields::createdBy(), QStringLiteral("created_by"));
        mapping.insert(KDCRMFields::createdByName(), QStringLiteral("created_by_name"));
        mapping.insert(KDCRMFields::currencyId(), QStringLiteral("currency_id"));
        mapping.insert(KDCRMFields::currencyName(), QStringLiteral("currency_name"));
        mapping.insert(KDCRMFields::currencySymbol(), QStringLiteral("currency_symbol"));
        mapping.insert(KDCRMFields::dateClosed(), QStringLiteral("date_closed"));
        mapping.insert(KDCRMFields::dateDue(), QStringLiteral("date_due"));
        mapping.insert(KDCRMFields::dateDueFlag(), QStringLiteral("date_due_flag"));
        mapping.insert(KDCRMFields::dateEntered(), QStringLiteral("date_entered"));
        mapping.insert(KDCRMFields::dateModified(), QStringLiteral("date_modified"));
        mapping.insert(KDCRMFields::dateSent(), QStringLiteral("date_sent"));
        mapping.insert(KDCRMFields::dateStart(), QStringLiteral("date_start"));
        mapping.insert(KDCRMFields::dateStartFlag(), QStringLiteral("date_start_flag"));
        mapping.insert(KDCRMFields::deleted(), QStringLiteral("deleted"));
        mapping.insert(KDCRMFields::department(), QStringLiteral("department"));
        mapping.insert(KDCRMFields::description(), QStringLiteral("description"));
        mapping.insert(KDCRMFields::doNotCall(), QStringLiteral("do_not_call"));
        mapping.insert(KDCRMFields::email1(), QStringLiteral("email1"));
        mapping.insert(KDCRMFields::email2(), QStringLiteral("email2"));
        mapping.insert(KDCRMFields::employees(), QStringLiteral("employees"));
        mapping.insert(KDCRMFields::endDate(), QStringLiteral("end_date"));
        mapping.insert(KDCRMFields::expectedCost(), QStringLiteral("expected_cost"));
        mapping.insert(KDCRMFields::expectedRevenue(), QStringLiteral("expected_revenue"));
        mapping.insert(KDCRMFields::fileMimeType(), QStringLiteral("file_mime_type"));
        mapping.insert(KDCRMFields::fileName(), QStringLiteral("file_name"));
        mapping.insert(KDCRMFields::firstName(), QStringLiteral("first_name"));
        mapping.insert(KDCRMFields::frequency(), QStringLiteral("frequency"));
        mapping.insert(KDCRMFields::fromAddrName(), QStringLiteral("from_addr_name"));
        mapping.insert(KDCRMFields::id(), QStringLiteral("id"));
        mapping.insert(KDCRMFields::impressions(), QStringLiteral("impressions"));
        mapping.insert(KDCRMFields::industry(), QStringLiteral("industry"));
        mapping.insert(KDCRMFields::lastName(), QStringLiteral("last_name"));
        mapping.insert(KDCRMFields::leadSource(), QStringLiteral("lead_source"));
        mapping.insert(KDCRMFields::leadSourceDescription(), QStringLiteral("lead_source_description"));
        mapping.insert(KDCRMFields::mAcceptStatusFields(), QStringLiteral("m_accept_status_fields"));
        mapping.insert(KDCRMFields::messageId(), QStringLiteral("message_id"));
        mapping.insert(KDCRMFields::modifiedByName(), QStringLiteral("modified_by_name"));
        mapping.insert(KDCRMFields::modifiedUserId(), QStringLiteral("modified_user_id"));
        mapping.insert(KDCRMFields::name(), QStringLiteral("name"));
        mapping.insert(KDCRMFields::nextStep(), QStringLiteral("next_step"));
        mapping.insert(KDCRMFields::objective(), QStringLiteral("objective"));
        mapping.insert(KDCRMFields::opportunityAmount(), QStringLiteral("opportunity_amount"));
        mapping.insert(KDCRMFields::opportunityId(), QStringLiteral("opportunity_id"));
        mapping.insert(KDCRMFields::opportunityName(), QStringLiteral("opportunity_name"));
        mapping.insert(KDCRMFields::opportunityRoleFields(), QStringLiteral("opportunity_role_fields"));
        mapping.insert(KDCRMFields::opportunityType(), QStringLiteral("opportunity_type"));
        mapping.insert(KDCRMFields::ownership(), QStringLiteral("ownership"));
        mapping.insert(KDCRMFields::parentId(), QStringLiteral("parent_id"));
        mapping.insert(KDCRMFields::parentName(), QStringLiteral("parent_name"));
        mapping.insert(KDCRMFields::parentType(), QStringLiteral("parent_type"));
        mapping.insert(KDCRMFields::phoneAssistant(), QStringLiteral("assistant_phone"));
        mapping.insert(KDCRMFields::phoneFax(), QStringLiteral("phone_fax"));
        mapping.insert(KDCRMFields::phoneHome(), QStringLiteral("phone_home"));
        mapping.insert(KDCRMFields::phoneMobile(), QStringLiteral("phone_mobile"));
        mapping.insert(KDCRMFields::phoneOther(), QStringLiteral("phone_other"));
        mapping.insert(KDCRMFields::phoneWork(), QStringLiteral("phone_work"));
        mapping.insert(KDCRMFields::portalApp(), QStringLiteral("portal_app"));
        mapping.insert(KDCRMFields::portalName(), QStringLiteral("portal_name"));
        mapping.insert(KDCRMFields::primaryAddressCity(), QStringLiteral("primary_address_city"));
        mapping.insert(KDCRMFields::primaryAddressCountry(), QStringLiteral("primary_address_country"));
        mapping.insert(KDCRMFields::primaryAddressPostalcode(), QStringLiteral("primary_address_postalcode"));
        mapping.insert(KDCRMFields::primaryAddressState(), QStringLiteral("primary_address_state"));
        mapping.insert(KDCRMFields::primaryAddressStreet(), QStringLiteral("primary_address_street"));
        mapping.insert(KDCRMFields::priority(), QStringLiteral("priority"));
        mapping.insert(KDCRMFields::probability(), QStringLiteral("probability"));
        mapping.insert(KDCRMFields::rating(), QStringLiteral("rating"));
        mapping.insert(KDCRMFields::referedBy(), QStringLiteral("refered_by"));
        mapping.insert(KDCRMFields::referUrl(), QStringLiteral("refer_url"));
        mapping.insert(KDCRMFields::reportsTo(), QStringLiteral("report_to_name"));
        mapping.insert(KDCRMFields::reportsToId(), QStringLiteral("reports_to_id"));
        mapping.insert(KDCRMFields::salesStage(), QStringLiteral("sales_stage"));
        mapping.insert(KDCRMFields::salutation(), QStringLiteral("salutation"));
        mapping.insert(KDCRMFields::shippingAddressCity(), QStringLiteral("shipping_address_city"));
        mapping.insert(KDCRMFields::shippingAddressCountry(), QStringLiteral("shipping_address_country"));
        mapping.insert(KDCRMFields::shippingAddressPostalcode(), QStringLiteral("shipping_address_postalcaode"));
        mapping.insert(KDCRMFields::shippingAddressState(), QStringLiteral("shipping_address_state"));
        mapping.insert(KDCRMFields::shippingAddressStreet(), QStringLiteral("shipping_address_street"));
        mapping.insert(KDCRMFields::sicCode(), QStringLiteral("sic_code"));
        mapping.insert(KDCRMFields::startDate(), QStringLiteral("start_date"));
        mapping.insert(KDCRMFields::status(), QStringLiteral("status"));
        mapping.insert(KDCRMFields::statusDescription(), QStringLiteral("status_description"));
        mapping.insert(KDCRMFields::tickerSymbol(), QStringLiteral("ticker_symbol"));
        mapping.insert(KDCRMFields::title(), QStringLiteral("title"));
        mapping.insert(KDCRMFields::trackerCount(), QStringLiteral("tracker_count"));
        mapping.insert(KDCRMFields::trackerKey(), QStringLiteral("tracker_key"));
        mapping.insert(KDCRMFields::trackerText(), QStringLiteral("tracker_text"));
        mapping.insert(KDCRMFields::toAddrsNames(), QStringLiteral("to_addrs_names"));
        mapping.insert(KDCRMFields::website(), QStringLiteral("website"));
        mapping.insert(KDCRMFields::documentName(), QStringLiteral("document_name"));
        mapping.insert(KDCRMFields::docId(), QStringLiteral("doc_id"));
        mapping.insert(KDCRMFields::docType(), QStringLiteral("doc_type"));
        mapping.insert(KDCRMFields::docUrl(), QStringLiteral("doc_url"));
        mapping.insert(KDCRMFields::activeDate(), QStringLiteral("active_date"));
        mapping.insert(KDCRMFields::expDate(), QStringLiteral("exp_date"));
        mapping.insert(KDCRMFields::categoryId(), QStringLiteral("category_id"));
        mapping.insert(KDCRMFields::subcategoryId(), QStringLiteral("subcategory_id"));
        mapping.insert(KDCRMFields::statusId(), QStringLiteral("status_id"));
        mapping.insert(KDCRMFields::documentRevisionId(), QStringLiteral("document_revision_id"));
        mapping.insert(KDCRMFields::relatedDocId(), QStringLiteral("related_doc_id"));
        mapping.insert(KDCRMFields::relatedDocName(), QStringLiteral("related_doc_name"));
        mapping.insert(KDCRMFields::relatedDocRevId(), QStringLiteral("related_doc_rev_id"));
        mapping.insert(KDCRMFields::isTemplate(), QStringLiteral("is_template"));
        mapping.insert(KDCRMFields::templateType(), QStringLiteral("template_type"));
    }

    return mapping;
}

QString ModuleHandler::sessionId() const
{
    return mSession->sessionId();
}

void ModuleHandler::slotCollectionModifyResult(KJob *job)
{
    if (job->error()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << job->errorString();
    }
}

void ModuleHandler::slotCollectionsReceived(const Akonadi::Collection::List &collections)
{
    if (collections.count() != 1) {
        return;
    }
    Akonadi::Collection collection = collections.at(0);
    if (ListEntriesJob::currentContentsVersion(collection) != expectedContentsVersion()) {
        // the contents need to be relisted, do this right now before users get a chance to view bad data
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Forcing a reload of" << collection.name() << "in module" << mModule << "because"
                 << ListEntriesJob::currentContentsVersion(collection) << "!="
                 << expectedContentsVersion();
        Akonadi::AgentManager::self()->synchronizeCollection(collection);
    }

    auto *attr = collection.attribute<EnumDefinitionAttribute>();
    mHasEnumDefinitions = attr;
}
