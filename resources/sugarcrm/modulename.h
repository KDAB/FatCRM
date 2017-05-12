/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef MODULENAME
#define MODULENAME

#include <QString>

enum Module {
    Accounts,
    Opportunities,
    Campaigns,
    Leads,
    Contacts,
    Documents,
    Emails,
    Notes,
    Tasks,
    Error,
};

QString moduleToName(Module moduleName);
Module nameToModule(const QString &name);

QDebug operator<<(QDebug dbg, Module module);

#endif

