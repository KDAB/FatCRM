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

#include "salesforcemodulehandler.h"
#include "salesforcesoap.h"
#include "salesforceresource_debug.h"
using namespace KDSoapGenerated;

SalesforceModuleHandler::SalesforceModuleHandler(const QString &moduleName)
    : mModuleName(moduleName)
{
}

SalesforceModuleHandler::~SalesforceModuleHandler()
{
}

QString SalesforceModuleHandler::moduleName() const
{
    return mModuleName;
}

QStringList SalesforceModuleHandler::availableFields() const
{
    return mAvailableFields;
}

void SalesforceModuleHandler::setDescriptionResult(const TNS__DescribeSObjectResult &description)
{
    mAvailableFields.clear();

    const QList<TNS__Field> fields = description.fields();
    Q_FOREACH (const TNS__Field &field, fields) {
        mAvailableFields << field.name();
    }

    qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "Module" << moduleName() << "has" << mAvailableFields.count() << "fields:"
             << mAvailableFields;
}
