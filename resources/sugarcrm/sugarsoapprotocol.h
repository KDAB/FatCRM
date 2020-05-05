/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2017-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef SUGARSOAPPROTOCOL_H
#define SUGARSOAPPROTOCOL_H

#include <QString>
#include "sugarsoap.h"
#include "sugarprotocolbase.h"


class SugarSoapProtocol : public SugarProtocolBase
{
public:
    SugarSoapProtocol();
    int login(const QString &user, const QString &password, QString &sessionId, QString &errorMessage) override;
    void logout() override;
    inline void setSession(SugarSession *session) override { mSession = session; }
    int getEntriesCount(const ListEntriesScope &scope, Module moduleName, const QString &query, int &entriesCount, QString &errorMessage) override;
    int getModuleFields(const QString &moduleName, KDSoapGenerated::TNS__Field_list &fields, QString &errorMessage) override;
    int listEntries(const ListEntriesScope &scope, Module moduleName, const QString &query,
                     const QString &orderBy, const QStringList &selectedFields, EntriesListResult &entriesListResult,
                    QString &errorMessage) override;
    int setEntry(Module moduleName, const KDSoapGenerated::TNS__Name_value_list& name_value_list, QString &id, QString &errorMessage) override;
    int getEntry(Module moduleName, const QString &remoteId, const QStringList &selectedFields,
                 KDSoapGenerated::TNS__Entry_value &entryValue, QString &errorMessage) override;
    int listModules(QStringList &moduleNames, QString &errorMessage) override;
private:
    SugarSession *mSession = nullptr;
};

#endif // SUGARSOAPPROTOCOL_H
