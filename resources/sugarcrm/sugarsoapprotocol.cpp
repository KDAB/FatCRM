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

#include "sugarsoapprotocol.h"
#include "sugarcrmresource_debug.h"
#include "sugarjob.h"
#include "sugarsession.h"
#include "listentriesscope.h"
#include <QNetworkReply>
#include <KLocalizedString>
#include <QEventLoop>

SugarSoapProtocol::SugarSoapProtocol()
{
}

int SugarSoapProtocol::login(const QString &user, const QString &password, QString &sessionId, QString &errorMessage)
{
    auto* soap = mSession->soap();
    Q_ASSERT(soap != nullptr);

    const QByteArray passwordHash = password.toUtf8();

    KDSoapGenerated::TNS__User_auth userAuth;
    userAuth.setUser_name(user);
    userAuth.setPassword(QString::fromLatin1(passwordHash));

    KDSoapGenerated::TNS__Name_value_list nameValueList;
    KDSoapGenerated::TNS__Name_value nameValue1;
    nameValue1.setName("notifyonsave");
    nameValue1.setValue("true");
    nameValueList.setItems({nameValue1});

    const auto entry_result = soap->login(userAuth, QLatin1String("FatCRM"), nameValueList);
    if (soap->lastError() == 0) {
        sessionId = entry_result.id();
        return KJob::NoError;
    } else {
        errorMessage = i18nc("@info:status", "Login for user %1 on %2 failed: %3", user, mSession->host(), soap->lastError());
        const int faultcode = soap->lastErrorCode();
        if (faultcode == QNetworkReply::UnknownNetworkError ||
           faultcode == QNetworkReply::HostNotFoundError) {
            return SugarJob::CouldNotConnectError;
        } else {
            return SugarJob::LoginError;
        }
    }
}

void SugarSoapProtocol::logout()
{
    auto *soap = mSession->soap();
    if (mSession->sessionId().isEmpty() && soap != nullptr) {
        soap->logout(mSession->sessionId());
        if (soap->lastErrorCode() != 0) {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "logout had fault" << soap->lastErrorCode() << soap->lastError();
        }
    }
    mSession->forgetSession();
}

static int checkError(KDSoapGenerated::Sugarsoap *soap, const char* methodName, QString &errorMessage)
{
    if (soap->lastErrorCode() == 0) {
        return KJob::NoError;
    } else if (soap->lastErrorCode() == 10){
        errorMessage = soap->lastError();
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << methodName << "returned error" << errorMessage << " -- interpreting this as CouldNotConnectError";
        return SugarJob::CouldNotConnectError;
    } else {
        errorMessage = soap->lastError();
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << methodName << "returned error" << soap->lastErrorCode() << soap->lastError();
        return SugarJob::SoapError;
    }
}

int SugarSoapProtocol::getEntriesCount(const ListEntriesScope &scope, Module moduleName, const QString &query,
                                       int &entriesCount, QString &errorMessage)
{
    auto *soap = mSession->soap();
    KDSoapGenerated::TNS__Get_entries_count_result entry_result =
            soap->get_entries_count(mSession->sessionId(), moduleToName(moduleName), query, scope.includeDeleted());
    entriesCount = entry_result.result_count();
    return checkError(soap, "getEntriesCount", errorMessage);
}

int SugarSoapProtocol::getModuleFields(const QString &moduleName, KDSoapGenerated::TNS__Field_list& fields, QString &errorMessage)
{
    // https://support.sugarcrm.com/Documentation/Sugar_Developer/Sugar_Developer_Guide_6.5/Application_Framework/Web_Services/Method_Calls/get_module_fields/
    auto *soap = mSession->soap();
    const KDSoapGenerated::TNS__New_module_fields result = soap->get_module_fields(mSession->sessionId(), moduleName, {});
    fields = result.module_fields();
    return checkError(soap, "getModuleFields", errorMessage);
}

int SugarSoapProtocol::listEntries(const ListEntriesScope &scope, Module moduleName, const QString &query,
                                   const QString &orderBy, const QStringList &selectedFields, EntriesListResult &entriesListResult,
                                   QString &errorMessage)
{
    auto *soap = mSession->soap();
    const int offset = scope.offset();
    const int maxResults = 100;
    const int fetchDeleted = scope.includeDeleted();

    KDSoapGenerated::TNS__Select_fields Fields;
    Fields.setItems(selectedFields);

    auto *job = new KDSoapGenerated::Get_entry_listJob(soap);
    job->setSession(mSession->sessionId());
    job->setModule_name(moduleToName(moduleName));
    job->setQuery(query);
    job->setOrder_by(orderBy);
    job->setOffset(offset);
    job->setSelect_fields(Fields);
    job->setMax_results(maxResults);
    job->setDeleted(fetchDeleted);

    QEventLoop eventLoop;
    QObject::connect(job, SIGNAL(finished(KDSoapJob*)), &eventLoop, SLOT(quit()));
    job->start();
    eventLoop.exec();

    const KDSoapGenerated::TNS__Get_entry_list_result_version2 entry_result = job->return_();

    entriesListResult.entryList = entry_result.entry_list();
    entriesListResult.nextOffset = entry_result.next_offset();
    entriesListResult.resultCount = entry_result.result_count();

    return checkError(soap, "listEntries", errorMessage);
}

int SugarSoapProtocol::setEntry(Module moduleName, const KDSoapGenerated::TNS__Name_value_list& name_value_list, QString &id, QString &errorMessage)
{
    auto *soap = mSession->soap();
    KDSoapGenerated::TNS__New_set_entry_result result = soap->set_entry(mSession->sessionId(), moduleToName(moduleName), name_value_list);
    id = result.id();
    return checkError(soap, "setEntry", errorMessage);
}

int SugarSoapProtocol::getEntry(Module moduleName, const QString &remoteId, const QStringList &selectedFields, KDSoapGenerated::TNS__Entry_value &entryValue, QString &errorMessage)
{
    auto *soap = mSession->soap();
    KDSoapGenerated::TNS__Select_fields fields;
    fields.setItems(selectedFields);
    // https://support.sugarcrm.com/Documentation/Sugar_Developer/Sugar_Developer_Guide_6.5/Application_Framework/Web_Services/Method_Calls/get_entry/
    KDSoapGenerated::TNS__Get_entry_result_version2 result = soap->get_entry(mSession->sessionId(), moduleToName(moduleName), remoteId, fields, {} /*link_..._array*/, false /*track_view*/);
    if (!result.entry_list().items().isEmpty()) {
        entryValue = result.entry_list().items().at(0);
    }
    return checkError(soap, "getEntry", errorMessage);
}

int SugarSoapProtocol::listModules(QStringList &moduleNames, QString &errorMessage)
{
    auto *soap = mSession->soap();
    const KDSoapGenerated::TNS__Module_list result = soap->get_available_modules(mSession->sessionId(), QString() /*filter*/);
    moduleNames.clear();
    const auto moduleItems = result.modules().items();
    moduleNames.reserve(moduleItems.size());
    for (const auto &module : moduleItems) {
        //qDebug() << module.module_key() << module.module_label();
        moduleNames << module.module_key();
    }
    return checkError(soap, "listModules", errorMessage);
}
