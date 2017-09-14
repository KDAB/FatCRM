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
#include <KContacts/Addressee>
#include <QObject>

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
class TNS__Select_fields;
}

class SugarMockProtocol : public QObject, public SugarProtocolBase
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.kdab.SugarCRM.Mock")
public:
    SugarMockProtocol();
    int login(const QString &user, const QString &password, QString &sessionId, QString &errorMessage) override;
    void logout() override;
    void setServerNotFound(bool serverNotFound) { mServerNotFound = serverNotFound; }
    void setSession(SugarSession *session) override;
    int getEntriesCount(const ListEntriesScope &scope, Module moduleName, const QString &query, int &entriesCount, QString &errorMessage) override;
    int listEntries(const ListEntriesScope &scope, Module moduleName, const QString &query,
                    const QString &orderBy, const QStringList &selectedFields, EntriesListResult &entriesListResult,
                    QString &errorMessage) override;
    int setEntry(Module module_name, const KDSoapGenerated::TNS__Name_value_list& name_value_list, QString &idItemCreate, QString &errorMessage) override;
    int getEntry(Module moduleName, const QString &remoteId, const QStringList &selectedFields,
                 KDSoapGenerated::TNS__Entry_value &entryValue, QString &errorMessage) override;
    int listModules(KDSoapGenerated::TNS__Select_fields &selectFields, QString &errorMessage) override;

    void setAccountsHandler(AccountsHandler *handler);
    void setOpportunitiesHandler(OpportunitiesHandler *handler);
    void setCampaignsHandler(CampaignsHandler *handler);
    void setLeadsHandler(LeadsHandler *handler);
    void setContactsHandler(ContactsHandler *handler);

    void addData();
    void addAccounts();
    void addOpportunities();



    QVector<SugarAccount> accounts() const;
    QVector<SugarOpportunity> opportunities() const;

public Q_SLOTS:
    Q_SCRIPTABLE void setNextSoapError(const QString &soapError) { mNextSoapError = soapError; }
    Q_SCRIPTABLE void addAccount(const QString &name, const QString &id);
    Q_SCRIPTABLE void deleteAccount(const QString &id);
    Q_SCRIPTABLE void updateAccount(const QString &name, const QString &id);
    Q_SCRIPTABLE void addOpportunity(const QString &name, const QString &id);
    Q_SCRIPTABLE void deleteOpportunity(const QString &id);
    Q_SCRIPTABLE void updateOpportunity(const QString &name, const QString &id);

    Q_SCRIPTABLE bool accountExists(const QString &name, const QString &id);
    Q_SCRIPTABLE bool opportunityExists(const QString &name, const QString &id);
    Q_SCRIPTABLE QString sessionId();

private:
    bool mServerNotFound;
    SugarSession *mSession;
    QString mNextSoapError;
    QVector<SugarAccount> mAccounts;
    QVector<SugarOpportunity> mOpportunities;
    QVector<SugarCampaign> mCampaigns;
    QVector<SugarLead> mLeads;
    QVector<KContacts::Addressee> mContacts;
    int mNextId = 1000;

    QList<KDSoapGenerated::TNS__Entry_value> listAccount(bool deleted) const;
    QList<KDSoapGenerated::TNS__Entry_value> listOpportunities(bool deleted) const;
    QList<KDSoapGenerated::TNS__Entry_value> listCampaigns(bool deleted) const;
    QList<KDSoapGenerated::TNS__Entry_value> listLeads(bool deleted) const;
    QList<KDSoapGenerated::TNS__Entry_value> listContacts(bool deleted) const;
};

#endif // SUGARMOCKPROTOCOL_H
