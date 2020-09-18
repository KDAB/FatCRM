/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Tobias Koenig <tobias.koenig@kdab.com>

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

#include "itemtransferinterface.h"

#include "sugarcrmresource_debug.h"

#include "sugarsession.h"
#include "wsdl_sugar41.h"
#include "sugarcrmresource.h"

#include <KCodecs>
#include <QTemporaryDir>

#include <QFile>

ItemTransferInterface::ItemTransferInterface(QObject *parent)
    : QObject(parent)
{
}

void ItemTransferInterface::setSession(SugarSession *session)
{
    mSession = session;
}

QString ItemTransferInterface::downloadDocumentRevision(const QString &documentRevisionId) const
{
    KDSoapGenerated::Sugarsoap *soap = mSession->soap();
    const QString sessionId = mSession->sessionId();
    if (sessionId.isEmpty()) {
        qWarning() << "No session! Need to login first.";
    }

    const KDSoapGenerated::TNS__New_return_document_revision result = soap->get_document_revision(sessionId, documentRevisionId);
    const KDSoapGenerated::TNS__Document_revision revision = result.document_revision();

    QTemporaryDir tempDir;
    tempDir.setAutoRemove(false);

    const QString fullPath(tempDir.path() + QLatin1Char('/') + revision.filename());
    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly)) {
        tempDir.setAutoRemove(true);
        return QString();
    }

    file.write(KCodecs::base64Decode(revision.file().toLatin1()));

    return fullPath;
}

QString ItemTransferInterface::uploadDocument(const QString &documentName, const QString &statusId, const QString &description, const QString &localFilePath) const
{
    QFile file(localFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open file" << file.fileName() << "for reading";
        return QString();
    }

    KDSoapGenerated::Sugarsoap *soap = mSession->soap();
    const QString sessionId = mSession->sessionId();
    if (sessionId.isEmpty()) {
        qWarning() << "No session! Need to login first.";
    }

    // first create the document entry
    KDSoapGenerated::TNS__Name_value documentNameProperty;
    documentNameProperty.setName("document_name");
    documentNameProperty.setValue(documentName);

    KDSoapGenerated::TNS__Name_value statusIdProperty;
    statusIdProperty.setName("status_id");
    statusIdProperty.setValue(statusId);

    KDSoapGenerated::TNS__Name_value descriptionProperty;
    descriptionProperty.setName("description");
    descriptionProperty.setValue(description);

    KDSoapGenerated::TNS__Name_value_list documentProperties;
    documentProperties.setItems(QList<KDSoapGenerated::TNS__Name_value>() << documentNameProperty << statusIdProperty << descriptionProperty);

    KDSoapGenerated::TNS__New_set_entry_result result = soap->set_entry(sessionId, "Documents", documentProperties);
    if (soap->lastErrorCode() != 0) {
        qWarning() << "Unable to create document:" << soap->lastError();
        return QString();
    }

    const QString documentId = result.id();

    // then upload the first document revision
    KDSoapGenerated::TNS__Document_revision documentRevision;
    documentRevision.setId(documentId);
    documentRevision.setRevision("1");
    documentRevision.setFilename(documentName);
    documentRevision.setFile(QString::fromLatin1(KCodecs::base64Encode(file.readAll())));

    result = soap->set_document_revision(sessionId, documentRevision);

    if (!soap->lastError().isEmpty()) { // got a SOAP fault as reply
        qWarning() << "Unable to create document revision:" << soap->lastError();
        return QStringLiteral("ERROR:") + soap->lastError();
    }

    return documentId;
}

bool ItemTransferInterface::linkItem(const QString &sourceItemId, const QString &sourceModuleName,
                                     const QString &targetItemId, const QString &targetModuleName) const
{
    const QString sessionId = mSession->sessionId();
    if (sessionId.isEmpty()) {
        qWarning() << "No session! Need to login first.";
        return false;
    }

    QUrl url(mSession->host());
    url.setPath("/service/v4_1/soap.php");
    url.setQuery(QString());

    KDSoapGenerated::Sugarsoap soap;
    soap.setEndPoint(url.url());


    KDSoapGenerated::TNS__Select_fields relatedIds;
    relatedIds.setItems(QStringList() << targetItemId);

    const KDSoapGenerated::TNS__New_set_relationship_list_result result = soap.set_relationship(sessionId, sourceModuleName, sourceItemId, targetModuleName.toLower(), relatedIds, KDSoapGenerated::TNS__Name_value_list(), 0);

    if (!soap.lastError().isEmpty()) {
        qWarning() << "Unable to link items:" << soap.lastError();
        return false;
    }

    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Link Item result:" << result.created() << "created," << result.deleted() << "deleted," << result.failed() << "failed.";

    return true;
}
