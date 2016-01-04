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

#include "resourcedebuginterface.h"
#include "sugarsession.h"
#include "sugarsoap.h"
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
    return ModuleHandler::listAvailableFields(mResource->mSession, module);
}

int ResourceDebugInterface::getCount(const QString &module) const
{
    SugarSession *session = mResource->mSession;
    KDSoapGenerated::Sugarsoap *soap = session->soap();
    const QString sessionId = session->sessionId();
    if (sessionId.isEmpty()) {
        qWarning() << "No session! Need to login first.";
    }

    // for notes and emails, use this:
    //const QString query = QString("parent_type=\"Opportunities\"");
    const QString query = QString();
    KDSoapGenerated::TNS__Get_entries_count_result response = soap->get_entries_count(sessionId, module, query, 0);
    qDebug() << response.result_count() << "entries";

    // Let's also take a peek at the first entry
    KDSoapGenerated::TNS__Select_fields fields;
    fields.setItems(availableFields(module));
    KDSoapGenerated::TNS__Get_entry_list_result listResponse = soap->get_entry_list(sessionId, module, query, QString() /*orderBy*/, 0 /*offset*/, fields, 1 /*maxResults*/, 0 /*fetchDeleted*/);
    const QList<KDSoapGenerated::TNS__Entry_value> items = listResponse.entry_list().items();
    if (!items.isEmpty()) {
        QList<KDSoapGenerated::TNS__Name_value> values = items.at(0).name_value_list().items();
        Q_FOREACH (const KDSoapGenerated::TNS__Name_value &value, values) {
            qDebug() << value.name() << "=" << value.value();
        }
    } else {
        qDebug() << "No items found. lastError=" << soap->lastError();
    }

    return response.result_count();
}

