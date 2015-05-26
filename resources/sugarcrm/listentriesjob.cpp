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

#include "listentriesjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"
#include "listentriesscope.h"

using namespace KDSoapGenerated;
#include <KDSoapClient/KDSoapMessage.h>

#include <akonadi/collection.h>

#include <KDebug>

#include <QStringList>

#include <akonadi/collectionmodifyjob.h>
#include <akonadi/entityannotationsattribute.h>
using namespace Akonadi;

class ListEntriesJob::Private
{
    ListEntriesJob *const q;

public:
    enum Stage {
        GetCount,
        GetExisting,
        GetDeleted
    };

    explicit Private(ListEntriesJob *parent, const Akonadi::Collection &collection)
        : q(parent), mCollection(collection), mHandler(0), mStage(GetCount)
    {
    }

public:
    Collection mCollection;
    ModuleHandler *mHandler;
    ListEntriesScope mListScope;
    Stage mStage;

public: // slots
    void getEntriesCountDone(const KDSoapGenerated::TNS__Get_entries_count_result &callResult);
    void getEntriesCountError(const KDSoapMessage &fault);
    void listEntriesDone(const KDSoapGenerated::TNS__Get_entry_list_result &callResult);
    void listEntriesError(const KDSoapMessage &fault);
};

static const char s_timeStampKey[] = "timestamp";

void ListEntriesJob::Private::getEntriesCountDone(const TNS__Get_entries_count_result &callResult)
{
    if (q->handleError(callResult.error())) {
        return;
    }

    kDebug() << q << "About to list" << callResult.result_count() << "entries";
    emit q->totalItems( callResult.result_count() );
    mStage = GetExisting;
    q->startSugarTask(); // proceed to next stage
}

void ListEntriesJob::Private::getEntriesCountError(const KDSoapMessage &fault)
{
    if (!q->handleLoginError(fault)) {
        kWarning() << q << fault.faultAsString();

        q->setError(SugarJob::SoapError);
        q->setErrorText(fault.faultAsString());
        q->emitResult();
    }
}

void ListEntriesJob::Private::listEntriesDone(const KDSoapGenerated::TNS__Get_entry_list_result &callResult)
{
    kDebug() << q << "stage" << mStage << "error" << callResult.error().number();
    if (q->handleError(callResult.error())) {
        return;
    }
    if (callResult.result_count() > 0) { // result_count is the size of entry_list, e.g. 100.
        Item::List items =
            mHandler->itemsFromListEntriesResponse(callResult.entry_list(), mCollection);
        switch (mStage) {
        case GetCount:
            Q_ASSERT(0);
            break;
        case GetExisting:
            if (mHandler->needsExtraInformation())
                mHandler->getExtraInformation(items);
            kDebug() << "List Entries for" << mHandler->moduleName()
                     << "received" << items.count() << "items";
            emit q->itemsReceived(items);
            break;
        case GetDeleted:
            kDebug() << "List Entries for" << mHandler->moduleName()
                     << "received" << items.count() << "deletes";
            emit q->deletedReceived(items);
            break;
        }
        mListScope.setOffset(callResult.next_offset());
    } else {
        if (mStage == GetDeleted || !mListScope.isUpdateScope()) {
            kDebug() << q << "List Entries for" << mHandler->moduleName() << "done, will emitResult";

            // Store timestamp into DB, to persist it across restarts
            EntityAnnotationsAttribute *annotationsAttribute =
                    mCollection.attribute<EntityAnnotationsAttribute>( Akonadi::Collection::AddIfMissing );
            Q_ASSERT(annotationsAttribute);
            if (annotationsAttribute->value(s_timeStampKey) != mHandler->latestTimestamp()) {
                annotationsAttribute->insert(s_timeStampKey, mHandler->latestTimestamp());

                // no parent, this job will outlive the ListEntriesJob
                Akonadi::CollectionModifyJob *modJob = new Akonadi::CollectionModifyJob(mCollection);
                Q_UNUSED(modJob);
            }
            q->emitResult();
            return;
        }

        Q_ASSERT(mListScope.isUpdateScope() && mStage == GetExisting);

        // if GetExisting is finished, continue getting the deleted
        mStage = GetDeleted;
        mListScope.fetchDeleted();
        kDebug() << q << "Listing updates for" << mHandler->moduleName()
                 << "done, getting deletes";
    }

    mHandler->listEntries(mListScope);
}

void ListEntriesJob::Private::listEntriesError(const KDSoapMessage &fault)
{    
    if (!q->handleLoginError(fault)) {
        kWarning() << q << "List Entries Error:" << fault.faultAsString();

        q->setError(SugarJob::SoapError);
        q->setErrorText(fault.faultAsString());
        q->emitResult();
    }
}

ListEntriesJob::ListEntriesJob(const Akonadi::Collection &collection, SugarSession *session, QObject *parent)
    : SugarJob(session, parent), d(new Private(this, collection))
{
    connect(soap(), SIGNAL(get_entries_countDone(KDSoapGenerated::TNS__Get_entries_count_result)),
            this, SLOT(getEntriesCountDone(KDSoapGenerated::TNS__Get_entries_count_result)));
    connect(soap(), SIGNAL(get_entries_countError(KDSoapMessage)),
            this, SLOT(getEntriesCountError(KDSoapMessage)));

    connect(soap(), SIGNAL(get_entry_listDone(KDSoapGenerated::TNS__Get_entry_list_result)),
            this,  SLOT(listEntriesDone(KDSoapGenerated::TNS__Get_entry_list_result)));
    connect(soap(), SIGNAL(get_entry_listError(KDSoapMessage)),
            this,  SLOT(listEntriesError(KDSoapMessage)));

    d->mStage = Private::GetCount;
    d->mListScope = ListEntriesScope(latestTimestamp());

    kDebug() << this;
}

ListEntriesJob::~ListEntriesJob()
{
    delete d;
    kDebug() << this;
}

Collection ListEntriesJob::collection() const
{
    return d->mCollection;
}

void ListEntriesJob::setModule(ModuleHandler *handler)
{
    d->mHandler = handler;
}

QString ListEntriesJob::latestTimestamp() const
{
    EntityAnnotationsAttribute *annotationsAttribute =
            d->mCollection.attribute<EntityAnnotationsAttribute>();
    if (annotationsAttribute)
        return annotationsAttribute->value(s_timeStampKey);
    return QString();
}

// This can be called multiple times, in case we lose connection at some point during the job
// So don't reset d->mStage here, we don't want to rewind to GetCount (which confuses itemsync)
void ListEntriesJob::startSugarTask()
{
    Q_ASSERT(d->mCollection.isValid());
    Q_ASSERT(d->mHandler != 0);

    switch (d->mStage) {
    case Private::GetCount:
        d->mHandler->getEntriesCount(d->mListScope);
        break;
    case Private::GetExisting:
    case Private::GetDeleted:
        d->mHandler->listEntries(d->mListScope);
        break;
    }
}

#include "listentriesjob.moc"
