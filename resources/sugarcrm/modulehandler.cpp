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

#include "modulehandler.h"

#include "sugarsession.h"
#include "sugarsoap.h"
#include "listentriesscope.h"

using namespace KDSoapGenerated;
#include "kdcrmdata/kdcrmutils.h"

#include <KLocale>

#include <QInputDialog>
#include <QStringList>

ModuleHandler::ModuleHandler(const QString &moduleName, SugarSession *session)
    : mSession(session), mModuleName(moduleName)
{
    Q_ASSERT(mSession != 0);
}

ModuleHandler::~ModuleHandler()
{
}

QString ModuleHandler::moduleName() const
{
    return mModuleName;
}

QString ModuleHandler::latestTimestamp() const
{
    return mLatestTimestamp;
}

void ModuleHandler::resetLatestTimestamp()
{
    mLatestTimestamp = QString();
}

Akonadi::Collection ModuleHandler::collection() const
{
    Akonadi::Collection coll = handlerCollection();
    coll.setRemoteId(moduleName());
    return coll;
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
    selectedFields.setItems(supportedFields());

    soap()->asyncGet_entry_list(sessionId(), moduleName(), query, orderBy, offset, selectedFields, maxResults, fetchDeleted);
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
    selectedFields.setItems(supportedFields());

    soap()->asyncGet_entry(sessionId(), mModuleName, item.remoteId(), selectedFields);
    return true;
}

Akonadi::Item::List ModuleHandler::itemsFromListEntriesResponse(const KDSoapGenerated::TNS__Entry_list &entryList, const Akonadi::Collection &parentCollection)
{
    Akonadi::Item::List items;

    Q_FOREACH (const KDSoapGenerated::TNS__Entry_value &entry, entryList.items()) {
        const Akonadi::Item item = itemFromEntry(entry, parentCollection);
        if (!item.remoteId().isEmpty()) {
            items << item;
            if (mLatestTimestamp.isNull() || item.remoteRevision() > mLatestTimestamp) {
                mLatestTimestamp = item.remoteRevision();
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
