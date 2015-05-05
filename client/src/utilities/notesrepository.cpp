#include "notesrepository.h"
#include <akonadi/collection.h>
#include <akonadi/itemfetchjob.h>
#include <akonadi/itemfetchscope.h>

NotesRepository::NotesRepository(QObject *parent) :
    QObject(parent)
{
}

void NotesRepository::setNotesCollection(const Akonadi::Collection &collection)
{
    mNotesCollection = collection;

    // load notes
    Akonadi::ItemFetchJob *job = new Akonadi::ItemFetchJob(mNotesCollection, this);
    job->fetchScope().setFetchRemoteIdentification(false);
    job->fetchScope().setIgnoreRetrievalErrors(true);
    job->fetchScope().fetchFullPayload(true);
    connect(job, SIGNAL(itemsReceived(Akonadi::Item::List)),
            this, SLOT(slotNotesReceived(Akonadi::Item::List)));
    // TODO setFetchChangedSince(), later on
}

QVector<SugarNote> NotesRepository::notesForOpportunity(const QString &id) const
{
    return mNotesHash.value(id);
}

void NotesRepository::slotNotesReceived(const Akonadi::Item::List &items)
{
    foreach(const Akonadi::Item &item, items) {
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
    //kDebug() << "got" << items.count() << "items; now hash has" << mNotesHash.count() << "entries";
}

void NotesRepository::setEmailsCollection(const Akonadi::Collection &collection)
{
    mEmailsCollection = collection;

    // load emails
    Akonadi::ItemFetchJob *job = new Akonadi::ItemFetchJob(mEmailsCollection, this);
    job->fetchScope().setFetchRemoteIdentification(false);
    job->fetchScope().setIgnoreRetrievalErrors(true);
    job->fetchScope().fetchFullPayload(true);
    connect(job, SIGNAL(itemsReceived(Akonadi::Item::List)),
            this, SLOT(slotEmailsReceived(Akonadi::Item::List)));
    // TODO setFetchChangedSince(), later on
}

QVector<SugarEmail> NotesRepository::emailsForOpportunity(const QString &id) const
{
    return mEmailsHash.value(id);
}

void NotesRepository::slotEmailsReceived(const Akonadi::Item::List &items)
{
    foreach(const Akonadi::Item &item, items) {
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
    //kDebug() << "got" << items.count() << "items; now hash has" << mEmailsHash.count() << "entries";
}
