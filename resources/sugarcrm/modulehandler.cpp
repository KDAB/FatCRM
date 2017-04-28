/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "modulehandler.h"

#include "sugarsession.h"
#include "sugarsoap.h"
#include "listentriesscope.h"
#include "listentriesjob.h"
using namespace KDSoapGenerated;

#include "kdcrmdata/kdcrmfields.h"
#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/enumdefinitionattribute.h"

#include "sugarprotocolbase.h"

#include <Akonadi/AgentManager>
#include <Akonadi/AttributeFactory>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionModifyJob>

#include <KLocale>

#include <QVector>

ModuleHandler::ModuleHandler(const QString &moduleName, SugarSession *session)
    : mSession(session),
      mModuleName(moduleName),
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

QString ModuleHandler::moduleName() const
{
    return mModuleName;
}

void ModuleHandler::initialCheck()
{
    Akonadi::CollectionFetchJob *job = new Akonadi::CollectionFetchJob(collection(), Akonadi::CollectionFetchJob::Base, this);
    connect(job, SIGNAL(collectionsReceived(Akonadi::Collection::List)), this, SLOT(slotCollectionsReceived(Akonadi::Collection::List)));
}

QString ModuleHandler::latestTimestamp() const
{
    return mLatestTimestamp;
}

Akonadi::Collection ModuleHandler::collection()
{
    if (!mCollection.isValid()) {
        mCollection = handlerCollection();
        mCollection.setRemoteId(moduleName());
    }
    return mCollection;
}

void ModuleHandler::modifyCollection(const Akonadi::Collection &collection)
{
    mCollection = collection;
    Akonadi::CollectionModifyJob *modJob = new Akonadi::CollectionModifyJob(collection, this);
    connect(modJob, SIGNAL(result(KJob*)), this, SLOT(slotCollectionModifyResult(KJob*)));
}

int ModuleHandler::getEntriesCount(const ListEntriesScope &scope, int &entriesCount, QString &errorMessage)
{
    const QString query = scope.query(queryStringForListing(), mModuleName.toLower());
    return mSession->protocol()->getEntriesCount(scope, moduleName(), query, entriesCount, errorMessage);
}

void ModuleHandler::listEntries(const ListEntriesScope &scope)
{
    const QString query = scope.query(queryStringForListing(), mModuleName.toLower());
    const QString orderBy = orderByForListing();
    const int offset = scope.offset();
    const int maxResults = 100;
    const int fetchDeleted = scope.deleted();

    KDSoapGenerated::TNS__Select_fields selectedFields;
    selectedFields.setItems(supportedSugarFields());

    soap()->asyncGet_entry_list(sessionId(), moduleName(), query, orderBy, offset, selectedFields, maxResults, fetchDeleted);
}

int ModuleHandler::listEntries(const ListEntriesScope &scope, EntriesListResult &entriesListResult ,QString &errorMessage)
{
    const QString query = scope.query(queryStringForListing(), mModuleName.toLower());
    const QString orderBy = orderByForListing();

    return mSession->protocol()->listEntries(scope, moduleName(), query, orderBy, supportedSugarFields(), entriesListResult, errorMessage);
}

QStringList ModuleHandler::availableFields() const
{
    if (mAvailableFields.isEmpty()) {
        kDebug() << "Available Fields for " << mModuleName
                 << "not fetched yet, getting them now";

        mAvailableFields = listAvailableFields(mSession, mModuleName);
    }

    return mAvailableFields;
}

// static (also used by debug handler for modules without a handler)
QStringList ModuleHandler::listAvailableFields(SugarSession *session, const QString &module)
{
    if (session->soap() == nullptr) {
        return QStringList();
    }

    KDSoapGenerated::Sugarsoap *soap = session->soap();
    const QString sessionId = session->sessionId();
    if (sessionId.isEmpty()) {
        qWarning() << "No session! Need to login first.";
    }

    const KDSoapGenerated::TNS__Module_fields response = soap->get_module_fields(sessionId, module);

    QStringList availableFields;
    const KDSoapGenerated::TNS__Error_value error = response.error();
    if (error.number().isEmpty() || error.number() == QLatin1String("0")) {
        const KDSoapGenerated::TNS__Field_list fieldList = response.module_fields();
        Q_FOREACH (const KDSoapGenerated::TNS__Field &field, fieldList.items()) {
            availableFields << field.name();
        }
        kDebug() << "Got" << availableFields << "fields";
    } else {
        kDebug() << "Got error: number=" << error.number()
                 << "name=" << error.name()
                 << "description=" << error.description();
    }
    return availableFields;
}

bool ModuleHandler::getEntry(const Akonadi::Item &item)
{
    if (item.remoteId().isEmpty()) {
        kError() << "Item remoteId is empty. id=" << item.id();
        return false;
    }

    KDSoapGenerated::TNS__Select_fields selectedFields;
    selectedFields.setItems(supportedSugarFields());

    soap()->asyncGet_entry(sessionId(), mModuleName, item.remoteId(), selectedFields);
    return true;
}

bool ModuleHandler::hasEnumDefinitions()
{
    return mHasEnumDefinitions;
}

bool ModuleHandler::parseFieldList(Akonadi::Collection &collection, const TNS__Field_list &fields)
{
    if (!mParsedEnumDefinitions) {
        mParsedEnumDefinitions = true;
        foreach (const KDSoapGenerated::TNS__Field &field, fields.items()) {
            const QString fieldName = field.name();
            //kDebug() << fieldName << "TYPE" << field.type();
            if (field.type() == QLatin1String("enum")) {
                //kDebug() << moduleName() << "enum" << fieldName;
                QString crmFieldName = sugarFieldToCrmField(fieldName);
                if (crmFieldName.isEmpty())
                    crmFieldName = customSugarFieldToCrmField(fieldName);
                EnumDefinitions::Enum definition(crmFieldName);
                foreach (const KDSoapGenerated::TNS__Name_value &nameValue, field.options().items()) {
                    // In general, name==value except for some like
                    // name="QtonAndroidFreeSessions" value="Qt on Android Free Sessions"
                    //kDebug() << nameValue.name() << nameValue.value();
                    EnumDefinitions::KeyValue keyValue;
                    keyValue.key = nameValue.name();
                    keyValue.value = nameValue.value();
                    definition.mEnumValues.append(keyValue);
                }
                mEnumDefinitions.append(definition);
            }
        }
        kDebug() << moduleName() << "found enum definitions:" << mEnumDefinitions.toString();
        // Accounts: account_type, industry
        // Contacts: salutation, lead_source, portal_user_type
        // Opportunities: opportunity_type, lead_source, sales_stage
        // Emails: type, status
        // Notes: <none>
        EnumDefinitionAttribute *attr = collection.attribute<EnumDefinitionAttribute>(Akonadi::Collection::AddIfMissing);
        const QString serialized = mEnumDefinitions.toString();
        if (attr->value() != serialized) {
            attr->setValue(serialized);
            return true;
        }
    }
    return false; // no change
}

Akonadi::Item::List ModuleHandler::itemsFromListEntriesResponse(const KDSoapGenerated::TNS__Entry_list &entryList, const Akonadi::Collection &parentCollection, QString *lastTimestamp)
{
    Akonadi::Item::List items;

    Q_FOREACH (const KDSoapGenerated::TNS__Entry_value &entry, entryList.items()) {
        const Akonadi::Item item = itemFromEntry(entry, parentCollection);
        if (!item.remoteId().isEmpty()) {
            items << item;
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

QString ModuleHandler::sugarFieldToCrmField(const QString &sugarFieldName) const
{
    const QMap<QString, QString> &map = fieldNamesMapping();
    return map.key(sugarFieldName, QString());
}

QString ModuleHandler::customSugarFieldToCrmField(const QString &sugarFieldName) const
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

QString ModuleHandler::sugarFieldFromCrmField(const QString &crmFieldName) const
{
    const QMap<QString, QString> &map = fieldNamesMapping();
    return map.value(crmFieldName, QString());
}

QString ModuleHandler::customSugarFieldFromCrmField(const QString &crmFieldName) const
{
    return crmFieldName + QLatin1String("_c");
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

const QMap<QString, QString>& ModuleHandler::fieldNamesMapping() const
{
    static QMap<QString, QString> mapping;
    if (mapping.isEmpty()) {
        //                      CRM field name , Sugar field name
        mapping.insert(KDCRMFields::accountDescription(), QLatin1String("account_description"));
        mapping.insert(KDCRMFields::accountId(), QLatin1String("account_id"));
        mapping.insert(KDCRMFields::accountName(), QLatin1String("account_name"));
        mapping.insert(KDCRMFields::accountType(), QLatin1String("account_type"));
        mapping.insert(KDCRMFields::accountPhoneOther(), QLatin1String("phone_alternate"));
        mapping.insert(KDCRMFields::accountPhoneWork(), QLatin1String("phone_office"));
        mapping.insert(KDCRMFields::actualCost(), QLatin1String("actual_cost"));
        mapping.insert(KDCRMFields::altAddressCity(), QLatin1String("alt_address_city"));
        mapping.insert(KDCRMFields::altAddressCountry(), QLatin1String("alt_address_country"));
        mapping.insert(KDCRMFields::altAddressPostalcode(), QLatin1String("alt_address_postalcode"));
        mapping.insert(KDCRMFields::altAddressState(), QLatin1String("alt_address_state"));
        mapping.insert(KDCRMFields::altAddressStreet(), QLatin1String("alt_address_street"));
        mapping.insert(KDCRMFields::amount(), QLatin1String("amount"));
        mapping.insert(KDCRMFields::amountUsDollar(), QLatin1String("amount_usdollar"));
        mapping.insert(KDCRMFields::annualRevenue(), QLatin1String("annual_revenue"));
        mapping.insert(KDCRMFields::assignedUserId(), QLatin1String("assigned_user_id"));
        mapping.insert(KDCRMFields::assignedUserName(), QLatin1String("assigned_user_name"));
        mapping.insert(KDCRMFields::assistant(), QLatin1String("assistant"));
        mapping.insert(KDCRMFields::billingAddressCity(), QLatin1String("billing_address_city"));
        mapping.insert(KDCRMFields::billingAddressCountry(), QLatin1String("billing_address_country"));
        mapping.insert(KDCRMFields::billingAddressPostalcode(), QLatin1String("billing_address_postalcode"));
        mapping.insert(KDCRMFields::billingAddressState(), QLatin1String("billing_address_state"));
        mapping.insert(KDCRMFields::billingAddressStreet(), QLatin1String("billing_address_street"));
        mapping.insert(KDCRMFields::birthdate(), QLatin1String("birthdate"));
        mapping.insert(KDCRMFields::budget(), QLatin1String("budget"));
        mapping.insert(KDCRMFields::cAcceptStatusFields(), QLatin1String("c_accept_status_fields"));
        mapping.insert(KDCRMFields::campaign(), QLatin1String("campaign"));
        mapping.insert(KDCRMFields::campaignId(), QLatin1String("campaign_id"));
        mapping.insert(KDCRMFields::campaignName(), QLatin1String("campaign_name"));
        mapping.insert(KDCRMFields::campaignType(), QLatin1String("campaign_type"));
        mapping.insert(KDCRMFields::ccAddrsNames(), QLatin1String("cc_addrs_names"));
        mapping.insert(KDCRMFields::contactId(), QLatin1String("contact_id"));
        mapping.insert(KDCRMFields::contactName(), QLatin1String("contact_name"));
        mapping.insert(KDCRMFields::content(), QLatin1String("content"));
        mapping.insert(KDCRMFields::converted(), QLatin1String("converted"));
        mapping.insert(KDCRMFields::createdBy(), QLatin1String("created_by"));
        mapping.insert(KDCRMFields::createdByName(), QLatin1String("created_by_name"));
        mapping.insert(KDCRMFields::currencyId(), QLatin1String("currency_id"));
        mapping.insert(KDCRMFields::currencyName(), QLatin1String("currency_name"));
        mapping.insert(KDCRMFields::currencySymbol(), QLatin1String("currency_symbol"));
        mapping.insert(KDCRMFields::dateClosed(), QLatin1String("date_closed"));
        mapping.insert(KDCRMFields::dateDue(), QLatin1String("date_due"));
        mapping.insert(KDCRMFields::dateDueFlag(), QLatin1String("date_due_flag"));
        mapping.insert(KDCRMFields::dateEntered(), QLatin1String("date_entered"));
        mapping.insert(KDCRMFields::dateModified(), QLatin1String("date_modified"));
        mapping.insert(KDCRMFields::dateSent(), QLatin1String("date_sent"));
        mapping.insert(KDCRMFields::dateStart(), QLatin1String("date_start"));
        mapping.insert(KDCRMFields::dateStartFlag(), QLatin1String("date_start_flag"));
        mapping.insert(KDCRMFields::deleted(), QLatin1String("deleted"));
        mapping.insert(KDCRMFields::department(), QLatin1String("department"));
        mapping.insert(KDCRMFields::description(), QLatin1String("description"));
        mapping.insert(KDCRMFields::doNotCall(), QLatin1String("do_not_call"));
        mapping.insert(KDCRMFields::email1(), QLatin1String("email1"));
        mapping.insert(KDCRMFields::email2(), QLatin1String("email2"));
        mapping.insert(KDCRMFields::employees(), QLatin1String("employees"));
        mapping.insert(KDCRMFields::endDate(), QLatin1String("end_date"));
        mapping.insert(KDCRMFields::expectedCost(), QLatin1String("expected_cost"));
        mapping.insert(KDCRMFields::expectedRevenue(), QLatin1String("expected_revenue"));
        mapping.insert(KDCRMFields::fileMimeType(), QLatin1String("file_mime_type"));
        mapping.insert(KDCRMFields::fileName(), QLatin1String("file_name"));
        mapping.insert(KDCRMFields::firstName(), QLatin1String("first_name"));
        mapping.insert(KDCRMFields::frequency(), QLatin1String("frequency"));
        mapping.insert(KDCRMFields::fromAddrName(), QLatin1String("from_addr_name"));
        mapping.insert(KDCRMFields::id(), QLatin1String("id"));
        mapping.insert(KDCRMFields::impressions(), QLatin1String("impressions"));
        mapping.insert(KDCRMFields::industry(), QLatin1String("industry"));
        mapping.insert(KDCRMFields::lastName(), QLatin1String("last_name"));
        mapping.insert(KDCRMFields::leadSource(), QLatin1String("lead_source"));
        mapping.insert(KDCRMFields::leadSourceDescription(), QLatin1String("lead_source_description"));
        mapping.insert(KDCRMFields::mAcceptStatusFields(), QLatin1String("m_accept_status_fields"));
        mapping.insert(KDCRMFields::messageId(), QLatin1String("message_id"));
        mapping.insert(KDCRMFields::modifiedByName(), QLatin1String("modified_by_name"));
        mapping.insert(KDCRMFields::modifiedUserId(), QLatin1String("modified_user_id"));
        mapping.insert(KDCRMFields::name(), QLatin1String("name"));
        mapping.insert(KDCRMFields::nextStep(), QLatin1String("next_step"));
        mapping.insert(KDCRMFields::objective(), QLatin1String("objective"));
        mapping.insert(KDCRMFields::opportunityAmount(), QLatin1String("opportunity_amount"));
        mapping.insert(KDCRMFields::opportunityId(), QLatin1String("opportunity_id"));
        mapping.insert(KDCRMFields::opportunityName(), QLatin1String("opportunity_name"));
        mapping.insert(KDCRMFields::opportunityRoleFields(), QLatin1String("opportunity_role_fields"));
        mapping.insert(KDCRMFields::opportunityType(), QLatin1String("opportunity_type"));
        mapping.insert(KDCRMFields::ownership(), QLatin1String("ownership"));
        mapping.insert(KDCRMFields::parentId(), QLatin1String("parent_id"));
        mapping.insert(KDCRMFields::parentName(), QLatin1String("parent_name"));
        mapping.insert(KDCRMFields::parentType(), QLatin1String("parent_type"));
        mapping.insert(KDCRMFields::phoneAssistant(), QLatin1String("assistant_phone"));
        mapping.insert(KDCRMFields::phoneFax(), QLatin1String("phone_fax"));
        mapping.insert(KDCRMFields::phoneHome(), QLatin1String("phone_home"));
        mapping.insert(KDCRMFields::phoneMobile(), QLatin1String("phone_mobile"));
        mapping.insert(KDCRMFields::phoneOther(), QLatin1String("phone_other"));
        mapping.insert(KDCRMFields::phoneWork(), QLatin1String("phone_work"));
        mapping.insert(KDCRMFields::portalApp(), QLatin1String("portal_app"));
        mapping.insert(KDCRMFields::portalName(), QLatin1String("portal_name"));
        mapping.insert(KDCRMFields::primaryAddressCity(), QLatin1String("primary_address_city"));
        mapping.insert(KDCRMFields::primaryAddressCountry(), QLatin1String("primary_address_country"));
        mapping.insert(KDCRMFields::primaryAddressPostalcode(), QLatin1String("primary_address_postalcode"));
        mapping.insert(KDCRMFields::primaryAddressState(), QLatin1String("primary_address_state"));
        mapping.insert(KDCRMFields::primaryAddressStreet(), QLatin1String("primary_address_street"));
        mapping.insert(KDCRMFields::priority(), QLatin1String("priority"));
        mapping.insert(KDCRMFields::probability(), QLatin1String("probability"));
        mapping.insert(KDCRMFields::rating(), QLatin1String("rating"));
        mapping.insert(KDCRMFields::referedBy(), QLatin1String("refered_by"));
        mapping.insert(KDCRMFields::referUrl(), QLatin1String("refer_url"));
        mapping.insert(KDCRMFields::reportsTo(), QLatin1String("report_to_name"));
        mapping.insert(KDCRMFields::reportsToId(), QLatin1String("reports_to_id"));
        mapping.insert(KDCRMFields::salesStage(), QLatin1String("sales_stage"));
        mapping.insert(KDCRMFields::salutation(), QLatin1String("salutation"));
        mapping.insert(KDCRMFields::shippingAddressCity(), QLatin1String("shipping_address_city"));
        mapping.insert(KDCRMFields::shippingAddressCountry(), QLatin1String("shipping_address_country"));
        mapping.insert(KDCRMFields::shippingAddressPostalcode(), QLatin1String("shipping_address_postalcaode"));
        mapping.insert(KDCRMFields::shippingAddressState(), QLatin1String("shipping_address_state"));
        mapping.insert(KDCRMFields::shippingAddressStreet(), QLatin1String("shipping_address_street"));
        mapping.insert(KDCRMFields::sicCode(), QLatin1String("sic_code"));
        mapping.insert(KDCRMFields::startDate(), QLatin1String("start_date"));
        mapping.insert(KDCRMFields::status(), QLatin1String("status"));
        mapping.insert(KDCRMFields::statusDescription(), QLatin1String("status_description"));
        mapping.insert(KDCRMFields::tickerSymbol(), QLatin1String("ticker_symbol"));
        mapping.insert(KDCRMFields::title(), QLatin1String("title"));
        mapping.insert(KDCRMFields::trackerCount(), QLatin1String("tracker_count"));
        mapping.insert(KDCRMFields::trackerKey(), QLatin1String("tracker_key"));
        mapping.insert(KDCRMFields::trackerText(), QLatin1String("tracker_text"));
        mapping.insert(KDCRMFields::toAddrsNames(), QLatin1String("to_addrs_names"));
        mapping.insert(KDCRMFields::website(), QLatin1String("website"));
        mapping.insert(KDCRMFields::documentName(), QLatin1String("document_name"));
        mapping.insert(KDCRMFields::docId(), QLatin1String("doc_id"));
        mapping.insert(KDCRMFields::docType(), QLatin1String("doc_type"));
        mapping.insert(KDCRMFields::docUrl(), QLatin1String("doc_url"));
        mapping.insert(KDCRMFields::activeDate(), QLatin1String("active_date"));
        mapping.insert(KDCRMFields::expDate(), QLatin1String("exp_date"));
        mapping.insert(KDCRMFields::categoryId(), QLatin1String("category_id"));
        mapping.insert(KDCRMFields::subcategoryId(), QLatin1String("subcategory_id"));
        mapping.insert(KDCRMFields::statusId(), QLatin1String("status_id"));
        mapping.insert(KDCRMFields::documentRevisionId(), QLatin1String("document_revision_id"));
        mapping.insert(KDCRMFields::relatedDocId(), QLatin1String("related_doc_id"));
        mapping.insert(KDCRMFields::relatedDocName(), QLatin1String("related_doc_name"));
        mapping.insert(KDCRMFields::relatedDocRevId(), QLatin1String("related_doc_rev_id"));
        mapping.insert(KDCRMFields::isTemplate(), QLatin1String("is_template"));
        mapping.insert(KDCRMFields::templateType(), QLatin1String("template_type"));
    }

    return mapping;
}

QString ModuleHandler::sessionId() const
{
    return mSession->sessionId();
}

Sugarsoap *ModuleHandler::soap() const
{
    return mSession->soap();
}

void ModuleHandler::slotCollectionModifyResult(KJob *job)
{
    if (job->error()) {
        kWarning() << job->errorString();
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
        kDebug() << "Forcing a reload of" << collection.name() << "in module" << mModuleName << "because"
                 << ListEntriesJob::currentContentsVersion(collection) << "!="
                 << expectedContentsVersion();
        Akonadi::AgentManager::self()->synchronizeCollection(collection);
    }

    EnumDefinitionAttribute *attr = collection.attribute<EnumDefinitionAttribute>();
    mHasEnumDefinitions = attr;
}
