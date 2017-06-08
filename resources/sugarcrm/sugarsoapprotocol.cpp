/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    Q_ASSERT(mSession->soap() != nullptr);

    const QByteArray passwordHash = password.toUtf8();

    KDSoapGenerated::TNS__User_auth userAuth;
    userAuth.setUser_name(user);
    userAuth.setPassword(QString::fromAscii(passwordHash));
    userAuth.setVersion(QLatin1String(".01"));

    KDSoapGenerated::TNS__Set_entry_result entry_result = mSession->soap()->login(userAuth, QLatin1String("FatCRM"));
    if (entry_result.error().number() == "0") {
        sessionId = entry_result.id();
        return KJob::NoError;
    } else {
        errorMessage = i18nc("@info:status", "Login for user %1 on %2 failed: %3", user, mSession->host(), mSession->soap()->lastError());
        int faultcode = mSession->soap()->lastErrorCode();
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
    if (mSession->sessionId().isEmpty() && mSession->soap() != nullptr) {
        KDSoapGenerated::TNS__Error_value errorValue = mSession->soap()->logout(mSession->sessionId());
        if (errorValue.number() != "0") {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "logout returned error" << errorValue.number() << errorValue.name() << errorValue.description();
        }
        if (!mSession->soap()->lastError().isEmpty()) {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "logout had fault" << mSession->soap()->lastError();
        }
    }
    mSession->forgetSession();
}

int SugarSoapProtocol::getEntriesCount(const ListEntriesScope &scope, Module moduleName, const QString &query,
                                       int &entriesCount, QString &errorMessage)
{
    KDSoapGenerated::TNS__Get_entries_count_result entry_result =
            mSession->soap()->get_entries_count(mSession->sessionId(), moduleToName(moduleName), query, scope.deleted());
    if (entry_result.error().number() == "0") {
        entriesCount = entry_result.result_count();
        return KJob::NoError;
    } else if (entry_result.error().number() == "10"){
        errorMessage = entry_result.error().description();
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "getEntriesCount returned error" << entry_result.error().number() << entry_result.error().name() << entry_result.error().description();
        return SugarJob::CouldNotConnectError;
    } else {
        errorMessage = entry_result.error().description();
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "getEntriesCount returned error" << entry_result.error().number() << entry_result.error().name() << entry_result.error().description()
                                                           << " Soap error:" << mSession->soap()->lastError();;
        return SugarJob::SoapError;
    }
}

int SugarSoapProtocol::listEntries(const ListEntriesScope &scope, Module moduleName, const QString &query,
                                   const QString &orderBy, const QStringList &selectedFields, EntriesListResult &entriesListResult,
                                    QString &errorMessage)
{
    const int offset = scope.offset();
    const int maxResults = 100;
    const int fetchDeleted = scope.deleted();

    KDSoapGenerated::TNS__Select_fields Fields;
    Fields.setItems(selectedFields);

    KDSoapGenerated::Get_entry_listJob *job = new KDSoapGenerated::Get_entry_listJob(mSession->soap());
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

    const KDSoapGenerated::TNS__Get_entry_list_result entry_result = job->return_();

    entriesListResult.entryList = entry_result.entry_list();
    entriesListResult.fieldList = entry_result.field_list();
    entriesListResult.nextOffset = entry_result.next_offset();
    entriesListResult.resultCount = entry_result.result_count();

    if (entry_result.error().number() == "0") {
        return KJob::NoError;
    } else if (entry_result.error().number() == "10"){
        errorMessage = entry_result.error().description();
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "listEntries returned error" << entry_result.error().number() << entry_result.error().name() << entry_result.error().description();
        return SugarJob::CouldNotConnectError;
    } else {
        errorMessage = entry_result.error().description();
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "listEntries returned error" << entry_result.error().number() << entry_result.error().name() << entry_result.error().description()
                                                           << " Soap error:" << mSession->soap()->lastError();
        return SugarJob::SoapError;
    }
}

int SugarSoapProtocol::setEntry(Module moduleName, const KDSoapGenerated::TNS__Name_value_list& name_value_list, QString &id, QString &errorMessage)
{
    KDSoapGenerated::TNS__Set_entry_result result = mSession->soap()->set_entry(mSession->sessionId(), moduleToName(moduleName), name_value_list);
    if (result.error().number() == "0") {
        id = result.id();
        return KJob::NoError;
    } else if (result.error().number() == "10"){
        errorMessage = result.error().description();
        return SugarJob::CouldNotConnectError;
    } else {
        errorMessage = result.error().description();
        return SugarJob::SoapError;
    }
}

int SugarSoapProtocol::getEntry(Module moduleName, const QString &remoteId, const QStringList &selectedFields, KDSoapGenerated::TNS__Entry_value &entryValue, QString &errorMessage)
{
    KDSoapGenerated::TNS__Select_fields fields;
    fields.setItems(selectedFields);
    KDSoapGenerated::TNS__Get_entry_result result = mSession->soap()->get_entry(mSession->sessionId(), moduleToName(moduleName), remoteId, fields);
    QString error = result.error().number();
    if (error == "0") {
        entryValue = result.entry_list().items().at(0);
        return KJob::NoError;
    } else if (error == "10") {
        errorMessage = mSession->soap()->lastError();
        return SugarJob::CouldNotConnectError;
    } else {
        errorMessage = result.error().description();
        return SugarJob::SoapError;
    }
}

int SugarSoapProtocol::listModules(KDSoapGenerated::TNS__Select_fields &selectFields, QString &errorMessage)
{
    KDSoapGenerated::TNS__Module_list result = mSession->soap()->get_available_modules(mSession->sessionId());
    QString error = result.error().number();
    if (error == "0") {
        selectFields = result.modules();
        return KJob::NoError;
    } else if (error == "10") {
        errorMessage = mSession->soap()->lastError();
        return SugarJob::CouldNotConnectError;
    } else {
        errorMessage = result.error().description();
        return SugarJob::SoapError;
    }
}
