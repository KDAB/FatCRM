/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Jeremy Entressangle <jeremy.entressangle@kdab.com>

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

#include "modulename.h"

#include <QDebug>
#include <QString>
#include <QMap>

struct ModuleEntry {
    const char* name;
    const Module module;
};

static const ModuleEntry s_moduleNames[] = {
    {"Accounts", Accounts},
    {"Opportunities", Opportunities},
    {"Campaigns", Campaigns},
    {"Leads", Leads},
    {"Contacts", Contacts},
    {"Documents", Documents},
    {"Emails", Emails},
    {"Notes", Notes},
    {"Tasks", Tasks}
};

static const int count = sizeof(s_moduleNames) / sizeof(*s_moduleNames);

QString moduleToName(Module moduleName)
{
    auto result = std::find_if(s_moduleNames, s_moduleNames + count, [moduleName](const ModuleEntry &a){return a.module == moduleName;});
    if (result == s_moduleNames + count) {
        return QString();
    } else {
        return result->name;
    }
}

Module nameToModule(const QString &name)
{
    auto result = std::find_if(s_moduleNames, s_moduleNames + count, [name](const ModuleEntry &a){return a.name == name;});
    if (result == s_moduleNames + count) {
        return Error;
    } else {
        return result->module;
    }
}

QDebug operator<<(QDebug dbg, Module module)
{
    return dbg << moduleToName(module);
}
