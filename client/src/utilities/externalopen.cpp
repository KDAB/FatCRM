/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Kevin Funk <kevin.funk@kdab.com>

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

#include "externalopen.h"

#include "sugarresourceitemtransfer.h"
#include "fatcrm_client_debug.h"

#include <AkonadiCore/ServerManager>

#include <KLocalizedString>
#include <KRun>

#include <QDebug>
#include <QMessageBox>
#include <QMimeDatabase>

bool ExternalOpen::openSugarDocument(const QString &documentRevisionId, const QString &resourceIdentifier, QWidget *window)
{
    qCDebug(FATCRM_CLIENT_LOG) << "Opening document:" << documentRevisionId << "via resource:" << resourceIdentifier;

    const auto service = Akonadi::ServerManager::agentServiceName(Akonadi::ServerManager::Resource, resourceIdentifier);

    ComKdabSugarCRMItemTransferInterface transferInterface(service, QLatin1String("/ItemTransfer"), QDBusConnection::sessionBus());
    QDBusPendingReply<QString> reply = transferInterface.downloadDocumentRevision(documentRevisionId);
    reply.waitForFinished();

    if (!reply.isValid()) {
        QMessageBox::warning(window, i18n("Unable to download Document"), i18n("Could not download the document, make sure the resource is online."));
        return false;
    }

    const QString filePath = reply.value();
    if (filePath.isEmpty()) {
        QMessageBox::warning(window, i18n("Unable to download Document"), i18n("Could not download the document (unknown file path), make sure the resource is online."));
        return false;
    }

    const QUrl localFile = QUrl::fromLocalFile(filePath);

    QMimeDatabase db;
    const auto mimeType = db.mimeTypeForUrl(localFile);

    return KRun::runUrl(localFile, mimeType.isValid() ? mimeType.name() : QString(),
                        window, KRun::RunFlags(KRun::DeleteTemporaryFiles));
}
