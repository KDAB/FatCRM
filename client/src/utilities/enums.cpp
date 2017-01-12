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

#include "enums.h"
#include <KLocalizedString>

QString typeToString(DetailsType type)
{
    if (type == Account) {
        return QStringLiteral("Accounts");
    } else if (type == Opportunity) {
        return QStringLiteral("Opportunities");
    } else if (type == Lead) {
        return QStringLiteral("Leads");
    } else if (type == Contact) {
        return QStringLiteral("Contacts");
    } else if (type == Campaign) {
        return QStringLiteral("Campaigns");
    } else {
        return QString();
    }
}

QString typeToTranslatedString(DetailsType type)
{
    if (type == Account) {
        return i18n("accounts");
    } else if (type == Opportunity) {
        return i18n("opportunities");
    } else if (type == Lead) {
        return i18n("leads");
    } else if (type == Contact) {
        return i18n("contacts");
    } else if (type == Campaign) {
        return i18n("campaigns");
    } else {
        return QString();
    }
}
