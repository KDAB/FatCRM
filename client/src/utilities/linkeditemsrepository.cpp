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

#include "linkeditemsrepository.h"

#include <Akonadi/Collection>
#include <Akonadi/CollectionStatistics>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/Monitor>

#include <QStringList>

LinkedItemsRepository::LinkedItemsRepository(QObject *parent) :
    QObject(parent),
    mMonitor(0),
    mNotesLoaded(0),
    mEmailsLoaded(0),
    mDocumentsLoaded(0)
{
}

void LinkedItemsRepository::clear()
{
    mNotesLoaded = 0;
    mEmailsLoaded = 0;
    mDocumentsLoaded = 0;
    mNotesHash.clear();
    mEmailsHash.clear();
    mAccountDocumentsHash.clear();
    mOpportunityDocumentsHash.clear();
    delete mMonitor;
    mMonitor = 0;
}

void LinkedItemsRepository::setNotesCollection(const Akonadi::Collection &collection)
{
    mNotesCollection = collection;
}

void LinkedItemsRepository::loadNotes()
{
    //kDebug() << "Loading" << mNotesCollection.statistics().count() << "notes";

    // load notes
    Akonadi::ItemFetchJob *job = new Akonadi::ItemFetchJob(mNotesCollection, this);
    configureItemFetchScope(job->fetchScope());
    connect(job, SIGNAL(itemsReceived(Akonadi::Item::List)),
            this, SLOT(slotNotesReceived(Akonadi::Item::List)));
}

QVector<SugarNote> LinkedItemsRepository::notesForOpportunity(const QString &id) const
{
    return mNotesHash.value(id);
}

void LinkedItemsRepository::slotNotesReceived(const Akonadi::Item::List &items)
{
    mNotesLoaded += items.count();
    foreach(const Akonadi::Item &item, items) {
        storeNote(item);
    }
    //kDebug() << "loaded" << mNotesLoaded << "notes; now hash has" << mNotesHash.count() << "entries";
    if (mNotesLoaded == mNotesCollection.statistics().count())
        emit notesLoaded(mNotesLoaded);
}

void LinkedItemsRepository::storeNote(const Akonadi::Item &item)
{
    if (item.hasPayload<SugarNote>()) {
        SugarNote note = item.payload<SugarNote>();
        const QString id = note.id();
        Q_ASSERT(!id.isEmpty());
        removeNote(id); // handle change of opp
        if (note.parentType() == QLatin1String("Opportunities")) {
            const QString parentId = note.parentId();
            if (!parentId.isEmpty()) {
                mNotesHash[parentId].append(note);
                mNotesParentIdHash.insert(id, parentId);
            } else {
                mNotesParentIdHash.remove(id);
            }
        } else {
            // We also get notes for Accounts and Emails.
            // (well, no longer, we filter this out in the resource)
            //kDebug() << "ignoring notes for" << note.parentType();
        }
    }
}

void LinkedItemsRepository::removeNote(const QString &id)
{
    Q_ASSERT(!id.isEmpty());
    const QString oldParentId = mNotesParentIdHash.value(id);
    if (!oldParentId.isEmpty()) {
        kDebug() << "note" << id << "oldParentId" << oldParentId;
        // Note is no longer associated with this opportunity
        QVector<SugarNote> &notes = mNotesHash[oldParentId];
        auto it = std::find_if(notes.constBegin(), notes.constEnd(), [id](const SugarNote &n) { return n.id() == id; });
        if (it != notes.constEnd()) {
            const int idx = std::distance(notes.constBegin(), it);
            kDebug() << "Removing note at" << idx;
            notes.remove(idx);
        }
    }
}

///

void LinkedItemsRepository::setEmailsCollection(const Akonadi::Collection &collection)
{
    mEmailsCollection = collection;
}

void LinkedItemsRepository::loadEmails()
{
    //kDebug() << "Loading" << mEmailsCollection.statistics().count() << "emails";

    // load emails
    Akonadi::ItemFetchJob *job = new Akonadi::ItemFetchJob(mEmailsCollection, this);
    configureItemFetchScope(job->fetchScope());
    connect(job, SIGNAL(itemsReceived(Akonadi::Item::List)),
            this, SLOT(slotEmailsReceived(Akonadi::Item::List)));
}

void LinkedItemsRepository::monitorChanges()
{
    mMonitor = new Akonadi::Monitor(this);
    mMonitor->setCollectionMonitored(mNotesCollection);
    mMonitor->setCollectionMonitored(mEmailsCollection);
    mMonitor->setCollectionMonitored(mDocumentsCollection);
    configureItemFetchScope(mMonitor->itemFetchScope());
    connect(mMonitor, SIGNAL(itemAdded(Akonadi::Item,Akonadi::Collection)),
            this, SLOT(slotItemAdded(Akonadi::Item,Akonadi::Collection)));
    connect(mMonitor, SIGNAL(itemRemoved(Akonadi::Item)),
            this, SLOT(slotItemRemoved(Akonadi::Item)));
    connect(mMonitor, SIGNAL(itemChanged(Akonadi::Item,QSet<QByteArray>)),
            this, SLOT(slotItemChanged(Akonadi::Item,QSet<QByteArray>)));
}

QVector<SugarEmail> LinkedItemsRepository::emailsForOpportunity(const QString &id) const
{
    return mEmailsHash.value(id);
}

void LinkedItemsRepository::slotEmailsReceived(const Akonadi::Item::List &items)
{
    mEmailsLoaded += items.count();
    foreach(const Akonadi::Item &item, items) {
        storeEmail(item);
    }
    //kDebug() << "loaded" << mEmailsLoaded << "emails; now hash has" << mEmailsHash.count() << "entries";
    if (mEmailsLoaded == mEmailsCollection.statistics().count())
        emit emailsLoaded(mEmailsLoaded);
}

void LinkedItemsRepository::storeEmail(const Akonadi::Item &item)
{
    if (item.hasPayload<SugarEmail>()) {
        SugarEmail email = item.payload<SugarEmail>();
        const QString id = email.id();
        Q_ASSERT(!id.isEmpty());
        removeEmail(id); // handle change of opp
        if (email.parentType() == QLatin1String("Opportunities")) {
            const QString parentId = email.parentId();
            if (!parentId.isEmpty()) {
                mEmailsHash[parentId].append(email);
                mEmailsParentIdHash.insert(id, parentId);
            } else {
                mEmailsParentIdHash.remove(id);
            }
        } else {
            // We also get emails for Accounts and Emails.
            // (well, no longer, we filter this out in the resource)
            //kDebug() << "ignoring emails for" << email.parentType();
        }
    }
}

void LinkedItemsRepository::removeEmail(const QString &id)
{
    Q_ASSERT(!id.isEmpty());
    const QString oldParentId = mEmailsParentIdHash.value(id);
    if (!oldParentId.isEmpty()) {
        kDebug() << "email" << id << "oldParentId" << oldParentId;
        // Email is no longer associated with this opportunity
        QVector<SugarEmail> &emails = mEmailsHash[oldParentId];
        auto it = std::find_if(emails.constBegin(), emails.constEnd(), [&id](const SugarEmail &n) { return n.id() == id; });
        if (it != emails.constEnd()) {
            const int idx = std::distance(emails.constBegin(), it);
            kDebug() << "Removing email at" << idx;
            emails.remove(idx);
        }
    }
}

///

void LinkedItemsRepository::setDocumentsCollection(const Akonadi::Collection &collection)
{
    mDocumentsCollection = collection;
}

void LinkedItemsRepository::loadDocuments()
{
    //kDebug() << "Loading" << mDocumentsCollection.statistics().count() << "documents";

    // load documents
    Akonadi::ItemFetchJob *job = new Akonadi::ItemFetchJob(mDocumentsCollection, this);
    configureItemFetchScope(job->fetchScope());
    connect(job, SIGNAL(itemsReceived(Akonadi::Item::List)),
            this, SLOT(slotDocumentsReceived(Akonadi::Item::List)));
}

QVector<SugarDocument> LinkedItemsRepository::documentsForOpportunity(const QString &id) const
{
    return mOpportunityDocumentsHash.value(id);
}

QVector<SugarDocument> LinkedItemsRepository::documentsForAccount(const QString &id) const
{
    return mAccountDocumentsHash.value(id);
}

void LinkedItemsRepository::slotDocumentsReceived(const Akonadi::Item::List &items)
{
    mDocumentsLoaded += items.count();
    foreach(const Akonadi::Item &item, items) {
        storeDocument(item);
    }

    if (mDocumentsLoaded == mDocumentsCollection.statistics().count())
        emit documentsLoaded(mDocumentsLoaded);
}

void LinkedItemsRepository::storeDocument(const Akonadi::Item &item)
{
    if (item.hasPayload<SugarDocument>()) {
        SugarDocument document = item.payload<SugarDocument>();
        const QString id = document.id();
        Q_ASSERT(!id.isEmpty());

        removeDocument(id); // handle change of opp

        mDocumentsAccountIdHash.remove(id);
        mDocumentsOpportunityIdHash.remove(id);

        Q_FOREACH (const QString &accountId, document.linkedAccountIds()) {
            mAccountDocumentsHash[accountId].append(document);
            mDocumentsAccountIdHash[id].insert(accountId);
        }

        Q_FOREACH (const QString &opportunityId, document.linkedOpportunityIds()) {
            mOpportunityDocumentsHash[opportunityId].append(document);
            mDocumentsOpportunityIdHash[id].insert(opportunityId);
        }
    }
}

void LinkedItemsRepository::removeDocument(const QString &id)
{
    Q_ASSERT(!id.isEmpty());

    const QSet<QString> oldLinkedAccountIds = mDocumentsAccountIdHash.value(id);
    Q_FOREACH (const QString &oldLinkedAccountId, oldLinkedAccountIds) {
        QVector<SugarDocument> &documents = mAccountDocumentsHash[oldLinkedAccountId];
        auto it = std::find_if(documents.constBegin(), documents.constEnd(), [&id](const SugarDocument &document) { return document.id() == id; });
        if (it != documents.constEnd()) {
            const int idx = std::distance(documents.constBegin(), it);
            kDebug() << "Removing document at" << idx;
            documents.remove(idx);
        }
    }

    const QSet<QString> oldLinkedOpportunityIds = mDocumentsOpportunityIdHash.value(id);
    Q_FOREACH (const QString &oldLinkedOpportunityId, oldLinkedOpportunityIds) {
        QVector<SugarDocument> &documents = mOpportunityDocumentsHash[oldLinkedOpportunityId];
        auto it = std::find_if(documents.constBegin(), documents.constEnd(), [&id](const SugarDocument &document) { return document.id() == id; });
        if (it != documents.constEnd()) {
            const int idx = std::distance(documents.constBegin(), it);
            kDebug() << "Removing document at" << idx;
            documents.remove(idx);
        }
    }
}

void LinkedItemsRepository::configureItemFetchScope(Akonadi::ItemFetchScope &scope)
{
    scope.setFetchRemoteIdentification(false);
    scope.setIgnoreRetrievalErrors(true);
    scope.fetchFullPayload(true);
}

void LinkedItemsRepository::updateItem(const Akonadi::Item &item, const Akonadi::Collection &collection)
{
    if (collection == mNotesCollection) {
        storeNote(item);
    } else if (collection == mEmailsCollection) {
        storeEmail(item);
    } else if (collection == mDocumentsCollection) {
        storeDocument(item);
    } else {
        kWarning() << "Unexpected collection" << collection << ", expected" << mNotesCollection.id() << "," << mEmailsCollection.id() << "or" << mDocumentsCollection.id();
    }
}

void LinkedItemsRepository::slotItemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection)
{
    kDebug() << item.id() << item.mimeType();
    updateItem(item, collection);
}

void LinkedItemsRepository::slotItemRemoved(const Akonadi::Item &item)
{
    kDebug() << item.id();
    const Akonadi::Collection collection = item.parentCollection();
    // Don't use the payload to handle removals (no payload anymore on removal)
    if (collection == mNotesCollection) {
        removeNote(item.remoteId());
    } else if (collection == mEmailsCollection) {
        removeEmail(item.remoteId());
    } else if (collection == mDocumentsCollection) {
        removeDocument(item.remoteId());
    } else {
        kWarning() << "Unexpected collection" << collection << ", expected" << mNotesCollection.id() << "," << mEmailsCollection.id() << "or" << mDocumentsCollection.id();
    }
}

void LinkedItemsRepository::slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray> &partIdentifiers)
{
    // I get only REMOTEREVISION even when changing the parentid in the SugarEmail...
    //kDebug() << item.id() << partIdentifiers;
    Q_UNUSED(partIdentifiers);

    // Handle the case where the parent id changed
    const Akonadi::Collection collection = item.parentCollection();
    updateItem(item, collection);
}
