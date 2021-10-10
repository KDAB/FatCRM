/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    if (soap->lastErrorCode() == 0) {
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
    QObject::connect(job, &KDSoapJob::finished, &eventLoop, &QEventLoop::quit);
    job->start();
    eventLoop.exec();

    if (job->isFault()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "listing entries in" << moduleName << "returned error" << job->faultAsString();
        errorMessage = job->faultAsString();
        return SugarJob::SoapError;
    }

    const KDSoapGenerated::TNS__Get_entry_list_result_version2 entry_result = job->return_();

    entriesListResult.entryList = entry_result.entry_list();
    entriesListResult.nextOffset = entry_result.next_offset();
    entriesListResult.resultCount = entry_result.result_count();

    //qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "got" << entriesListResult.entryList.items().count() << "entries in" << moduleName;

    return checkError(soap, "listEntries", errorMessage);
}

int SugarSoapProtocol::setEntry(Module moduleName, const KDSoapGenerated::TNS__Name_value_list& name_value_list, QString &id, QString &errorMessage)
{
    auto *soap = mSession->soap();
    KDSoapGenerated::TNS__New_set_entry_result result = soap->set_entry(mSession->sessionId(), moduleToName(moduleName), name_value_list);
    id = result.id();
    return checkError(soap, "setEntry", errorMessage);
}

SugarProtocolBase::GetRelationShipsResult SugarSoapProtocol::getRelationships(const QString &sourceItemId, Module sourceModule, Module targetModule) const
{
    GetRelationShipsResult ret;
    KDSoapGenerated::TNS__Select_fields selectedFields;
    selectedFields.setItems({QStringLiteral("id")});

    // Get the Contact(s) related to this opportunity
    // https://support.sugarcrm.com/Documentation/Sugar_Developer/Sugar_Developer_Guide_10.0/Integration/Web_Services/Legacy_API/Methods/get_relationships/
    // https://support.sugarcrm.com/Documentation/Sugar_Developer/Sugar_Developer_Guide_10.0/Cookbook/Web_Services/Legacy_API/SOAP/PHP/Retrieving_Related_Records/
    KDSoapGenerated::TNS__Get_entry_result_version2 result = mSession->soap()->get_relationships(mSession->sessionId(), moduleToName(sourceModule), sourceItemId,
                                                                                                 moduleToName(targetModule).toLower(), {}, selectedFields,
                                                                                                 {}, 0 /*deleted*/, QString(), 0 /*offset*/, 0 /*limit*/);
    const auto &items = result.entry_list().items();
    if (!items.isEmpty()) {
        ret.ids.reserve(items.size());
        auto extractId = [](const KDSoapGenerated::TNS__Entry_value& entry) { return entry.id(); };
        std::transform(items.begin(), items.end(), std::back_inserter(ret.ids), extractId);
    }
    ret.errorCode = checkError(mSession->soap(), "getRelationships", ret.errorMessage);
    return ret;

}

int SugarSoapProtocol::setRelationship(const QString &sourceItemId, Module sourceModule, const QStringList &targetItemIds, Module targetModule, bool shouldDelete, QString &errorMessage) const
{
     auto *soap = mSession->soap();

     KDSoapGenerated::TNS__Select_fields relatedIds;
     relatedIds.setItems(targetItemIds);

     const QString sourceModuleName = moduleToName(sourceModule);
     const QString targetModuleName = moduleToName(targetModule);
     const KDSoapGenerated::TNS__New_set_relationship_list_result result =
             soap->set_relationship(mSession->sessionId(), sourceModuleName, sourceItemId, targetModuleName.toLower(),
                                    relatedIds, KDSoapGenerated::TNS__Name_value_list(), shouldDelete ? 1 : 0);
     if ((!shouldDelete && result.created()) || (shouldDelete && result.deleted())) {
         return KJob::NoError;
     }

     const QString baseErrorMessage = QStringLiteral("Unable to link %1 %2 to %3 %4").arg(sourceModuleName, sourceItemId, targetModuleName, targetItemIds.join(','));
     if (!soap->lastError().isEmpty()) {
         errorMessage = baseErrorMessage + ":" + soap->lastError();
     } else if (result.failed()) {
         errorMessage = baseErrorMessage + ": the server said 'failed'";
     }
     qWarning() << errorMessage;
     return KJob::UserDefinedError;
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
