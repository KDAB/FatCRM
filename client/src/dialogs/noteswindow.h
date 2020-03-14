/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Robin Jonsson <robin.jonsson@kdab.com>

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

#ifndef NOTESWINDOW_H
#define NOTESWINDOW_H

#include <QWidget>
#include <QDateTime>
#include "enums.h"

namespace Ui {
class NotesWindow;
}
class SugarEmail;
class SugarNote;
class KJob;
class LinkedItemsRepository;

// internal. Generic representation for notes and emails, to sort them by date.
struct NoteText
{
    NoteText() {}
    NoteText(const QDateTime &dt, const QString &htmlHeader, const QString &text, bool isHtml = false)
        : m_date(dt), m_htmlHeader(htmlHeader), m_text(text), m_isHtml(isHtml) {}

    bool operator<(const NoteText &other) const {
        // Most recent at the top
        return m_date > other.m_date;
    }

    QString text() const { return m_text; }
    QString htmlHeader() const { return m_htmlHeader; }
    bool isHtml() const { return m_isHtml; }

private:
    QDateTime m_date;
    QString m_htmlHeader;
    QString m_text;
    bool m_isHtml = false;
};

class NotesWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NotesWindow(QWidget *parent = nullptr);
    ~NotesWindow() override;

    void setResourceIdentifier(const QString &resourceIdentifier);
    void setLinkedItemsRepository(LinkedItemsRepository *repository);

    void setLinkedTo(const QString &id, DetailsType itemType);

    void addNote(const SugarNote &note);
    void addEmail(const SugarEmail &email);

    void setVisible(bool visible) override;


protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();

    void slotJobResult(KJob *job);

private:
    bool isModified() const;
    void saveChanges();

    QVector<NoteText> m_notes;
    Ui::NotesWindow *ui;
    QString mResourceIdentifier;
    LinkedItemsRepository *mLinkedItemsRepository = nullptr;
    bool mIsNotModifiedOverride = false;

    QString mLinkedItemId;
    DetailsType mLinkedItemType;
};

#endif // NotesWindow_H
