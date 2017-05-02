/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef SUGARPROTOCOLBASE_H
#define SUGARPROTOCOLBASE_H

#include <QString>
#include "sugarsoap.h"

class ListEntriesScope;
class SugarSession;
struct EntriesListResult
{
    int resultCount;
    int nextOffset = 0;
    KDSoapGenerated::TNS__Entry_list entryList;
    KDSoapGenerated::TNS__Field_list fieldList;
};

class SugarProtocolBase
{
public:
    virtual ~SugarProtocolBase();
    virtual int login(const QString &user, const QString &password, QString &sessionId, QString &errorMessage) = 0;
    virtual void logout() = 0;
    virtual void setSession(SugarSession *session) = 0;
    virtual int getEntriesCount(const ListEntriesScope &scope, const QString &moduleName, const QString &query, int &entriesCount, QString &errorMssage) = 0;
    virtual int listEntries(const ListEntriesScope &scope, const QString &moduleName, const QString &query,
                             const QString &orderBy, const QStringList &selectedFields, EntriesListResult &entriesListResult,
                            QString &errorMessage) = 0;
};



#endif /* SUGARPROTOCOLBASE_H */
