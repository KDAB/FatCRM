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
#include "collectionmanager.h"

#include <Akonadi/Collection>
#include <Akonadi/CollectionStatistics>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/Monitor>

#include <QStringList>

LinkedItemsRepository::LinkedItemsRepository(CollectionManager *collectionManager, QObject *parent) :
    QObject(parent),
    mMonitor(0),
    mNotesLoaded(0),
    mEmailsLoaded(0),
    mDocumentsLoaded(0),
    mCollectionManager(collectionManager)
{
}

void LinkedItemsRepository::clear()
{
    mNotesLoaded = 0;
    mEmailsLoaded = 0;
    mDocumentsLoaded = 0;
    mAccountNotesHash.clear();
    mContactNotesHash.clear();
    mOpportunityNotesHash.clear();
    mAccountEmailsHash.clear();
    mContactEmailsHash.clear();
    mOpportunityEmailsHash.clear();
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

QVector<SugarNote> LinkedItemsRepository::notesForAccount(const QString &id) const
{
    return mAccountNotesHash.value(id);
}

QVector<SugarNote> LinkedItemsRepository::notesForContact(const QString &id) const
{
    return mContactNotesHash.value(id);
}

QVector<SugarNote> LinkedItemsRepository::notesForOpportunity(const QString &id) const
{
    return mOpportunityNotesHash.value(id);
}

void LinkedItemsRepository::slotNotesReceived(const Akonadi::Item::List &items)
{
    mNotesLoaded += items.count();
    foreach(const Akonadi::Item &item, items) {
        storeNote(item);
    }
    //kDebug() << "loaded" << mNotesLoaded << "notes";
    if (mNotesLoaded == mNotesCollection.statistics().count())
        emit notesLoaded(mNotesLoaded);
}

void LinkedItemsRepository::storeNote(const Akonadi::Item &item)
{
    if (item.hasPayload<SugarNote>()) {
        SugarNote note = item.payload<SugarNote>();
        const QString id = note.id();
        Q_ASSERT(!id.isEmpty());
        removeNote(id); // handle change of parent
        const QString parentId = note.parentId();
        if (note.parentType() == QLatin1String("Accounts")) {
            if (!parentId.isEmpty()) {
                mAccountNotesHash[parentId].append(note);
                mNotesAccountIdHash.insert(id, parentId);
            } else {
                mNotesAccountIdHash.remove(id);
            }
        } else if (note.parentType() == QLatin1String("Contacts")) {
            if (!parentId.isEmpty()) {
                mContactNotesHash[parentId].append(note);
                mNotesContactIdHash.insert(id, parentId);
            } else {
                mNotesContactIdHash.remove(id);
            }
        } else if (note.parentType() == QLatin1String("Opportunities")) {
            if (!parentId.isEmpty()) {
                mOpportunityNotesHash[parentId].append(note);
                mNotesOpportunityIdHash.insert(id, parentId);
            } else {
                mNotesOpportunityIdHash.remove(id);
            }
        } else {
            // We filter out the rest in the resource, but just in case:
            kDebug() << "ignoring notes for" << note.parentType();
        }
    } else {
        kWarning() << "Note item without a SugarNote payload?" << item.id() << item.remoteId();
    }
}

void LinkedItemsRepository::removeNote(const QString &id)
{
    Q_ASSERT(!id.isEmpty());

    const QString oldAccountId = mNotesAccountIdHash.value(id);
    if (!oldAccountId.isEmpty()) {
        //kDebug() << "note" << id << "oldAccountId" << oldAccountId;
        // Note is no longer associated with this account
        QVector<SugarNote> &notes = mAccountNotesHash[oldAccountId];
        auto it = std::find_if(notes.constBegin(), notes.constEnd(), [id](const SugarNote &n) { return n.id() == id; });
        if (it != notes.constEnd()) {
            const int idx = std::distance(notes.constBegin(), it);
            kDebug() << "Removing note at" << idx;
            notes.remove(idx);
        }
    }

    const QString oldContactId = mNotesContactIdHash.value(id);
    if (!oldContactId.isEmpty()) {
        //kDebug() << "note" << id << "oldContactId" << oldContactId;
        // Note is no longer associated with this contact
        QVector<SugarNote> &notes = mContactNotesHash[oldContactId];
        auto it = std::find_if(notes.constBegin(), notes.constEnd(), [id](const SugarNote &n) { return n.id() == id; });
        if (it != notes.constEnd()) {
            const int idx = std::distance(notes.constBegin(), it);
            kDebug() << "Removing note at" << idx;
            notes.remove(idx);
        }
    }

    const QString oldOpportunityId = mNotesOpportunityIdHash.value(id);
    if (!oldOpportunityId.isEmpty()) {
        //kDebug() << "note" << id << "oldOpportunityId" << oldOpportunityId;
        // Note is no longer associated with this opportunity
        QVector<SugarNote> &notes = mOpportunityNotesHash[oldOpportunityId];
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
    kDebug() << "Loading" << mEmailsCollection.statistics().count() << "emails";

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
    connect(mMonitor, SIGNAL(collectionChanged(Akonadi::Collection,QSet<QByteArray>)),
            mCollectionManager, SLOT(slotCollectionChanged(Akonadi::Collection,QSet<QByteArray>)));
}

QVector<SugarEmail> LinkedItemsRepository::emailsForAccount(const QString &id) const
{
    return mAccountEmailsHash.value(id);
}

QVector<SugarEmail> LinkedItemsRepository::emailsForContact(const QString &id) const
{
    return mContactEmailsHash.value(id);
}

QVector<SugarEmail> LinkedItemsRepository::emailsForOpportunity(const QString &id) const
{
    return mOpportunityEmailsHash.value(id);
}

void LinkedItemsRepository::slotEmailsReceived(const Akonadi::Item::List &items)
{
    mEmailsLoaded += items.count();
    foreach(const Akonadi::Item &item, items) {
        storeEmail(item);
    }
    //kDebug() << "loaded" << mEmailsLoaded << "emails";
    if (mEmailsLoaded == mEmailsCollection.statistics().count()) {
        emit emailsLoaded(mEmailsLoaded);
    }
}

void LinkedItemsRepository::storeEmail(const Akonadi::Item &item)
{
    if (item.hasPayload<SugarEmail>()) {
        SugarEmail email = item.payload<SugarEmail>();
        const QString id = email.id();
        Q_ASSERT(!id.isEmpty());
        removeEmail(id); // handle change of parent
        const QString parentId = email.parentId();
        if (email.parentType() == QLatin1String("Accounts")) {
            if (!parentId.isEmpty()) {
                mAccountEmailsHash[parentId].append(email);
                mEmailsAccountIdHash.insert(id, parentId);
            } else {
                mEmailsAccountIdHash.remove(id);
            }
        } else if (email.parentType() == QLatin1String("Contacts")) {
            if (!parentId.isEmpty()) {
                mContactEmailsHash[parentId].append(email);
                mEmailsContactIdHash.insert(id, parentId);
            } else {
                mEmailsContactIdHash.remove(id);
            }
        } else if (email.parentType() == QLatin1String("Opportunities")) {
            if (!parentId.isEmpty()) {
                mOpportunityEmailsHash[parentId].append(email);
                mEmailsOpportunityIdHash.insert(id, parentId);
            } else {
                mEmailsOpportunityIdHash.remove(id);
            }
        } else {
            // We filter out the rest in the resource, but just in case:
            kDebug() << "ignoring emails for" << email.parentType();
        }
    } else {
        kWarning() << "Email item without a SugarEmail payload?" << item.id() << item.remoteId();
    }
}

void LinkedItemsRepository::removeEmail(const QString &id)
{
    Q_ASSERT(!id.isEmpty());

    const QString oldAccountId = mEmailsAccountIdHash.value(id);
    if (!oldAccountId.isEmpty()) {
        //kDebug() << "email" << id << "oldAccountId" << oldAccountId;
        // Email is no longer associated with this account
        QVector<SugarEmail> &emails = mAccountEmailsHash[oldAccountId];
        auto it = std::find_if(emails.constBegin(), emails.constEnd(), [&id](const SugarEmail &n) { return n.id() == id; });
        if (it != emails.constEnd()) {
            const int idx = std::distance(emails.constBegin(), it);
            kDebug() << "Removing email at" << idx;
            emails.remove(idx);
        }
    }

    const QString oldContactId = mEmailsContactIdHash.value(id);
    if (!oldContactId.isEmpty()) {
        //kDebug() << "email" << id << "oldContactId" << oldContactId;
        // Email is no longer associated with this contact
        QVector<SugarEmail> &emails = mContactEmailsHash[oldContactId];
        auto it = std::find_if(emails.constBegin(), emails.constEnd(), [&id](const SugarEmail &n) { return n.id() == id; });
        if (it != emails.constEnd()) {
            const int idx = std::distance(emails.constBegin(), it);
            kDebug() << "Removing email at" << idx;
            emails.remove(idx);
        }
    }

    const QString oldOpportunityId = mEmailsOpportunityIdHash.value(id);
    if (!oldOpportunityId.isEmpty()) {
        //kDebug() << "email" << id << "oldOpportunityId" << oldOpportunityId;
        // Email is no longer associated with this contact
        QVector<SugarEmail> &emails = mOpportunityEmailsHash[oldOpportunityId];
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

Akonadi::Collection LinkedItemsRepository::documentsCollection() const
{
    return mDocumentsCollection;
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

Akonadi::Item LinkedItemsRepository::documentItem(const QString &id) const
{
    return mDocumentItems.value(id);
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

        mDocumentItems[id] = item;
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

    mDocumentItems.remove(id);
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
