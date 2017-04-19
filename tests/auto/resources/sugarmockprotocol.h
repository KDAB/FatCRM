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

#ifndef SUGARMOCKPROTOCOL_H
#define SUGARMOCKPROTOCOL_H

#include <QString>
#include <QVector>
#include "sugarprotocolbase.h"
#include "sugaraccount.h"
#include "sugaropportunity.h"
#include "sugarcampaign.h"
#include "sugarlead.h"
#include <KABC/Addressee>

#include "sugarsoap.h"

class AccountsHandler;
class OpportunitiesHandler;
class CampaignsHandler;
class LeadsHandler;
class ContactsHandler;
class ModuleHandler;
namespace KDSoapGenerated
{
class TNS__Get_entry_list_result;
}

class SugarMockProtocol : public SugarProtocolBase
{
public:
    SugarMockProtocol();
    int login(const QString &user, const QString &password, QString &sessionId, QString &errorMessage) override;
    void logout() override;
    inline void setServerNotFound(bool serverNotFound) { mServerNotFound = serverNotFound; }
    void setSession(SugarSession *session) override;
    int getEntriesCount(const ListEntriesScope &scope, const QString &moduleName, const QString &query, int &entriesCount, QString &errorMessage) override;
    int listEntries(const ListEntriesScope &scope, const QString &moduleName, const QString &query,
                    const QString &orderBy, const QStringList &selectedFields, EntriesListResult &entriesListResult,
                    QString &errorMessage) override;

    void setAccountsHandler(AccountsHandler *handler);
    void setOpportunitiesHandler(OpportunitiesHandler *handler);
    void setCampaignsHandler(CampaignsHandler *handler);
    void setLeadsHandler(LeadsHandler *handler);
    void setContactsHandler(ContactsHandler *handler);

private:
    AccountsHandler *mAccountHandler;
    OpportunitiesHandler *mOpportunityHandler;
    CampaignsHandler *mCampaignHandler;
    LeadsHandler *mLeadHandler;
    ContactsHandler *mContactHandler;
    bool mServerNotFound;
    QVector<SugarAccount> mAccounts;
    QVector<SugarOpportunity> mOpportunities;
    QVector<SugarCampaign> mCampaigns;
    QVector<SugarLead> mLeads;
    QVector<KABC::Addressee> mContacts;

    QList<KDSoapGenerated::TNS__Entry_value> listAccount() const;
    QList<KDSoapGenerated::TNS__Entry_value> listOpportunities() const;
    QList<KDSoapGenerated::TNS__Entry_value> listCampaigns() const;
    QList<KDSoapGenerated::TNS__Entry_value> listLeads() const;
    QList<KDSoapGenerated::TNS__Entry_value> listContacts() const;
};

#endif // SUGARMOCKPROTOCOL_H
