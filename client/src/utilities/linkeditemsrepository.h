/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "kdcrmdata/sugardocument.h"
#include "kdcrmdata/sugaremail.h"
#include "kdcrmdata/sugarnote.h"
#include "kdcrmdata/sugaropportunity.h"
#include "fatcrmprivate_export.h"

#include <AkonadiCore/Item>
#include <AkonadiCore/Collection>

#include <QObject>

#include <KContacts/Addressee>

class CollectionManager;

namespace Akonadi
{
    class Monitor;
    class ItemFetchScope;
}

/**
 * The LinkedItemsRepository class stores the Documents, Notes and Emails (abstracted as "items" in this class)
 * associated with Accounts, Contacts and Opportunities (the main objects in FatCRM).
 *
 * The repository monitors the Documents, Notes and Emails folders in order to update itself automatically.
 */
class FATCRMPRIVATE_EXPORT LinkedItemsRepository : public QObject
{
    Q_OBJECT
public:
    explicit LinkedItemsRepository(CollectionManager *collectionManager, QObject *parent = nullptr);

    void clear();

    void setNotesCollection(const Akonadi::Collection &collection);
    Akonadi::Collection notesCollection() const;
    void setEmailsCollection(const Akonadi::Collection &collection);
    void setDocumentsCollection(const Akonadi::Collection &collection);
    Akonadi::Collection documentsCollection() const;

    void loadNotes();
    void loadEmails();
    void loadDocuments();
    void monitorChanges();

    QVector<SugarNote> notesForAccount(const QString &id) const;
    QVector<SugarNote> notesForContact(const QString &id) const;
    QVector<SugarNote> notesForOpportunity(const QString &id) const;
    QVector<SugarEmail> emailsForAccount(const QString &id) const;
    QVector<SugarEmail> emailsForContact(const QString &id) const;
    QVector<SugarEmail> emailsForOpportunity(const QString &id) const;

    QVector<SugarDocument> documentsForAccount(const QString &id) const;
    QVector<SugarDocument> documentsForOpportunity(const QString &id) const;

    Akonadi::Item documentItem(const QString &id) const;

    void addOpportunity(const SugarOpportunity &opp);
    void removeOpportunity(const SugarOpportunity &opp);
    void updateOpportunity(const SugarOpportunity &opp);
    QVector<SugarOpportunity> opportunitiesForAccount(const QString &accountId) const;

    void addContact(const KContacts::Addressee &contact);
    void removeContact(const KContacts::Addressee &contact);
    void updateContact(const KContacts::Addressee &contact);
    QVector<KContacts::Addressee> contactsForAccount(const QString &accountId) const;

signals:
    void notesLoaded(int count);
    void emailsLoaded(int count);
    void documentsLoaded(int count);

    // Emitted when notes, emails or documents for this account have been modified.
    void accountModified(const QString &accountId);
    // Emitted when notes, emails or documents for this opportunity have been modified.
    void opportunityModified(const QString &oppId);
    // Emitted when notes, emails or documents for this contact have been modified.
    void contactModified(const QString &contactId);

private Q_SLOTS:
    void slotNotesReceived(const Akonadi::Item::List &items);
    void slotItemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection);
    void slotItemRemoved(const Akonadi::Item &item);
    void slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray>& partIdentifiers);

    void slotEmailsReceived(const Akonadi::Item::List &items);
    void slotDocumentsReceived(const Akonadi::Item::List &items);

private:
    void storeNote(const Akonadi::Item &item, bool emitSignals);
    void removeNote(const QString &id);
    void storeEmail(const Akonadi::Item &item, bool emitSignals);
    void removeEmail(const QString &id);
    void storeDocument(const Akonadi::Item &item, bool emitSignals);
    void removeDocument(const QString &id);
    void configureItemFetchScope(Akonadi::ItemFetchScope &scope);
    void updateItem(const Akonadi::Item &item, const Akonadi::Collection &collection);

    Akonadi::Collection mNotesCollection;
    Akonadi::Monitor *mMonitor;
    typedef QHash<QString, QVector<SugarNote> > NotesHash;
    NotesHash mAccountNotesHash;
    NotesHash mContactNotesHash;
    NotesHash mOpportunityNotesHash;
    QHash<QString, QString> mNotesAccountIdHash; // note id -> account id (to handle removals)
    QHash<QString, QString> mNotesContactIdHash; // note id -> contact id (to handle removals)
    QHash<QString, QString> mNotesOpportunityIdHash; // note id -> opportunity id (to handle removals)
    int mNotesLoaded;

    Akonadi::Collection mEmailsCollection;
    typedef QHash<QString, QVector<SugarEmail> > EmailsHash;
    EmailsHash mAccountEmailsHash;
    EmailsHash mContactEmailsHash;
    EmailsHash mOpportunityEmailsHash;
    QHash<QString, QString> mEmailsAccountIdHash; // email id -> account id (to handle removals)
    QHash<QString, QString> mEmailsContactIdHash; // email id -> contact id (to handle removals)
    QHash<QString, QString> mEmailsOpportunityIdHash; // email id -> opportunity id (to handle removals)
    int mEmailsLoaded;

    Akonadi::Collection mDocumentsCollection;
    typedef QHash<QString, QVector<SugarDocument> > DocumentsHash;
    DocumentsHash mAccountDocumentsHash;
    DocumentsHash mOpportunityDocumentsHash;
    QHash<QString, QSet<QString> > mDocumentsAccountIdHash; // document id -> account ids (to handle removals)
    QHash<QString, QSet<QString> > mDocumentsOpportunityIdHash; // document id -> opportunity ids (to handle removals)
    QHash<QString, Akonadi::Item> mDocumentItems;
    int mDocumentsLoaded;

    using OpportunitiesHash = QHash<QString, QVector<SugarOpportunity>>;
    using ContactsHash = QHash<QString, QVector<KContacts::Addressee>>;
    OpportunitiesHash mAccountOpportunitiesHash;
    ContactsHash mAccountContactsHash;

    CollectionManager *mCollectionManager;
};

#endif // LINKEDITEMSREPOSITORY_H
