/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "listdeletedentriesjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"
#include "listentriesscope.h"
using namespace KDSoapGenerated;

#include "kdcrmdata/kdcrmutils.h"

#include <KDSoapClient/KDSoapMessage.h>

#include <AkonadiCore/Collection>
#include <AkonadiCore/ItemFetchJob>
#include <AkonadiCore/ItemDeleteJob>
#include <AkonadiCore/ItemFetchScope>
#include <AkonadiCore/EntityAnnotationsAttribute>
using namespace Akonadi;

#include "sugarcrmresource_debug.h"

#include <QStringList>

class ListDeletedEntriesJob::Private
{
    ListDeletedEntriesJob *const q;

public:
    explicit Private(ListDeletedEntriesJob *parent, const Akonadi::Collection &collection)
        : q(parent),
          mCollection(collection),
          mHandler(nullptr),
          mCollectionAttributesChanged(false)
    {
    }

    // Step 1: get deleted items from Sugar
    // Step 2: resolve RID to ID
    // Step 3: delete the items

public:
    Collection mCollection;
    ModuleHandler *mHandler;
    ListEntriesScope mListScope;
    Akonadi::Item::List mPendingDeletedItems;
    QString mLatestTimestampFromItems;
    bool mCollectionAttributesChanged;

    void listEntriesDone(const EntriesListResult &callResult);
    void listEntriesError(int error, const QString &errorMessage);
    void listNextEntries();

public: // slots
    void slotResolvedDeletedItems(KJob *);
    void slotDeleteJobResult(KJob *job);

    void updateAnnotationAttribute();
};

void ListDeletedEntriesJob::Private::listEntriesDone(const EntriesListResult &callResult)
{
    if (callResult.resultCount > 0) { // result_count is the size of entry_list, e.g. 100.
        Item::List items =
            mHandler->itemsFromListEntriesResponse(callResult.entryList, mCollection, &mLatestTimestampFromItems);
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "List Entries for" << mHandler->module() << "since" << mLatestTimestampFromItems
                 << "received" << items.count() << "deletes";
        mPendingDeletedItems += items;
        mListScope.setOffset(callResult.nextOffset);

        // Avoid double recursion
        QMetaObject::invokeMethod(q, "listNextEntries", Qt::QueuedConnection);

    } else {
        if (!mPendingDeletedItems.isEmpty()) {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Resolving" << mPendingDeletedItems.count() << "deleted items";
            // workaround for RID REMOVE not working in akonadiserver. Our deleted items need an ID.
            Akonadi::ItemFetchJob *job = new Akonadi::ItemFetchJob(mPendingDeletedItems, q);
            job->setCollection(mCollection);
            job->fetchScope().fetchAllAttributes(false);
            job->fetchScope().fetchFullPayload(false);
            job->fetchScope().setCacheOnly(true);
            connect(job, SIGNAL(result(KJob*)),
                    q, SLOT(slotResolvedDeletedItems(KJob*)));
        } else {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "found 0 deleted items";
            q->emitResult();
        }
    }
}

void ListDeletedEntriesJob::Private::listEntriesError(int error, const QString &errorMessage)
{
    if (error == SugarJob::CouldNotConnectError) {
        // Invalid login error, meaning we need to log in again
        if (q->shouldTryRelogin()) {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Got error 10, probably a session timeout, let's login again";
            QMetaObject::invokeMethod(q, "startLogin", Qt::QueuedConnection);
            return;
        }
    }
    qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << q << error << errorMessage;

    q->setError(SugarJob::SoapError);
    q->setErrorText(errorMessage);
    q->emitResult();
}

void ListDeletedEntriesJob::Private::listNextEntries()
{
    QString errorMessage;
    EntriesListResult entriesListResult;
    int result = mHandler->listEntries(mListScope, entriesListResult, errorMessage);
    if (result == KJob::NoError) {
        listEntriesDone(entriesListResult);
    } else {
        listEntriesError(result, errorMessage);
    }
}

void ListDeletedEntriesJob::Private::slotResolvedDeletedItems(KJob *job)
{
    mPendingDeletedItems.clear();

    qDebug() << job->errorString();

    if (job->error()) {
        // Update attribute even on error, because the most common error is "item already deleted locally"
        // (especially the very first time this code will run...)
        updateAnnotationAttribute();

        q->setError(job->error());
        q->setErrorText(job->errorText());
        q->emitResult();
    } else {
        Akonadi::ItemFetchJob *fetchJob = static_cast<Akonadi::ItemFetchJob *>(job);
        const Akonadi::Item::List items = fetchJob->items();

        Akonadi::ItemDeleteJob *deleteJob = new Akonadi::ItemDeleteJob(items, q);
        connect(deleteJob, SIGNAL(result(KJob*)), q, SLOT(slotDeleteJobResult(KJob*)));
    }
}

static const char s_deletedItemsTimeStampKey[] = "deleteditems_timestamp";

void ListDeletedEntriesJob::Private::updateAnnotationAttribute()
{
    // Store timestamp into DB, to persist it across restarts
    // Add one second, so we don't get the same stuff all over again every time

    KDCRMUtils::incrementTimeStamp(mLatestTimestampFromItems);
    EntityAnnotationsAttribute *annotationsAttribute =
            mCollection.attribute<EntityAnnotationsAttribute>(Akonadi::Collection::AddIfMissing);
    Q_ASSERT(annotationsAttribute);
    if (!mLatestTimestampFromItems.isEmpty() && annotationsAttribute->value(s_deletedItemsTimeStampKey) != mLatestTimestampFromItems) {
        annotationsAttribute->insert(s_deletedItemsTimeStampKey, mLatestTimestampFromItems);
        mCollectionAttributesChanged = true;
    }
}

void ListDeletedEntriesJob::Private::slotDeleteJobResult(KJob *job)
{
    if (job->error()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << job->errorString();
        q->setError(job->error());
        q->setErrorText(job->errorText());
    } else {
        updateAnnotationAttribute();
    }

    q->emitResult();
}

ListDeletedEntriesJob::ListDeletedEntriesJob(const Akonadi::Collection &collection, SugarSession *session, QObject *parent)
    : SugarJob(session, parent), d(new Private(this, collection))
{
    //qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << this;
}

ListDeletedEntriesJob::~ListDeletedEntriesJob()
{
    delete d;
    //qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << this;
}

Collection ListDeletedEntriesJob::collection() const
{
    return d->mCollection;
}

void ListDeletedEntriesJob::setModule(ModuleHandler *handler)
{
    d->mHandler = handler;
}

ModuleHandler *ListDeletedEntriesJob::module() const
{
    return d->mHandler;
}

void ListDeletedEntriesJob::setLatestTimestamp(const QString &timestamp)
{
    d->mListScope = ListEntriesScope(timestamp);
    d->mListScope.fetchDeleted();
}

void ListDeletedEntriesJob::setInitialTimestamp(const QString &timestamp)
{
    d->mLatestTimestampFromItems = timestamp;
    d->updateAnnotationAttribute();
}

void ListDeletedEntriesJob::setCollectionAttributesChanged(bool b)
{
    d->mCollectionAttributesChanged = b;
}

bool ListDeletedEntriesJob::collectionAttributesChanged() const
{
    return d->mCollectionAttributesChanged;
}

QString ListDeletedEntriesJob::latestTimestamp(const Akonadi::Collection &collection)
{
    EntityAnnotationsAttribute *annotationsAttribute =
            collection.attribute<EntityAnnotationsAttribute>();
    if (annotationsAttribute) {
        return annotationsAttribute->value(s_deletedItemsTimeStampKey);
    }
    return QString();
}

void ListDeletedEntriesJob::startSugarTask()
{
    Q_ASSERT(d->mCollection.isValid());
    Q_ASSERT(d->mHandler != nullptr);
    d->listNextEntries();
}
#include "moc_listdeletedentriesjob.cpp"
