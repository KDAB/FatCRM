/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/enumdefinitionattribute.h"

#include <AkonadiCore/AgentManager>
#include <AkonadiCore/AttributeFactory>
#include <AkonadiCore/CollectionFetchJob>
#include <AkonadiCore/CollectionModifyJob>

#include <KLocalizedString>

#include <QVector>

ModuleHandler::ModuleHandler(const QString &moduleName, SugarSession *session)
    : mSession(session),
      mModuleName(moduleName),
      mParsedEnumDefinitions(false),
      mHasEnumDefinitions(false)
{
    Q_ASSERT(mSession != 0);

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
    Akonadi::CollectionModifyJob *modJob = new Akonadi::CollectionModifyJob(collection, this);
    connect(modJob, SIGNAL(result(KJob*)), this, SLOT(slotCollectionModifyResult(KJob*)));
}

void ModuleHandler::getEntriesCount(const ListEntriesScope &scope)
{
    const QString query = scope.query(queryStringForListing(), mModuleName.toLower());
    soap()->asyncGet_entries_count(sessionId(), moduleName(), query, scope.deleted());
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

QStringList ModuleHandler::availableFields() const
{
    if (mAvailableFields.isEmpty()) {
        qDebug() << "Available Fields for " << mModuleName
                 << "not fetched yet, getting them now";

        mAvailableFields = listAvailableFields(mSession, mModuleName);
    }

    return mAvailableFields;
}

// static (also used by debug handler for modules without a handler)
QStringList ModuleHandler::listAvailableFields(SugarSession *session, const QString &module)
{
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
        qDebug() << "Got" << availableFields << "fields";
    } else {
        qDebug() << "Got error: number=" << error.number()
                 << "name=" << error.name()
                 << "description=" << error.description();
    }
    return availableFields;
}

bool ModuleHandler::getEntry(const Akonadi::Item &item)
{
    if (item.remoteId().isEmpty()) {
        qCritical() << "Item remoteId is empty. id=" << item.id();
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

void ModuleHandler::parseFieldList(const TNS__Field_list &fields)
{
    if (!mParsedEnumDefinitions) {
        mParsedEnumDefinitions = true;
        foreach (const KDSoapGenerated::TNS__Field &field, fields.items()) {
            const QString fieldName = field.name();
            //qDebug() << fieldName << "TYPE" << field.type();
            if (field.type() == QLatin1String("enum")) {
                //qDebug() << moduleName() << "enum" << fieldName;
                EnumDefinitions::Enum definition(fieldName);
                foreach (const KDSoapGenerated::TNS__Name_value &nameValue, field.options().items()) {
                    // In general, name==value except for some like
                    // name="QtonAndroidFreeSessions" value="Qt on Android Free Sessions"
                    //qDebug() << nameValue.name() << nameValue.value();
                    definition.mEnumValues.insert(nameValue.name(), nameValue.value());
                }
                mEnumDefinitions.append(definition);
            }
        }
        qDebug() << moduleName() << "found enum definitions:" << mEnumDefinitions.toString();
        // Accounts: account_type, industry
        // Contacts: salutation, lead_source, portal_user_type
        // Opportunities: opportunity_type, lead_source, sales_stage
        // Emails: type, status
        // Notes: <none>
        Akonadi::Collection coll = collection();
#ifdef AKONADI_OLD_API
        EnumDefinitionAttribute *attr = coll.attribute<EnumDefinitionAttribute>(Akonadi::Entity::AddIfMissing);
#else
        EnumDefinitionAttribute *attr = coll.attribute<EnumDefinitionAttribute>(Akonadi::Collection::AddIfMissing);
#endif
        const QString serialized = mEnumDefinitions.toString();
        if (attr->value() != serialized) {
            attr->setValue(serialized);
            modifyCollection(coll);
        }
    }
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
        qWarning() << job->errorString();
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
        qDebug() << "Forcing a reload of" << collection.name() << "in module" << mModuleName << "because"
                 << ListEntriesJob::currentContentsVersion(collection) << "!="
                 << expectedContentsVersion();
        Akonadi::AgentManager::self()->synchronizeCollection(collection);
    }

    EnumDefinitionAttribute *attr = collection.attribute<EnumDefinitionAttribute>();
    mHasEnumDefinitions = attr;
}
