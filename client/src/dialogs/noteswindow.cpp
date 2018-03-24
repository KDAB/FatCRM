/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Robin Jonsson <robin.jonsson@kdab.com>, David Faure <david.faure@kdab.com>

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

#include "noteswindow.h"
#include "ui_noteswindow.h"
#include "kdcrmutils.h"
#include "clientsettings.h"
#include "linkeditemsrepository.h"

#include "kdcrmdata/sugarnote.h"
#include "kdcrmdata/sugaremail.h"

#include <AkonadiCore/Item>
#include <AkonadiCore/ItemCreateJob>

#include <KLocalizedString>

#include <QCloseEvent>
#include <QMessageBox>
#include <QScrollBar>

NotesWindow::NotesWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NotesWindow)
{
    ui->setupUi(this);
    ui->textEdit->enableFindReplace(true);
    ui->textEdit->setAcceptRichText(true);
    ui->textEdit->setReadOnly(true);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    ui->newNoteDescription->setPlaceholderText(i18n("Type here for the detailed description of the new note..."));
#endif
    ClientSettings::self()->restoreWindowSize("NotesWindow", this);
}

NotesWindow::~NotesWindow()
{
    ClientSettings::self()->saveWindowSize(QStringLiteral("NotesWindow"), this);
    delete ui;
}

void NotesWindow::setLinkedItemsRepository(LinkedItemsRepository *repository)
{
    mLinkedItemsRepository = repository;
}

void NotesWindow::setLinkedTo(const QString &id, DetailsType itemType)
{
    mLinkedItemId = id;
    mLinkedItemType = itemType;
}

void NotesWindow::addNote(const SugarNote &note)
{
    const QDateTime modified(note.dateModified());
    QString htmlHeader;
    htmlHeader += (QStringLiteral("<html><h1>Note by %1, last modified %2:</h1>\n").arg(note.createdByName()).arg(KDCRMUtils::formatDateTime(modified)));
    htmlHeader += "<h2>" + note.name() + "</h2>\n"; // called "Subject" in the web gui
    QString text;
    if (!note.description().isEmpty()) {
        text += note.description() + '\n';
    }
    text += '\n';
    m_notes.append(NoteText(modified, htmlHeader, text));
}

void NotesWindow::addEmail(const SugarEmail &email)
{
    const QString toList = email.toAddrNames();
    const QDateTime dateSent = KDCRMUtils::dateTimeFromString(email.dateSent());
    QString htmlHeader;
    htmlHeader += QStringLiteral("<html><h1>Mail from %1. Date: %2</h1>\n").arg(email.fromAddrName().trimmed(), KDCRMUtils::formatDateTime(dateSent));
    htmlHeader += QStringLiteral("<h2>Subject: %1</h2>\n").arg(email.name());
    htmlHeader += QStringLiteral("<p>To: %1</p>\n").arg(toList);

    const bool useHtml = email.description().isEmpty();
    const QString text = (useHtml ? email.descriptionHtml() : email.description()) + '\n';
    m_notes.append(NoteText(dateSent, htmlHeader, text, useHtml));
}

void NotesWindow::setVisible(bool visible)
{
    if (ui->textEdit->document()->isEmpty()) {
        qSort(m_notes);
        QTextCursor cursor = ui->textEdit->textCursor();
        foreach (const NoteText &note, m_notes) {
            //cursor.insertText(s_separator);
            cursor.insertHtml(QStringLiteral("<hr>"));
            cursor.insertBlock();
            cursor.setBlockFormat(QTextBlockFormat());
            cursor.insertHtml(note.htmlHeader());
            cursor.setBlockFormat(QTextBlockFormat());
            cursor.insertBlock();
            cursor.insertBlock();
            if (note.isHtml())
                cursor.insertHtml(note.text());
            else
                cursor.insertText(note.text());
        }
    }
    QWidget::setVisible(visible);
    ui->textEdit->verticalScrollBar()->setValue(0);
}

void NotesWindow::closeEvent(QCloseEvent *event)
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

void NotesWindow::on_buttonBox_rejected()
{
    mIsNotModifiedOverride = true;
    QWidget::close();
}

void NotesWindow::on_buttonBox_accepted() // "Save"
{
    if (isModified())
        saveChanges();
    else
        QWidget::close();
}

bool NotesWindow::isModified() const
{
    if (mIsNotModifiedOverride)
        return false;
    return !ui->newNoteSubject->text().isEmpty() ||
           !ui->newNoteDescription->document()->isEmpty();
}

void NotesWindow::saveChanges()
{
    SugarNote newNote;
    newNote.setName(ui->newNoteSubject->text());
    newNote.setDescription(ui->newNoteDescription->toPlainText());
    // Unlike documents, a note is associated to only one item.
    // So it's very easy, we don't need to call ComKdabSugarCRMItemTransferInterface::linkItem().
    newNote.setParentId(mLinkedItemId);
    const QString linkedItemModuleName = typeToString(mLinkedItemType);
    newNote.setParentType(linkedItemModuleName);
    Akonadi::Item item;
    item.setMimeType(SugarNote::mimeType());
    item.setPayload(newNote);
    Akonadi::ItemCreateJob *createJob = new Akonadi::ItemCreateJob(item, mLinkedItemsRepository->notesCollection(), this);
    connect(createJob, SIGNAL(result(KJob*)), this, SLOT(slotJobResult(KJob*)));
}

void NotesWindow::setResourceIdentifier(const QString &resourceIdentifier)
{
    mResourceIdentifier = resourceIdentifier;
}

void NotesWindow::slotJobResult(KJob *job)
{
    if (job->error())
        qWarning() << job->errorString();

    mIsNotModifiedOverride = true;
    QWidget::close();
}
