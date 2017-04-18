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

#include "sugarmockprotocol.h"
#include "sugarjob.h"
#include "accountshandler.h"
#include "opportunitieshandler.h"

SugarMockProtocol::SugarMockProtocol()
    : mAccountHandler(0), mOpportunityHandler(0), mServerNotFound(false)
{
    mAccounts.resize(3);
    mOpportunities.resize(2);
}



int SugarMockProtocol::login(const QString &user, const QString &password, QString &sessionId, QString &errorMessage)
{
    if (!mServerNotFound) {
        if (user == "user" && password == "password") {
            sessionId = "1";
            return KJob::NoError;
        } else {
            int error = SugarJob::Errors::LoginError;
            errorMessage = QString("Login for user %1 on %2 failed: %3").arg(user).arg("test").arg(error);
            return error;
        }
    } else {
        return SugarJob::Errors::CouldNotConnectError;
    }
}

void SugarMockProtocol::logout()
{
}

void SugarMockProtocol::setSession(SugarSession *session)
{
    Q_UNUSED(session);
}

int SugarMockProtocol::getEntriesCount(const ListEntriesScope &scope, const QString &moduleName, const QString &query,
                                       int &entriesCount, QString &errorMessage)
{
    Q_UNUSED(scope);
    Q_UNUSED(query);
    Q_UNUSED(errorMessage);
    if (moduleName == "account") {
        entriesCount = mAccounts.size();
    } else if (moduleName == "opportunity") {
        entriesCount = mOpportunities.size();
    } else {
        entriesCount = 0;
    }

    return 0;
}

QList<KDSoapGenerated::TNS__Entry_value> SugarMockProtocol::listAccount() const
{
    QList<KDSoapGenerated::TNS__Entry_value> items;
    for ( int i = 0; i < mAccounts.size(); ++i) {
        KDSoapGenerated::TNS__Entry_value entryValue;
        entryValue.setId(QString::number(i));
        entryValue.setModule_name("account");
        KDSoapGenerated::TNS__Name_value_list nvl = mAccountHandler->SugarAccountToNameValueList(mAccounts.at(i));
        entryValue.setName_value_list(nvl);
        items.push_back(entryValue);
    }
    return items;
}

QList<KDSoapGenerated::TNS__Entry_value> SugarMockProtocol::listOpportunities() const
{
    QList<KDSoapGenerated::TNS__Entry_value> items;
    for ( int i = 0; i < mOpportunities.size(); ++i) {
        KDSoapGenerated::TNS__Entry_value entryValue;
        entryValue.setId(QString::number(i));
        entryValue.setModule_name("opportunity");
        KDSoapGenerated::TNS__Name_value_list nvl = mOpportunityHandler->SugarOpportunityToNameValueList(mOpportunities.at(i));
        entryValue.setName_value_list(nvl);
        items.push_back(entryValue);
    }
    return items;
}

int SugarMockProtocol::listEntries(const ListEntriesScope &scope, const QString &moduleName, const QString &query, const QString &orderBy,
                                   const QStringList &selectedFields, EntriesListResult &entriesListResult, QString &errorMessage)
{
    Q_UNUSED(scope); Q_UNUSED(moduleName);
    Q_UNUSED(query); Q_UNUSED(orderBy);
    Q_UNUSED(selectedFields); Q_UNUSED(errorMessage);

    if (moduleName == "account") {
        entriesListResult.resultCount = mAccounts.size();
        entriesListResult.entryList.setItems(listAccount());
    } else if (moduleName == "opportunity") {
        entriesListResult.resultCount = mOpportunities.size();
        entriesListResult.entryList.setItems(listOpportunities());
    }
    return 0;
}

void SugarMockProtocol::setAccountsHandler(AccountsHandler *handler)
{
    mAccountHandler = handler;
}

void SugarMockProtocol::setOpportunitiesHandler(OpportunitiesHandler *handler)
{
    mOpportunityHandler = handler;
}
