/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

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

#ifndef NOTESREPOSITORY_H
#define NOTESREPOSITORY_H

#include <QObject>

#include <akonadi/item.h>
#include <akonadi/collection.h>
#include "kdcrmdata/sugarnote.h"
#include "kdcrmdata/sugaremail.h"

class NotesRepository : public QObject
{
    Q_OBJECT
public:
    explicit NotesRepository(QObject *parent = 0);

    void setNotesCollection(const Akonadi::Collection &collection);
    void setEmailsCollection(const Akonadi::Collection &collection);

    void loadNotes();
    void loadEmails();

    QVector<SugarNote> notesForOpportunity(const QString &id) const;
    QVector<SugarEmail> emailsForOpportunity(const QString &id) const;

signals:
    void notesLoaded(int count);
    void emailsLoaded(int count);

private Q_SLOTS:
    void slotNotesReceived(const Akonadi::Item::List &items);
    void slotEmailsReceived(const Akonadi::Item::List &items);

private:
    Akonadi::Collection mNotesCollection;
    typedef QHash<QString, QVector<SugarNote> > NotesHash;
    NotesHash mNotesHash;
    int mNotesLoaded;

    Akonadi::Collection mEmailsCollection;
    typedef QHash<QString, QVector<SugarEmail> > EmailsHash;
    EmailsHash mEmailsHash;
    int mEmailsLoaded;
};

#endif // NOTESREPOSITORY_H
