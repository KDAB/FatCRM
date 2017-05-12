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

#ifndef LISTENTRIESSCOPE_H
#define LISTENTRIESSCOPE_H

#include <QString>
#include "modulename.h"

class ListEntriesScope
{
public:
    ListEntriesScope();
    explicit ListEntriesScope(const QString &timestamp);

    QString timestamp() const;
    bool isUpdateScope() const;

    void setOffset(int offset);

    int offset() const;

    void fetchDeleted();

    int deleted() const;

    QString query(const QString &filter, Module moduleName) const;

private:
    int mOffset;
    QString mUpdateTimestamp;
    bool mGetDeleted;
};

#endif // LISTENTRIESSCOPE_H
