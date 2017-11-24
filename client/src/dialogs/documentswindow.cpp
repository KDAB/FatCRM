/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "kdcrmdata/enumdefinitionattribute.h"
#include "kdcrmutils.h"
#include "linkeditemsrepository.h"
#include "sugarresourceitemtransfer.h"

#include <AkonadiCore/ItemDeleteJob>
#include <AkonadiCore/ItemModifyJob>

#include <KLocalizedString>
#include <KRun>

#include <QComboBox>
#include <QFileDialog>
#include <QLabel>
#include <QMimeType>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QScrollBar>

DocumentWidget::DocumentWidget(EnumDefinitions *definitions, QWidget *parent)
    : QWidget(parent),
      mEnumDefinitions(definitions)
{
    mNameLabel = new QLabel(this);
    mStatusBox = new QComboBox(this);
    mDeleteButton = new QPushButton(this);
    mDescriptionEdit = new QPlainTextEdit(this);
    QFrame *horizontalLine = new QFrame(this);

    mNameLabel->setOpenExternalLinks(false);

    const int enumIndex = mEnumDefinitions->indexOf("status_id");
    if (enumIndex != -1) {
        const EnumDefinitions::Enum &def = mEnumDefinitions->at(enumIndex);
        for (EnumDefinitions::Enum::Vector::const_iterator it = def.mEnumValues.constBegin();
             it != def.mEnumValues.constEnd(); ++it) {
            mStatusBox->addItem(it->value, it->key);
        }
    }

    mDeleteButton->setText(i18n("Delete"));
    horizontalLine->setFrameShape(QFrame::HLine);

    connect(mNameLabel, SIGNAL(linkActivated(QString)), SIGNAL(urlClicked(QString)));
    connect(mDeleteButton, SIGNAL(clicked()), SIGNAL(deleteDocument()));

    QVBoxLayout *topLayout = new QVBoxLayout(this);

    QHBoxLayout *firstRow = new QHBoxLayout;
    firstRow->addWidget(mNameLabel);
    firstRow->addWidget(mStatusBox);
    firstRow->addStretch();
    firstRow->addWidget(mDeleteButton);

    topLayout->addLayout(firstRow);
    topLayout->addWidget(mDescriptionEdit);
    topLayout->addWidget(horizontalLine);
}

void DocumentWidget::setDocument(const SugarDocument &document)
{
    mDocument = document;

    mNameLabel->setText(QString::fromLatin1("<a href=\"document:///%1\">%2</a>").arg(mDocument.documentRevisionId(), Qt::escape(mDocument.documentName())));
    mStatusBox->setCurrentIndex(mStatusBox->findText(mDocument.statusId()));
    mDescriptionEdit->setPlainText(mDocument.description());
}

SugarDocument DocumentWidget::document() const
{
    return mDocument;
}

SugarDocument DocumentWidget::modifiedDocument() const
{
    SugarDocument document(mDocument);

    document.setStatusId(mStatusBox->currentText());
    document.setDescription(mDescriptionEdit->toPlainText());

    return document;
}

void DocumentWidget::setNewDocumentFilePath(const QString &filePath)
{
    mNewDocumentFilePath = filePath;
}

QString DocumentWidget::newDocumentFilePath() const
{
    return mNewDocumentFilePath;
}

bool DocumentWidget::isModified() const
{
    if (mDocument.id().startsWith(QLatin1String("__temp")))
        return true;

    if (mStatusBox->currentText() != mDocument.statusId())
        return true;

    if (mDescriptionEdit->toPlainText() != mDocument.description())
        return true;

    return false;
}


DocumentsWindow::DocumentsWindow(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::DocumentsWindow),
      mLinkedItemsRepository(nullptr),
      mIsNotModifiedOverride(false),
      mLinkedItemType(Account),
      mPendingJobCount(0)
{
    ui->setupUi(this);
    ClientSettings::self()->restoreWindowSize("DocumentsWindow", this);

    connect(ui->attachButton, SIGNAL(clicked()), SLOT(attachDocument()));
}

DocumentsWindow::~DocumentsWindow()
{
    ClientSettings::self()->saveWindowSize("DocumentsWindow", this);
    delete ui;
}

void DocumentsWindow::setResourceIdentifier(const QString &identifier)
{
    mResourceIdentifier = identifier;
}

void DocumentsWindow::setLinkedItemsRepository(LinkedItemsRepository *repository)
{
    mLinkedItemsRepository = repository;

    EnumDefinitionAttribute *enumsAttr = mLinkedItemsRepository->documentsCollection().attribute<EnumDefinitionAttribute>();
    if (enumsAttr)
        mEnumDefinitions = EnumDefinitions::fromString(enumsAttr->value());
}

void DocumentsWindow::loadDocumentsFor(const QString &id, LinkedItemType itemType)
{
    Q_ASSERT(mLinkedItemsRepository);

    mLinkedItemId = id;
    mLinkedItemType = itemType;

    QVector<SugarDocument> documents;
    switch (itemType) {
    case Account:
        documents = mLinkedItemsRepository->documentsForAccount(id);
        break;
    case Opportunity:
        documents = mLinkedItemsRepository->documentsForOpportunity(id);
        break;
    }

    foreach (const SugarDocument &document, documents) {
        addDocument(document);
    }
}

void DocumentsWindow::closeEvent(QCloseEvent *event)
{
    if (isModified()) {
        QMessageBox msgBox(this);
        msgBox.setText(i18n("The list of attached documents has been modified."));
        msgBox.setInformativeText(i18n("Do you want to save your changes?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);

        switch (msgBox.exec()) {
        case QMessageBox::Save:
            saveChanges();
            event->ignore(); // postpone closing until async save operation is done
            return;
        case QMessageBox::Discard:
            break;
        default:
            event->ignore();
            return;
        }
    }

    event->accept();
}

void DocumentsWindow::on_buttonBox_accepted() // "Save"
{
    if (isModified())
        saveChanges();
    else
        QWidget::close();
}

void DocumentsWindow::on_buttonBox_rejected()
{
    mIsNotModifiedOverride = true;
    QWidget::close();
}

void DocumentsWindow::urlClicked(const QString &url)
{
    const QString documentRevisionId = QUrl(url).path().mid(1); // strip leading '/' from path

    ComKdabSugarCRMItemTransferInterface transferInterface(QLatin1String("org.freedesktop.Akonadi.Resource.") + mResourceIdentifier, QLatin1String("/ItemTransfer"), QDBusConnection::sessionBus());

    QDBusPendingReply<QString> reply = transferInterface.downloadDocumentRevision(documentRevisionId);
    reply.waitForFinished();

    if (reply.isValid()) {

        const QString filePath = reply.value();
        if (!filePath.isEmpty()) {
            const QUrl localFile = QUrl::fromLocalFile(filePath);

            QMimeDatabase db;
            const auto mimeType = db.mimeTypeForUrl(localFile);

            KRun::runUrl(localFile, mimeType.isValid() ? mimeType.name() : QString(), this, true, false);

            return;
        }
    }

    QMessageBox::warning(this, i18n("Unable to download Document"), i18n("Could not download the document, make sure the resource is online."));
}

void DocumentsWindow::deleteDocument()
{
    DocumentWidget *widget = qobject_cast<DocumentWidget*>(sender());
    if (!widget)
        return;

    const QString documentId = widget->document().id();

    const int answer = QMessageBox::question(this, i18n("Delete Document?"), i18n("Do you really want to delete the document %1?", widget->document().documentName()),
                                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::No)
        return;

    // Remove the document from the view (mDocumentWidgets). Keep in mDocuments so that saveChanges() sees it and triggers deletion.
    mDocumentWidgets.remove(mDocumentWidgets.indexOf(widget));

    widget->deleteLater();
}

void DocumentsWindow::attachDocument()
{
    const QString filePath = QFileDialog::getOpenFileName(this, i18n("Select document file"));
    if (filePath.isEmpty())
        return;

    const QFileInfo fileInfo(filePath);

    static int tempDocumentCounter = 0;
    ++tempDocumentCounter;

    SugarDocument document;
    document.setId(QString::fromLatin1("__temp%1").arg(tempDocumentCounter));
    document.setDocumentName(fileInfo.fileName());

    DocumentWidget *widget = addDocument(document);
    widget->setNewDocumentFilePath(filePath);
}

void DocumentsWindow::slotJobResult(KJob *job)
{
    if (job->error())
        qWarning() << job->errorString();

    --mPendingJobCount;

    if (mPendingJobCount == 0) {
        mIsNotModifiedOverride = true;
        QWidget::close();
    }
}

DocumentWidget* DocumentsWindow::addDocument(const SugarDocument &document)
{
    mDocuments.append(document);

    DocumentWidget *widget = new DocumentWidget(&mEnumDefinitions);
    widget->setDocument(document);
    mDocumentWidgets.append(widget);

    connect(widget, SIGNAL(urlClicked(QString)), SLOT(urlClicked(QString)));
    connect(widget, SIGNAL(deleteDocument()), SLOT(deleteDocument()));

    if (ui->scrollAreaLayout->count() > 1) // remove the stretch item at the end
        ui->scrollAreaLayout->removeItem(ui->scrollAreaLayout->itemAt(ui->scrollAreaLayout->count() - 1));

    ui->scrollAreaLayout->addWidget(widget);
    ui->scrollAreaLayout->addStretch();

    return widget;
}

bool DocumentsWindow::isModified() const
{
    if (mIsNotModifiedOverride)
        return false;

    if (mDocuments.count() != mDocumentWidgets.count())
        return true;

    foreach (DocumentWidget *widget, mDocumentWidgets) {
        if (widget->isModified())
            return true;
    }

    return false;
}

void DocumentsWindow::saveChanges()
{
    foreach (const SugarDocument &oldDocument, mDocuments) {
        const QString id = oldDocument.id();

        auto it = std::find_if(mDocumentWidgets.constBegin(), mDocumentWidgets.constEnd(), [&id](DocumentWidget *widget) { return widget->document().id() == id; });
        if (it == mDocumentWidgets.end()) {
            const Akonadi::Item item = mLinkedItemsRepository->documentItem(id);
            if (item.isValid()) {
                Akonadi::ItemDeleteJob *job = new Akonadi::ItemDeleteJob(item, this);
                connect(job, SIGNAL(result(KJob*)), this, SLOT(slotJobResult(KJob*)));

                ++mPendingJobCount;
            }
        }
    }

    foreach (DocumentWidget *widget, mDocumentWidgets) {
        if (widget->isModified()) {
            const SugarDocument document = widget->document();
            const SugarDocument modifiedDocument = widget->modifiedDocument();
            if (document.id().startsWith(QLatin1String("__temp"))) {
                ComKdabSugarCRMItemTransferInterface transferInterface(QLatin1String("org.freedesktop.Akonadi.Resource.") + mResourceIdentifier, QLatin1String("/ItemTransfer"), QDBusConnection::sessionBus());

                // create new document instance
                QDBusPendingReply<QString> uploadReply = transferInterface.uploadDocument(modifiedDocument.documentName(), modifiedDocument.statusId(),
                                                                                          modifiedDocument.description(), widget->newDocumentFilePath());
                uploadReply.waitForFinished();

                const QString documentId = uploadReply.value();
                if (documentId.isEmpty()) {
                    qWarning() << "Unable to upload new document";
                    continue;
                }

                // link created instance to currently loaded Account/Opportunity
                const QString linkedItemModuleName = (mLinkedItemType == Account ? "Accounts" : "Opportunities");

                QDBusPendingReply<bool> linkReply = transferInterface.linkItem(documentId, "Documents", mLinkedItemId, linkedItemModuleName);
                linkReply.waitForFinished();
                if (linkReply.isError()) {
                    qWarning() << "Unable to link document" << documentId << ":" << linkReply.error().message();
                    continue;
                }

                const bool ok = linkReply.value();
                if (!ok) {
                    qWarning() << "Unable to link document" << documentId << ", see resource logs for details";
                    continue;
                }

            } else {
                Akonadi::Item item = mLinkedItemsRepository->documentItem(document.id());
                if (item.isValid()) {
                    item.setPayload(modifiedDocument);

                    Akonadi::ItemModifyJob *job = new Akonadi::ItemModifyJob(item, this);
                    connect(job, SIGNAL(result(KJob*)), this, SLOT(slotJobResult(KJob*)));

                    ++mPendingJobCount;
                }
            }
        }
    }

    if (mPendingJobCount == 0) {
        mIsNotModifiedOverride = true;
        QWidget::close();
    }
}
