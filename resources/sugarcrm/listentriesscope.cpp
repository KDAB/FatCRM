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

#include "listentriesscope.h"

ListEntriesScope::ListEntriesScope()
{
}

ListEntriesScope::ListEntriesScope(const QString &timestamp)
    : mOffset(0),
      mUpdateTimestamp(timestamp),
      mGetDeleted(false)
{
}

QString ListEntriesScope::timestamp() const
{
    return mUpdateTimestamp;
}

bool ListEntriesScope::isUpdateScope() const
{
    return !mUpdateTimestamp.isEmpty();
}

void ListEntriesScope::setOffset(int offset)
{
    mOffset = offset;
}

int ListEntriesScope::offset() const
{
    return mOffset;
}

void ListEntriesScope::fetchDeleted()
{
    mGetDeleted = true;
    mOffset = 0;
}

int ListEntriesScope::includeDeleted() const
{
    return mGetDeleted ? 1 : 0;
}

QString ListEntriesScope::query(const QString &filter, Module moduleName) const
{
    QString queryStr = filter;

    if (!filter.isEmpty()) {
        queryStr = filter;
    }

    if (mUpdateTimestamp.isEmpty()) {
        return queryStr;
    }

    if (!queryStr.isEmpty())
        queryStr = QLatin1String("( ") + queryStr + QLatin1String(" ) AND ");

    return queryStr + moduleToName(moduleName).toLower() + QLatin1String(".date_modified >= '") + mUpdateTimestamp + QLatin1String("'");
}
