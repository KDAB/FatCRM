/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef LINKEDITEMSREPOSITORY_H
#define LINKEDITEMSREPOSITORY_H

#include "kdcrmdata/sugarnote.h"
#include "kdcrmdata/sugaremail.h"

#include <Akonadi/Item>
#include <Akonadi/Collection>

#include <QObject>

namespace Akonadi
{
    class Monitor;
    class ItemFetchScope;
}

class LinkedItemsRepository : public QObject
{
    Q_OBJECT
public:
    explicit LinkedItemsRepository(QObject *parent = 0);

    void clear();

    void setNotesCollection(const Akonadi::Collection &collection);
    void setEmailsCollection(const Akonadi::Collection &collection);

    void loadNotes();
    void loadEmails();
    void monitorChanges();

    QVector<SugarNote> notesForOpportunity(const QString &id) const;
    QVector<SugarEmail> emailsForOpportunity(const QString &id) const;

signals:
    void notesLoaded(int count);
    void emailsLoaded(int count);

private Q_SLOTS:
    void slotNotesReceived(const Akonadi::Item::List &items);
    void slotItemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection);
    void slotItemRemoved(const Akonadi::Item &item);
    void slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray>& partIdentifiers);

    void slotEmailsReceived(const Akonadi::Item::List &items);

private:
    void storeNote(const Akonadi::Item &item);
    void removeNote(const QString &id);
    void storeEmail(const Akonadi::Item &item);
    void removeEmail(const QString &id);
    void configureItemFetchScope(Akonadi::ItemFetchScope &scope);
    void updateItem(const Akonadi::Item &item, const Akonadi::Collection &collection);

    Akonadi::Collection mNotesCollection;
    Akonadi::Monitor *mMonitor;
    typedef QHash<QString, QVector<SugarNote> > NotesHash;
    NotesHash mNotesHash;
    QHash<QString, QString> mNotesParentIdHash; // note id -> opportunity id (to handle removals)
    int mNotesLoaded;

    Akonadi::Collection mEmailsCollection;
    typedef QHash<QString, QVector<SugarEmail> > EmailsHash;
    EmailsHash mEmailsHash;
    QHash<QString, QString> mEmailsParentIdHash; // email id -> opportunity id (to handle removals)
    int mEmailsLoaded;

};

#endif // LINKEDITEMSREPOSITORY_H
