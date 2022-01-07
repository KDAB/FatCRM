/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "sugarcontactwrapper.h"

#include <Akonadi/Collection>
#include <Akonadi/CollectionStatistics>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/Monitor>

#include <QStringList>

#include "fatcrm_client_debug.h"

#define kDebug() qCDebug(FATCRM_CLIENT_LOG)
#define kWarning() qCWarning(FATCRM_CLIENT_LOG)

LinkedItemsRepository::LinkedItemsRepository(CollectionManager *collectionManager, QObject *parent) :
    QObject(parent),
    mMonitor(nullptr),
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
    mAccountOpportunitiesHash.clear();
    mAccountContactsHash.clear();
    delete mMonitor;
    mMonitor = nullptr;
}

void LinkedItemsRepository::setNotesCollection(const Akonadi::Collection &collection)
{
    mNotesCollection = collection;
}

void LinkedItemsRepository::loadNotes()
{
    //qCDebug(FATCRM_CLIENT_LOG) << "Loading" << mNotesCollection.statistics().count() << "notes";

    // load notes
    auto *job = new Akonadi::ItemFetchJob(mNotesCollection, this);
    configureItemFetchScope(job->fetchScope());
    connect(job, &Akonadi::ItemFetchJob::itemsReceived,
            this, &LinkedItemsRepository::slotNotesReceived);
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
        storeNote(item, false);
    }
    //qCDebug(FATCRM_CLIENT_LOG) << "loaded" << mNotesLoaded << "notes; now hash has" << mNotesHash.count() << "entries";
    if (mNotesLoaded == mNotesCollection.statistics().count())
        emit notesLoaded(mNotesLoaded);
}

void LinkedItemsRepository::storeNote(const Akonadi::Item &item, bool emitSignals)
{
    if (item.hasPayload<SugarNote>()) {
        SugarNote note = item.payload<SugarNote>();
        const QString id = note.id();
        if (id.isEmpty()) {
            // We just created a note in akonadi, and it hasn't been synced to Sugar yet. We can't store it yet.
            // Once it's created slotItemChanged will be called and we'll come here again to store it for real.
            return;
        }
        removeNote(id); // handle change of parent
        const QString parentId = note.parentId();
        if (note.parentType() == QLatin1String("Accounts")) {
            if (!parentId.isEmpty()) {
                mAccountNotesHash[parentId].append(note);
                mNotesAccountIdHash.insert(id, parentId);
                if (emitSignals) {
                    emit accountModified(parentId);
                }
            } else {
                mNotesAccountIdHash.remove(id);
            }
        } else if (note.parentType() == QLatin1String("Contacts")) {
            if (!parentId.isEmpty()) {
                mContactNotesHash[parentId].append(note);
                mNotesContactIdHash.insert(id, parentId);
                if (emitSignals) {
                    emit contactModified(parentId);
                }
            } else {
                mNotesContactIdHash.remove(id);
            }
        } else if (note.parentType() == QLatin1String("Opportunities")) {
            if (!parentId.isEmpty()) {
                mOpportunityNotesHash[parentId].append(note);
                mNotesOpportunityIdHash.insert(id, parentId);
                if (emitSignals) {
                    emit opportunityModified(parentId);
                }
            } else {
                mNotesOpportunityIdHash.remove(id);
            }
        } else {
            // We filter out the rest in the resource, but just in case:
            qCDebug(FATCRM_CLIENT_LOG) << "ignoring notes for" << note.parentType();
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
            emit accountModified(oldAccountId);
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
            emit contactModified(oldContactId);
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
            qCDebug(FATCRM_CLIENT_LOG) << "Removing note at" << idx;
            notes.remove(idx);
            emit opportunityModified(oldOpportunityId);
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
    qCDebug(FATCRM_CLIENT_LOG) << "Loading" << mEmailsCollection.statistics().count() << "emails";

    // load emails
    auto *job = new Akonadi::ItemFetchJob(mEmailsCollection, this);
    configureItemFetchScope(job->fetchScope());
    connect(job, &Akonadi::ItemFetchJob::itemsReceived,
            this, &LinkedItemsRepository::slotEmailsReceived);
}

void LinkedItemsRepository::monitorChanges()
{
    // cppcheck-suppress publicAllocationError
    mMonitor = new Akonadi::Monitor(this);
    mMonitor->setCollectionMonitored(mNotesCollection);
    mMonitor->setCollectionMonitored(mEmailsCollection);
    mMonitor->setCollectionMonitored(mDocumentsCollection);
    configureItemFetchScope(mMonitor->itemFetchScope());
    connect(mMonitor, &Akonadi::Monitor::itemAdded,
            this, &LinkedItemsRepository::slotItemAdded);
    connect(mMonitor, &Akonadi::Monitor::itemRemoved,
            this, &LinkedItemsRepository::slotItemRemoved);
    connect(mMonitor, &Akonadi::Monitor::itemChanged,
            this, &LinkedItemsRepository::slotItemChanged);
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
        storeEmail(item, false);
    }
    //qCDebug(FATCRM_CLIENT_LOG) << "loaded" << mEmailsLoaded << "emails";
    if (mEmailsLoaded == mEmailsCollection.statistics().count()) {
        emit emailsLoaded(mEmailsLoaded);
    }
}

void LinkedItemsRepository::storeEmail(const Akonadi::Item &item, bool emitSignals)
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
                if (emitSignals) {
                    emit accountModified(parentId);
                }
            } else {
                mEmailsAccountIdHash.remove(id);
            }
        } else if (email.parentType() == QLatin1String("Contacts")) {
            if (!parentId.isEmpty()) {
                mContactEmailsHash[parentId].append(email);
                mEmailsContactIdHash.insert(id, parentId);
                if (emitSignals) {
                    emit contactModified(parentId);
                }

            } else {
                mEmailsContactIdHash.remove(id);
            }
        } else if (email.parentType() == QLatin1String("Opportunities")) {
            if (!parentId.isEmpty()) {
                mOpportunityEmailsHash[parentId].append(email);
                mEmailsOpportunityIdHash.insert(id, parentId);
                if (emitSignals) {
                    emit opportunityModified(parentId);
                }
            } else {
                mEmailsOpportunityIdHash.remove(id);
            }
        } else {
            // We filter out the rest in the resource, but just in case:
            qCDebug(FATCRM_CLIENT_LOG) << "ignoring emails for" << email.parentType();
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
            emit accountModified(oldAccountId);
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
            emit contactModified(oldContactId);
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
            qCDebug(FATCRM_CLIENT_LOG) << "Removing email at" << idx;
            emails.remove(idx);
            emit opportunityModified(oldOpportunityId);
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
    //qCDebug(FATCRM_CLIENT_LOG) << "Loading" << mDocumentsCollection.statistics().count() << "documents";

    // load documents
    auto *job = new Akonadi::ItemFetchJob(mDocumentsCollection, this);
    configureItemFetchScope(job->fetchScope());
    connect(job, &Akonadi::ItemFetchJob::itemsReceived,
            this, &LinkedItemsRepository::slotDocumentsReceived);
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

void LinkedItemsRepository::addOpportunity(const SugarOpportunity &opp)
{
    mAccountOpportunitiesHash[opp.accountId()].push_back(opp);
}

static bool eraseOpportunityByUid(QVector<SugarOpportunity> &vec, const QString &id)
{
    auto it = std::find_if(vec.begin(), vec.end(), [&id](const SugarOpportunity &testOpp) {
        return id == testOpp.id();
    });
    if (it != vec.end()) {
        vec.erase(it);
        return true;
    }
    return false;
}

void LinkedItemsRepository::removeOpportunity(const SugarOpportunity &opp)
{
    auto & opps = mAccountOpportunitiesHash[opp.accountId()];
    eraseOpportunityByUid(opps, opp.id());
}

void LinkedItemsRepository::updateOpportunity(const SugarOpportunity &opp)
{
    for (auto accIt = mAccountOpportunitiesHash.begin(); accIt != mAccountOpportunitiesHash.end(); ++accIt) {
        QVector<SugarOpportunity> &opps = accIt.value();
        if (eraseOpportunityByUid(opps, opp.id()))
            break;
    }

    addOpportunity(opp);
}

QVector<SugarOpportunity> LinkedItemsRepository::opportunitiesForAccount(const QString &accountId) const
{
    if (accountId.isEmpty())
        return {};
    return mAccountOpportunitiesHash.value(accountId);
}

void LinkedItemsRepository::addContact(const KContacts::Addressee &contact)
{
    mAccountContactsHash[SugarContactWrapper(contact).accountId()].push_back(contact);
}

static bool eraseContactByUid(QVector<KContacts::Addressee> &vec, const QString &uid)
{
    auto it = std::find_if(vec.begin(), vec.end(), [&uid](const KContacts::Addressee &testContact) {
        return uid == testContact.uid();
    });
    if (it != vec.end()) {
        vec.erase(it);
        return true;
    }
    return false;
}

void LinkedItemsRepository::removeContact(const KContacts::Addressee &contact)
{
    auto & contacts = mAccountContactsHash[SugarContactWrapper(contact).accountId()];
    eraseContactByUid(contacts, contact.uid());
}

void LinkedItemsRepository::updateContact(const KContacts::Addressee &contact)
{
    for (auto accIt = mAccountContactsHash.begin(); accIt != mAccountContactsHash.end(); ++accIt) {
        QVector<KContacts::Addressee> &contacts = accIt.value();
        if (eraseContactByUid(contacts, contact.uid())) {
            break;
        }
    }

    addContact(contact);
}

QVector<KContacts::Addressee> LinkedItemsRepository::contactsForAccount(const QString &accountId) const
{
    if (accountId.isEmpty())
        return {};
    return mAccountContactsHash.value(accountId);
}

void LinkedItemsRepository::slotDocumentsReceived(const Akonadi::Item::List &items)
{
    mDocumentsLoaded += items.count();
    foreach(const Akonadi::Item &item, items) {
        storeDocument(item, false);
    }

    if (mDocumentsLoaded == mDocumentsCollection.statistics().count())
        emit documentsLoaded(mDocumentsLoaded);
}

void LinkedItemsRepository::storeDocument(const Akonadi::Item &item, bool emitSignals)
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
            if (emitSignals) {
                emit accountModified(accountId);
            }
        }

        Q_FOREACH (const QString &opportunityId, document.linkedOpportunityIds()) {
            mOpportunityDocumentsHash[opportunityId].append(document);
            mDocumentsOpportunityIdHash[id].insert(opportunityId);
            if (emitSignals) {
                emit opportunityModified(opportunityId);
            }
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
            qCDebug(FATCRM_CLIENT_LOG) << "Removing document at" << idx;
            documents.remove(idx);
            emit accountModified(oldLinkedAccountId);
        }
    }

    const QSet<QString> oldLinkedOpportunityIds = mDocumentsOpportunityIdHash.value(id);
    Q_FOREACH (const QString &oldLinkedOpportunityId, oldLinkedOpportunityIds) {
        QVector<SugarDocument> &documents = mOpportunityDocumentsHash[oldLinkedOpportunityId];
        auto it = std::find_if(documents.constBegin(), documents.constEnd(), [&id](const SugarDocument &document) { return document.id() == id; });
        if (it != documents.constEnd()) {
            const int idx = std::distance(documents.constBegin(), it);
            qCDebug(FATCRM_CLIENT_LOG) << "Removing document at" << idx;
            documents.remove(idx);
            emit opportunityModified(oldLinkedOpportunityId);
        }
    }

    mDocumentItems.remove(id);
}

void LinkedItemsRepository::configureItemFetchScope(Akonadi::ItemFetchScope &scope)
{
    scope.setFetchRemoteIdentification(true); // remoteId() is used by slotItemRemoved
    scope.setIgnoreRetrievalErrors(true);
    scope.fetchFullPayload(true);
}

void LinkedItemsRepository::updateItem(const Akonadi::Item &item, const Akonadi::Collection &collection)
{
    if (collection == mNotesCollection) {
        storeNote(item, true);
    } else if (collection == mEmailsCollection) {
        storeEmail(item, true);
    } else if (collection == mDocumentsCollection) {
        storeDocument(item, true);
    } else {
        qCWarning(FATCRM_CLIENT_LOG) << "Unexpected collection" << collection << ", expected" << mNotesCollection.id() << "," << mEmailsCollection.id() << "or" << mDocumentsCollection.id();
    }
}

Akonadi::Collection LinkedItemsRepository::notesCollection() const
{
    return mNotesCollection;
}

void LinkedItemsRepository::slotItemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection)
{
    qCDebug(FATCRM_CLIENT_LOG) << item.id() << item.mimeType();
    updateItem(item, collection);
}

void LinkedItemsRepository::slotItemRemoved(const Akonadi::Item &item)
{
    qCDebug(FATCRM_CLIENT_LOG) << item.id();
    const Akonadi::Collection collection = item.parentCollection();
    // Don't use the payload to handle removals (no payload anymore on removal)
    if (collection == mNotesCollection) {
        removeNote(item.remoteId());
    } else if (collection == mEmailsCollection) {
        removeEmail(item.remoteId());
    } else if (collection == mDocumentsCollection) {
        removeDocument(item.remoteId());
    } else {
        qCWarning(FATCRM_CLIENT_LOG) << "Unexpected collection" << collection << ", expected" << mNotesCollection.id() << "," << mEmailsCollection.id() << "or" << mDocumentsCollection.id();
    }
}

void LinkedItemsRepository::slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray> &partIdentifiers)
{
    // I get only REMOTEREVISION when changing the parentid in the SugarNote or SugarEmail.
    // I also get REMOTEID when creating a note.
    // But anyway we handle all cases in updateItem().
    //qCDebug(FATCRM_CLIENT_LOG) << item.id() << partIdentifiers;
    Q_UNUSED(partIdentifiers);

    // Handle the case where the parent id changed
    const Akonadi::Collection collection = item.parentCollection();
    updateItem(item, collection);
}
