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

#include "resourcedebuginterface.h"
#include "sugarcrmresource_debug.h"
#include "sugarsession.h"
#include "wsdl_sugar41.h"
#include "modulehandler.h"
#include "sugarcrmresource.h"
#include "settings.h"
#include "moduledebuginterface.h"

ResourceDebugInterface::ResourceDebugInterface(SugarCRMResource *resource)
    : QObject(resource), mResource(resource)
{
}

ResourceDebugInterface::~ResourceDebugInterface()
{
}

void ResourceDebugInterface::setKDSoapDebugEnabled(bool enable)
{
    qputenv("KDSOAP_DEBUG", (enable ? "1" : "0"));
}

bool ResourceDebugInterface::kdSoapDebugEnabled() const
{
    const QByteArray value = qgetenv("KDSOAP_DEBUG");
    return value.toInt() != 0;
}

QStringList ResourceDebugInterface::availableModules() const
{
    return Settings::availableModules();
}

QStringList ResourceDebugInterface::supportedModules() const
{
    return mResource->mModuleHandlers->keys();
}

QStringList ResourceDebugInterface::availableFields(const QString &module) const
{
    return ModuleHandler::listAvailableFieldNames(mResource->mSession, module);
}


static void dumpNameValueList(const QList<KDSoapGenerated::TNS__Name_value> &values)
{
    for (const KDSoapGenerated::TNS__Name_value &value : values) {
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << value.name() << "=" << value.value();
    }
}

int ResourceDebugInterface::getCount(const QString &module) const
{
    SugarSession *session = mResource->mSession;
    KDSoapGenerated::Sugarsoap *soap = session->soap();
    const QString sessionId = session->sessionId();
    if (sessionId.isEmpty()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "No session! Need to login first.";
    }

    // for notes and emails, use this:
    //const QString query = QString("parent_type=\"Opportunities\"");
    //const QString query = QString("( emails.parent_type='Opportunities' or emails.parent_type='Accounts' or emails.parent_type='Contacts' ) AND emails.date_modified >= '2015-10-21 09:52:58'");
    const QString query = QString();
    KDSoapGenerated::TNS__Get_entries_count_result response = soap->get_entries_count(sessionId, module, query, 0);
    const int totalCount = response.result_count();
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << totalCount << "entries";

    // Let's also take a peek at the first entry
    KDSoapGenerated::TNS__Select_fields fields;
    fields.setItems(availableFields(module));
    const auto listResponse = soap->get_entry_list(sessionId, module, query, QString() /*orderBy*/, 0 /*offset*/, fields, {}, 1 /*maxResults*/, 1 /*fetchDeleted*/, false /*favorites*/);
    const QList<KDSoapGenerated::TNS__Entry_value> items = listResponse.entry_list().items();
    if (!items.isEmpty()) {
        const QList<KDSoapGenerated::TNS__Name_value> values = items.at(0).name_value_list().items();
        dumpNameValueList(values);
    } else {
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "No items found. lastError=" << soap->lastError();
    }


#if 0
    // List all IDs
    KDSoapGenerated::TNS__Select_fields justIdFields;
    justIdFields.setItems({"id"});
    for (int offset = 0 ; offset < totalCount ; offset += 100) {
        qDebug() << "asking for" << offset << "-" << (offset+100);
        const auto listAllResponse = soap->get_entry_list(sessionId, module, query, QString() /*orderBy*/, offset, justIdFields, {}, 100 /*maxResults*/, 0 /*fetchDeleted*/, false /*favorites*/);
        const QList<KDSoapGenerated::TNS__Entry_value> allItems = listAllResponse.entry_list().items();
        if (!allItems.isEmpty()) {
            for (const KDSoapGenerated::TNS__Entry_value& item : allItems) {
                const QList<KDSoapGenerated::TNS__Name_value> values = item.name_value_list().items();
                for (const KDSoapGenerated::TNS__Name_value &value : values) {
                    if (value.name() == "id") {
                        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << value.name() << "=" << value.value();
                    }
                }
            }
        } else {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Error listing all IDs. lastError=" << soap->lastError();
        }
        break; // OK, I only needed the first 100
    }
#endif

    return totalCount;
}

void ResourceDebugInterface::getEntry(const QString &module, const QString &id) const
{
    SugarSession *session = mResource->mSession;
    KDSoapGenerated::Sugarsoap *soap = session->soap();
    const QString sessionId = session->sessionId();
    if (sessionId.isEmpty()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "No session! Need to login first.";
    }

    const QString query = QString("%1.id=\"%2\"").arg(module.toLower(), id);
    KDSoapGenerated::TNS__Select_fields fields;
    fields.setItems(availableFields(module));
    const auto listResponse = soap->get_entry_list(sessionId, module, query, QString() /*orderBy*/, 0 /*offset*/, fields, {}, 1 /*maxResults*/, 1 /*fetchDeleted*/, false /*favorites*/);
    const QList<KDSoapGenerated::TNS__Entry_value> items = listResponse.entry_list().items();
    if (!items.isEmpty()) {
        const QList<KDSoapGenerated::TNS__Name_value> values = items.at(0).name_value_list().items();
        dumpNameValueList(values);
    } else {
        qWarning() << "No items found. lastError=" << soap->lastError();
    }
}

