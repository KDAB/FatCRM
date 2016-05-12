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

#include "documentswindow.h"
#include "ui_documentswindow.h"

#include "clientsettings.h"
#include "kdcrmdata/sugardocument.h"
#include "kdcrmutils.h"
#include "sugarresourceitemtransfer.h"

#include <KMimeType>
#include <KRun>

#include <QMessageBox>
#include <QScrollBar>

DocumentsWindow::DocumentsWindow(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::DocumentsWindow)
{
    ui->setupUi(this);
    ui->textBrowser->setAcceptRichText(true);
    ui->textBrowser->setOpenLinks(false);
    ClientSettings::self()->restoreWindowSize("DocumentsWindow", this);

    connect(ui->textBrowser, SIGNAL(anchorClicked(QUrl)), SLOT(urlClicked(QUrl)));
}

DocumentsWindow::~DocumentsWindow()
{
    ClientSettings::self()->saveWindowSize("DocumentsWindow", this);
    delete ui;
}

void DocumentsWindow::addDocument(const SugarDocument &document)
{
    m_documents.append(document);
}

void DocumentsWindow::setVisible(bool visible)
{
    if (ui->textBrowser->document()->isEmpty()) {
        QTextCursor cursor = ui->textBrowser->textCursor();
        foreach (const SugarDocument &document, m_documents) {
            cursor.insertHtml("<hr>");
            cursor.insertBlock();
            cursor.setBlockFormat(QTextBlockFormat());
            cursor.insertHtml(QString::fromLatin1("<p><a href=\"document:///%1\">%2</a> (%3)<br/>%4</p>").arg(document.documentRevisionId(), Qt::escape(document.documentName()), Qt::escape(document.statusId()),Qt::escape(document.description())));
            cursor.setBlockFormat(QTextBlockFormat());
            cursor.insertBlock();
            cursor.insertBlock();
        }
    }

    QWidget::setVisible(visible);
    ui->textBrowser->verticalScrollBar()->setValue(0);
}

void DocumentsWindow::setResourceIdentifier(const QString &identifier)
{
    mResourceIdentifier = identifier;
}

void DocumentsWindow::on_buttonBox_rejected()
{
    QWidget::close();
}

void DocumentsWindow::urlClicked(const QUrl &url)
{
    const QString documentRevisionId = url.path().mid(1); // strip leading '/' from path

    ComKdabSugarCRMItemTransferInterface transferInterface(QLatin1String("org.freedesktop.Akonadi.Resource.") + mResourceIdentifier, QLatin1String("/ItemTransfer"), QDBusConnection::sessionBus());

    QDBusPendingReply<QString> reply = transferInterface.downloadDocumentRevision(documentRevisionId);
    reply.waitForFinished();

    if (reply.isValid()) {

        const QString filePath = reply.value();
        if (!filePath.isEmpty()) {
            const QUrl localFile = QUrl::fromLocalFile(filePath);
            const KMimeType::Ptr mimeType = KMimeType::findByUrl(localFile);

            KRun::runUrl(localFile, mimeType ? mimeType->name() : QString(), this, true, false);

            return;
        }
    }

    QMessageBox::warning(this, i18n("Unable to download Document"), i18n("Could not download the document, make sure the resource is online."));
}
