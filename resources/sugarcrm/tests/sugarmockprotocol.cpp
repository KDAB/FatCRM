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
#include "campaignshandler.h"
#include "leadshandler.h"
#include "contactshandler.h"
#include "listentriesscope.h"
#include "modulename.h"
#include <QtDBus/QDBusConnection>

SugarMockProtocol::SugarMockProtocol()
    :mServerNotFound(false)
{
    QDBusConnection::sessionBus().registerObject(QLatin1String("/CRMMock"),
            this,
            QDBusConnection::ExportScriptableSlots);
}



int SugarMockProtocol::login(const QString &user, const QString &password, QString &sessionId, QString &errorMessage)
{
    if (!mServerNotFound) {
        if (user == "user" && password == "password") {
            sessionId = '1';
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

int SugarMockProtocol::getEntriesCount(const ListEntriesScope &scope, Module moduleName, const QString &query,
                                       int &entriesCount, QString &errorMessage)
{
    Q_UNUSED(scope);
    Q_UNUSED(query);
    Q_UNUSED(errorMessage);
    if (moduleName == Module::Accounts) {
        entriesCount = mAccounts.size();
    } else if (moduleName == Module::Opportunities) {
        entriesCount = mOpportunities.size();
    } else if (moduleName == Module::Campaigns) {
        entriesCount = mCampaigns.size();
    } else if (moduleName == Module::Leads) {
        entriesCount = mLeads.size();
    } else if (moduleName == Module::Contacts) {
        entriesCount = mContacts.size();
    } else {
        entriesCount = 0;
    }

    return 0;
}

QList<KDSoapGenerated::TNS__Entry_value> SugarMockProtocol::listAccount() const
{
    QList<KDSoapGenerated::TNS__Entry_value> items;
    for (const SugarAccount &acc : mAccounts) {
        KDSoapGenerated::TNS__Entry_value entryValue;
        entryValue.setId(acc.id());
        entryValue.setModule_name(moduleToName(Module::Accounts));
        KDSoapGenerated::TNS__Name_value_list nvl = AccountsHandler::sugarAccountToNameValueList(acc);
        entryValue.setName_value_list(nvl);
        items.push_back(entryValue);
    }
    return items;
}

QList<KDSoapGenerated::TNS__Entry_value> SugarMockProtocol::listOpportunities() const
{
    QList<KDSoapGenerated::TNS__Entry_value> items;
    for (const SugarOpportunity &opp : mOpportunities) {
        KDSoapGenerated::TNS__Entry_value entryValue;
        entryValue.setId(opp.id());
        entryValue.setModule_name(moduleToName(Module::Opportunities));
        KDSoapGenerated::TNS__Name_value_list nvl = OpportunitiesHandler::sugarOpportunityToNameValueList(opp);
        entryValue.setName_value_list(nvl);
        items.push_back(entryValue);
    }
    return items;
}

QList<KDSoapGenerated::TNS__Entry_value> SugarMockProtocol::listCampaigns() const
{
    QList<KDSoapGenerated::TNS__Entry_value> items;
    for (const SugarCampaign &cam : mCampaigns)
    for ( int i = 0; i < mCampaigns.size(); ++i) {
        KDSoapGenerated::TNS__Entry_value entryValue;
        entryValue.setId(cam.id());
        entryValue.setModule_name(moduleToName(Module::Campaigns));
        KDSoapGenerated::TNS__Name_value_list nvl = CampaignsHandler::sugarCampaignToNameValueList(cam);
        entryValue.setName_value_list(nvl);
        items.push_back(entryValue);
    }
    return items;
}

QList<KDSoapGenerated::TNS__Entry_value> SugarMockProtocol::listLeads() const
{
    QList<KDSoapGenerated::TNS__Entry_value> items;
    for (const SugarLead &lead : mLeads) {
        KDSoapGenerated::TNS__Entry_value entryValue;
        entryValue.setId(lead.id());
        entryValue.setModule_name(moduleToName(Module::Leads));
        KDSoapGenerated::TNS__Name_value_list nvl = LeadsHandler::sugarLeadToNameValueList(lead);
        entryValue.setName_value_list(nvl);
        items.push_back(entryValue);
    }
    return items;
}

QList<KDSoapGenerated::TNS__Entry_value> SugarMockProtocol::listContacts() const
{
    QList<KDSoapGenerated::TNS__Entry_value> items;
    for (const KABC::Addressee &contact : mContacts) {
        KDSoapGenerated::TNS__Entry_value entryValue;
        const QString id = contact.custom("FATCRM", "X-ContactId");
        entryValue.setId(id);
        entryValue.setModule_name("Contacts");
        KDSoapGenerated::TNS__Name_value_list nvl = ContactsHandler::addresseeToNameValueList(contact);
        entryValue.setName_value_list(nvl);
        items.push_back(entryValue);
    }
    return items;
}

int SugarMockProtocol::listEntries(const ListEntriesScope &scope, Module moduleName, const QString &query, const QString &orderBy,
                                   const QStringList &selectedFields, EntriesListResult &entriesListResult, QString &errorMessage)
{
    Q_UNUSED(query); Q_UNUSED(orderBy);
    Q_UNUSED(selectedFields); Q_UNUSED(errorMessage);

    if (scope.offset() > 0) {
        entriesListResult.resultCount = 0;
    } else {
        if (moduleName == Module::Accounts) {
            entriesListResult.resultCount = mAccounts.size();
            entriesListResult.entryList.setItems(listAccount());
        } else if (moduleName == Module::Opportunities) {
            entriesListResult.resultCount = mOpportunities.size();
            entriesListResult.entryList.setItems(listOpportunities());
        } else if (moduleName == Module::Campaigns) {
            entriesListResult.resultCount = mCampaigns.size();
            entriesListResult.entryList.setItems(listCampaigns());
        } else if (moduleName == Module::Leads) {
            entriesListResult.resultCount = mLeads.size();
            entriesListResult.entryList.setItems(listLeads());
        } else if (moduleName == Module::Contacts) {
            entriesListResult.resultCount = mContacts.size();
            entriesListResult.entryList.setItems(listContacts());
        } else {
            entriesListResult.resultCount = -1;
        }
    }
    entriesListResult.nextOffset += entriesListResult.resultCount;
    return 0;
}

int SugarMockProtocol::getEntry(Module moduleName, const QString &remoteId, const QStringList &selectedFields, KDSoapGenerated::TNS__Entry_value &entryValue, QString &errorMessage)
{
    Q_UNUSED(selectedFields);
    Q_UNUSED(errorMessage);
    bool found = false;
    if (moduleName == Module::Accounts) {
        for (int i = 0; i < mAccounts.size() && !found; ++i) {
            if (mAccounts.at(i).id() == remoteId) {
                found = true;
                entryValue.setId(mAccounts.at(i).id());
                KDSoapGenerated::TNS__Name_value_list nvl = AccountsHandler::sugarAccountToNameValueList(mAccounts.at(i));
                entryValue.setName_value_list(nvl);
            }
        }
    } else if (moduleName == Module::Opportunities) {
        for (int i = 0; i < mOpportunities.size() && !found; ++i) {
            if (mOpportunities.at(i).id() == remoteId) {
                found = true;
                entryValue.setId(mOpportunities.at(i).id());
                KDSoapGenerated::TNS__Name_value_list nvl = OpportunitiesHandler::sugarOpportunityToNameValueList(mOpportunities.at(i));
                entryValue.setName_value_list(nvl);
            }
        }
    }
    return found ? int(KJob::NoError) : int(SugarJob::SoapError);
}

int SugarMockProtocol::setEntry(Module moduleName, const KDSoapGenerated::TNS__Name_value_list& nameValueList, QString &newId, QString &errorMessage)
{
    Q_UNUSED(errorMessage);
    QList<KDSoapGenerated::TNS__Name_value> list = nameValueList.items();
    bool deleted = std::any_of(list.begin(), list.end(), [](const KDSoapGenerated::TNS__Name_value &nv){return nv.name() == "deleted" && nv.value() == "1";});
    QString id;
    auto result = std::find_if(list.begin(), list.end(), [](const KDSoapGenerated::TNS__Name_value &nv){return nv.name() == "id";});
    if (result != list.end()) {
        id = result->value();
    }

    if (moduleName == Module::Accounts) {
        SugarAccount account = AccountsHandler::nameValueListToSugarAccount(nameValueList,id);
        if (!id.isEmpty()) {
            bool found = false;
            for (int i = 0; i < mAccounts.size(); ++i) {
                if (mAccounts.at(i).id() == id) {
                    found = true;
                    if (deleted) {
                        mAccounts.remove(i);
                    } else {
                        account.setId(mAccounts.at(i).id());
                        mAccounts[i] = account;
                    }
                    break;
                }
            }
            if (!found) {
                return SugarJob::SoapError;
            }
        } else { // create an account
            if (!deleted) {
                account.setId(QString::number(mNextId++));
                mAccounts.push_back(account);
                newId = account.id();
            } else {
                return SugarJob::SoapError;
            }
        }
    } else if (moduleName == Module::Opportunities) {
        SugarOpportunity opp = OpportunitiesHandler::nameValueListToSugarOpportunity(nameValueList,id);
        if (!id.isEmpty()) {
            bool found = false;
            for (int i = 0; i < mOpportunities.size() && !found; ++i) {
                if (mOpportunities.at(i).id() == id) {
                    found = true;
                    if (deleted) {
                        mOpportunities.remove(i);
                    } else {
                        opp.setId(mOpportunities.at(i).id());
                        mOpportunities[i] = opp;
                    }
                    break;
                }
            }
            if (!found) {
                return SugarJob::SoapError;
            }
        } else { // create an opp
            if (!deleted) {
                opp.setId(QString::number(mNextId++));
                mOpportunities.push_back(opp);
                newId = opp.id();
            } else {
                return SugarJob::SoapError;
            }
        }
    }
    return KJob::NoError;
}

void SugarMockProtocol::addAccounts()
{
    mAccounts.resize(3);
    mAccounts[0].setName("accountZero");
    mAccounts[0].setId("0");
    mAccounts[1].setName("accountOne");
    mAccounts[1].setId("1");
    mAccounts[2].setName("accountTwo");
    mAccounts[2].setId("2");
}

void SugarMockProtocol::addOpportunities()
{
    mOpportunities.resize(2);
    mOpportunities[0].setId("100");
    mOpportunities[0].setName("validOpp");
    mOpportunities[0].setTempAccountName("accountZero");
    mOpportunities[1].setId("101");
    mOpportunities[1].setName("oppWithNonExistingAccount");
    mOpportunities[1].setTempAccountName("accountTest");
}

void SugarMockProtocol::addData()
{
    addAccounts();
    addOpportunities();

    mCampaigns.resize(1);
    mLeads.resize(1);
    mContacts.resize(1);
}

void SugarMockProtocol::addAccount(const QString &name, const QString &id)
{
    SugarAccount account;
    account.setName(name);
    account.setId(id);
    mAccounts.push_back(account);
}

void SugarMockProtocol::deleteAccount(const QString &id)
{
    auto result = std::find_if(mAccounts.begin(), mAccounts.end(), [id](const SugarAccount &account) {return account.id() == id;});
    if (result != mAccounts.end()) {
        mAccounts.erase(result);
    } else {
        qDebug() << "error: non-existent id";
    }
}

void SugarMockProtocol::updateAccount(const QString &name, const QString &id)
{
    auto result = std::find_if(mAccounts.begin(), mAccounts.end(), [id](const SugarAccount &account) {return account.id() == id;});
    if (result != mAccounts.end()) {
        result->setName(name);
    } else {
        qDebug() << "error: non-existent id";
    }
}

bool SugarMockProtocol::accountExists(const QString &name, const QString &id)
{
    auto result = std::find_if(mAccounts.begin(), mAccounts.end(),
                               [id, name](const SugarAccount &account) {return account.id() == id && account.name() == name;});
    return result != mAccounts.end();

}

void SugarMockProtocol::addOpportunity(const QString &name, const QString &id)
{
    SugarOpportunity opp;
    opp.setName(name);
    opp.setId(id);
    mOpportunities.push_back(opp);
}

void SugarMockProtocol::deleteOpportunity(const QString &id)
{
    auto result = std::find_if(mOpportunities.begin(), mOpportunities.end(), [id](const SugarOpportunity &opp) {return opp.id() == id;});
    if (result != mOpportunities.end()) {
        mOpportunities.erase(result);
    } else {
        qDebug() << "error: non-existent id";
    }
}

void SugarMockProtocol::updateOpportunity(const QString &name, const QString &id)
{
    auto result = std::find_if(mOpportunities.begin(), mOpportunities.end(), [id](const SugarOpportunity &opp) {return opp.id() == id;});
    if (result != mOpportunities.end()) {
        result->setName(name);
    } else {
        qDebug() << "error: non-existent id";
    }
}

bool SugarMockProtocol::opportunityExists(const QString &name, const QString &id)
{
    auto result = std::find_if(mOpportunities.begin(), mOpportunities.end(),
                               [id, name](const SugarOpportunity &opp) {return opp.id() == id && opp.name() == name;});
    return result != mOpportunities.end();
}

QVector<SugarAccount> SugarMockProtocol::accounts() const
{
    return mAccounts;
}

QVector<SugarOpportunity> SugarMockProtocol::opportunities() const
{
    return mOpportunities;
}

int SugarMockProtocol::listModules(KDSoapGenerated::TNS__Select_fields &selectFields, QString &errorMessage)
{
    Q_UNUSED(errorMessage);
    QStringList modules;
    modules << moduleToName(Module::Accounts)
            << moduleToName(Module::Opportunities)
            << moduleToName(Module::Contacts)
            << moduleToName(Module::Documents)
            << moduleToName(Module::Emails);
    selectFields.setItems(modules);
    return KJob::NoError;
}
