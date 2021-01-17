/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QDebug>

QString typeToString(DetailsType type)
{
    switch (type) {
    case DetailsType::Account:
        return QStringLiteral("Accounts");
    case DetailsType::Opportunity:
        return QStringLiteral("Opportunities");
    case DetailsType::Lead:
        return QStringLiteral("Leads");
    case DetailsType::Contact:
        return QStringLiteral("Contacts");
    case DetailsType::Campaign:
        return QStringLiteral("Campaigns");
    }
    return QString();
}

DetailsType stringToType(const QString &typeStr)
{
    static const QVector<QString> types = {"Accounts", "Opportunities", "Leads", "Contacts", "Campaigns"};

    const int index = types.indexOf(typeStr);
    if (index == -1)
        return DetailsType::Account; // default

    return static_cast<DetailsType>(index);
}

QString typeToTranslatedString(DetailsType type)
{
    switch (type) {
    case DetailsType::Account:
        return i18n("accounts");
    case DetailsType::Opportunity:
        return i18n("opportunities");
    case DetailsType::Lead:
        return i18n("leads");
    case DetailsType::Contact:
        return i18n("contacts");
    case DetailsType::Campaign:
        return i18n("campaigns");
    }
    return QString();
}

static const char* s_maxNextStepDateStrings[] = {
    "NoDate",
    "OneMonthAgo",
    "Today",
    "EndOfThisWeek",
    "EndOfThisMonth",
    "OneMonthFromNow",
    "EndOfThisYear",
    "CustomDate"
};

QString maxNextStepDateToString(MaxNextStepDate date)
{
    return QString::fromLatin1(s_maxNextStepDateStrings[date]);
}

MaxNextStepDate maxNextStepDateFromString(const QString &str)
{
    static int s_count = sizeof(s_maxNextStepDateStrings) / sizeof(*s_maxNextStepDateStrings);
    for (int i = 0; i < s_count; ++i) {
        if (str == s_maxNextStepDateStrings[i]) {
            return MaxNextStepDate(i);
        }
    }
    qWarning() << "Unknown value for MaxNextStepDate enum:" << str;
    return NoDate;
}
