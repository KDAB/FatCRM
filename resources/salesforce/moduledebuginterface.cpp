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

#include "moduledebuginterface.h"

#include "salesforcemodulehandler.h"
#include "salesforceresource.h"
#include "wsdl_salesforce-partner.h"

#include <QStringList>

ModuleDebugInterface::ModuleDebugInterface(const QString &moduleName, SalesforceResource *resource)
    : QObject(resource), mModuleName(moduleName), mResource(resource)
{
}

ModuleDebugInterface::~ModuleDebugInterface()
{
}

QStringList ModuleDebugInterface::availableFields() const
{
    SalesforceModuleHandler *handler = (*mResource->mModuleHandlers)[ mModuleName ];
    if (handler != nullptr) {
        return handler->availableFields();
    }

    return QStringList();
}

QStringList ModuleDebugInterface::supportedFields() const
{
    SalesforceModuleHandler *handler = (*mResource->mModuleHandlers)[ mModuleName ];
    if (handler != nullptr) {
        return handler->supportedFields();
    }

    return QStringList();
}

