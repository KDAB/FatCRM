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

#include "notesrepository.h"
#include <akonadi/collection.h>
#include <akonadi/collectionstatistics.h>
#include <akonadi/itemfetchjob.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/monitor.h>

NotesRepository::NotesRepository(QObject *parent) :
    QObject(parent),
    mMonitor(0),
    mNotesLoaded(0),
    mEmailsLoaded(0)
{
}

void NotesRepository::setNotesCollection(const Akonadi::Collection &collection)
{
    mNotesCollection = collection;
}

void NotesRepository::loadNotes()
{
    //kDebug() << "Loading" << mNotesCollection.statistics().count() << "notes";

    // load notes
    Akonadi::ItemFetchJob *job = new Akonadi::ItemFetchJob(mNotesCollection, this);
    configureItemFetchScope(job->fetchScope());
    connect(job, SIGNAL(itemsReceived(Akonadi::Item::List)),
            this, SLOT(slotNotesReceived(Akonadi::Item::List)));
}

QVector<SugarNote> NotesRepository::notesForOpportunity(const QString &id) const
{
    return mNotesHash.value(id);
}

void NotesRepository::slotNotesReceived(const Akonadi::Item::List &items)
{
    mNotesLoaded += items.count();
    foreach(const Akonadi::Item &item, items) {
        storeNote(item);
    }
    //kDebug() << "loaded" << mNotesLoaded << "notes; now hash has" << mNotesHash.count() << "entries";
    if (mNotesLoaded == mNotesCollection.statistics().count())
        emit notesLoaded(mNotesLoaded);
}

void NotesRepository::storeNote(const Akonadi::Item &item)
{
    //kDebug() << item.id() << item.mimeType() << ;
    if (item.hasPayload<SugarNote>()) {
        SugarNote note = item.payload<SugarNote>();
        if (note.parentType() == QLatin1String("Opportunities")) {
            mNotesHash[note.parentId()].append(note);
        } else {
            // We also get notes for Accounts and Emails.
            // (well, no longer, we filter this out in the resource)
            //kDebug() << "ignoring notes for" << note.parentType();
        }
    }
}

///

void NotesRepository::setEmailsCollection(const Akonadi::Collection &collection)
{
    mEmailsCollection = collection;
}

void NotesRepository::loadEmails()
{
    //kDebug() << "Loading" << mEmailsCollection.statistics().count() << "emails";

    // load emails
    Akonadi::ItemFetchJob *job = new Akonadi::ItemFetchJob(mEmailsCollection, this);
    configureItemFetchScope(job->fetchScope());
    connect(job, SIGNAL(itemsReceived(Akonadi::Item::List)),
            this, SLOT(slotEmailsReceived(Akonadi::Item::List)));
}

void NotesRepository::monitorChanges()
{
    mMonitor = new Akonadi::Monitor(this);
    mMonitor->setCollectionMonitored(mNotesCollection);
    mMonitor->setCollectionMonitored(mEmailsCollection);
    configureItemFetchScope(mMonitor->itemFetchScope());
    connect(mMonitor, SIGNAL(itemAdded(Akonadi::Item,Akonadi::Collection)),
            this, SLOT(slotItemAdded(Akonadi::Item,Akonadi::Collection)));
}

QVector<SugarEmail> NotesRepository::emailsForOpportunity(const QString &id) const
{
    return mEmailsHash.value(id);
}

void NotesRepository::slotEmailsReceived(const Akonadi::Item::List &items)
{
    mEmailsLoaded += items.count();
    foreach(const Akonadi::Item &item, items) {
        storeEmail(item);
    }
    //kDebug() << "loaded" << mEmailsLoaded << "emails; now hash has" << mEmailsHash.count() << "entries";
    if (mEmailsLoaded == mEmailsCollection.statistics().count())
        emit emailsLoaded(mEmailsLoaded);
}

void NotesRepository::storeEmail(const Akonadi::Item &item)
{
    //kDebug() << item.id() << item.mimeType() << ;
    if (item.hasPayload<SugarEmail>()) {
        SugarEmail email = item.payload<SugarEmail>();
        if (email.parentType() == QLatin1String("Opportunities")) {
            mEmailsHash[email.parentId()].append(email);
        } else {
            // We also get emails for Accounts and Emails.
            // (well, no longer, we filter this out in the resource)
            //kDebug() << "ignoring emails for" << email.parentType();
        }
    }
}

void NotesRepository::configureItemFetchScope(Akonadi::ItemFetchScope &scope)
{
    scope.setFetchRemoteIdentification(false);
    scope.setIgnoreRetrievalErrors(true);
    scope.fetchFullPayload(true);
}

void NotesRepository::slotItemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection)
{
    if (collection == mNotesCollection) {
        //kDebug() << item.id() << item.mimeType();
        storeNote(item);
    } else if (collection == mEmailsCollection) {
        storeEmail(item);
    } else {
        kWarning() << "Unexpected collection" << collection << ", expected" << mNotesCollection.id();
        return;
    }
}
