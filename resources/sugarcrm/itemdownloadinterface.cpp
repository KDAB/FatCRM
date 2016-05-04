/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "itemdownloadinterface.h"

#include "sugarsession.h"
#include "sugarsoap.h"
#include "sugarcrmresource.h"

#include <KCodecs>
#include <KTempDir>

#include <QFile>

ItemDownloadInterface::ItemDownloadInterface(SugarCRMResource *resource)
    : QObject(resource), mResource(resource)
{
}

QString ItemDownloadInterface::downloadDocumentRevision(const QString &documentRevisionId) const
{
    SugarSession *session = mResource->mSession;
    KDSoapGenerated::Sugarsoap *soap = session->soap();
    const QString sessionId = session->sessionId();
    if (sessionId.isEmpty()) {
        qWarning() << "No session! Need to login first.";
    }

    const KDSoapGenerated::TNS__Return_document_revision result = soap->get_document_revision(sessionId, documentRevisionId);
    const KDSoapGenerated::TNS__Document_revision revision = result.document_revision();

    KTempDir tempDir;
    tempDir.setAutoRemove(false);

    const QString fullPath(tempDir.name() + revision.filename());
    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly))
        return QString();

    file.write(KCodecs::base64Decode(revision.file().toLatin1()));

    return fullPath;
}

#include "itemdownloadinterface.moc"
