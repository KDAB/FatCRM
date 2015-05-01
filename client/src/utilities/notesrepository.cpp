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
                //kDebug() << "ignoring notes for" << note.parentType();
            }
        }
    }
    //kDebug() << "got" << items.count() << "items; now hash has" << mNotesHash.count() << "entries";
}
