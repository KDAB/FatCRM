/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>
           Jeremy Entressangle <jeremy.entressangle@kdab.com>

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
#include "wsdl_sugar41.h"
#include "listentriesscope.h"
#include "sugarprotocolbase.h"
using namespace KDSoapGenerated;

#include <QTimer>

#include "kdcrmdata/kdcrmutils.h"

#include <KDSoapClient/KDSoapMessage.h>

#include <Akonadi/Collection>
#include <Akonadi/EntityAnnotationsAttribute>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>

using namespace Akonadi;

#include "sugarcrmresource_debug.h"

#include <QStringList>

class ListEntriesJob::Private
{
    ListEntriesJob *const q;

public:
    enum Stage {
        GetCount,
        GetExisting
    };

    explicit Private(ListEntriesJob *parent, const Akonadi::Collection &collection)
        : q(parent),
          mCollection(collection),
          mHandler(nullptr),
          mStage(GetCount),
          mCollectionAttributesChanged(false)
    {
        qRegisterMetaType<EntriesListResult>();
    }

public:
    Collection mCollection;
    ModuleHandler *mHandler;
    ListEntriesScope mListScope;
    Stage mStage;
    QString mLatestTimestampFromItems;
    Akonadi::Item::List mDeletedItems;
    int mTotalCount = 0;
    int mItemsAlreadyEmitted = 0;
    bool mCollectionAttributesChanged;

public:
    void getEntriesCountDone(int count);
    void listEntriesDone(const EntriesListResult &callResult);
    void handleError(int error, const QString &errorMessage);
    void listNextEntries();
};

void ListEntriesJob::Private::getEntriesCountDone(int count)
{
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << q << "About to list" << count << "entries";
    emit q->totalItems(count);
    if (count == 0) {
        q->emitResult();
    } else {
        mTotalCount = count;
        mStage = GetExisting;
        q->startSugarTask(); // proceed to next stage
    }
}

static const char s_timeStampKey[] = "timestamp"; // duplicated in collectionmanager.cpp
static const char s_supportedFieldsKey[] = "supportedFields"; // duplicated in collectionmanager.cpp

// When the resource evolves and can store more things (or if e.g. encoding bugs are fixed)
// then increasing the expected version ensures that old caches are thrown out.
static const char s_contentsVersionKey[] = "contentsVersion";

void ListEntriesJob::Private::listNextEntries()
{
    QString errorMessage;
    EntriesListResult entriesListResult;
    int result = mHandler->listEntries(mListScope, entriesListResult, errorMessage);
    if (result == KJob::NoError) {
        listEntriesDone(entriesListResult);
    } else {
        handleError(result, errorMessage);
    }
}

void ListEntriesJob::Private::listEntriesDone(const EntriesListResult &callResult)
{
    if (callResult.resultCount > 0) { // result_count is the size of entry_list, e.g. 100.
        Item::List items =
            mHandler->itemsFromListEntriesResponse(callResult.entryList, mCollection, mDeletedItems, &mLatestTimestampFromItems);

        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "List Entries for" << mHandler->module()
                 << "received" << callResult.entryList.items().size() << "SOAP entries, processed into" << items.count() << "items and" << mDeletedItems.count() << "deleted items.";
        if (!items.isEmpty()) {
            if (mHandler->needsExtraInformation())
                mHandler->getExtraInformation(items);

            emit q->itemsReceived(items, mListScope.isUpdateScope());
        }
        mItemsAlreadyEmitted += items.count();
        emit q->progress(mItemsAlreadyEmitted + mDeletedItems.count());

        mListScope.setOffset(callResult.nextOffset);

        if (mItemsAlreadyEmitted < mTotalCount) {
            // Avoid double recursion
            QMetaObject::invokeMethod(q, "listNextEntries", Qt::QueuedConnection);
            return;
        }
    }

    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << q << "List Entries for" << mHandler->module() << "done. Latest timestamp=" << mLatestTimestampFromItems;

    // Store timestamp into DB, to persist it across restarts

    // Here we used to increment mLatestTimestampFromItems, to avoid getting the same last modified object
    // every time, but this can break if we end up doing modify+list+modify in the same second
    // (could be 2 or 3 different users, so it could happen).
    // So now we'll always get the last modified object again, on every listing.
    // TODO: we could optimize away the sending of that object to akonadi
    // (if SugarAccount::operator== says same object as last time, don't call itemsRetrievedIncremental)

    auto *annotationsAttribute =
            mCollection.attribute<EntityAnnotationsAttribute>( Akonadi::Collection::AddIfMissing );
    Q_ASSERT(annotationsAttribute);
    bool changed = false;
    if (!mLatestTimestampFromItems.isEmpty() && annotationsAttribute->value(s_timeStampKey) != mLatestTimestampFromItems) {
        annotationsAttribute->insert(s_timeStampKey, mLatestTimestampFromItems);
        changed = true;
    }
    if (!mListScope.isUpdateScope()) {
        // We just did a full listing (first time, or after a contents version upgrade)
        // then upgrade the contents version attribute.
        const int currentVersion = mHandler->expectedContentsVersion();
        if (annotationsAttribute->value(s_contentsVersionKey).toInt() != currentVersion) {
            annotationsAttribute->insert(s_contentsVersionKey, QString::number(currentVersion));
            changed = true;
        }
    }
    // Also store the list of supported fields, so that the GUI knows what to expect and set
    const QString fields = mHandler->supportedCRMFields().join(QStringLiteral(","));
    if (annotationsAttribute->value(s_supportedFieldsKey) != fields) {
        annotationsAttribute->insert(s_supportedFieldsKey, fields);
        changed = true;
    }

    mCollectionAttributesChanged = mCollectionAttributesChanged || changed;
    q->emitResult();
}

void ListEntriesJob::Private::handleError(int error, const QString &errorMessage)
{
    qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << q << error << errorMessage;
    if (q->handleConnectError(error, errorMessage)) {
        return;
    }

    q->setError(SugarJob::SoapError);
    q->setErrorText(errorMessage);
    q->emitResult();
}

ListEntriesJob::ListEntriesJob(const Akonadi::Collection &collection, SugarSession *session, QObject *parent)
    : SugarJob(session, parent), d(new Private(this, collection))
{
    d->mStage = Private::GetCount;
    //qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << this;
}

ListEntriesJob::~ListEntriesJob()
{
    delete d;
    //qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << this;
}

Collection ListEntriesJob::collection() const
{
    return d->mCollection;
}

void ListEntriesJob::setModule(ModuleHandler *handler)
{
    d->mHandler = handler;
}

ModuleHandler *ListEntriesJob::module() const
{
    return d->mHandler;
}

void ListEntriesJob::setLatestTimestamp(const QString &timestamp)
{
    d->mListScope = ListEntriesScope(timestamp);
    if (!timestamp.isEmpty()) {
        d->mListScope.fetchDeleted();
    }
}

QString ListEntriesJob::newTimestamp() const
{
    return d->mLatestTimestampFromItems;
}

bool ListEntriesJob::collectionAttributesChanged() const
{
    return d->mCollectionAttributesChanged;
}

bool ListEntriesJob::isUpdateJob() const
{
    return d->mListScope.isUpdateScope();
}

Item::List ListEntriesJob::deletedItems() const
{
    return d->mDeletedItems;
}

int ListEntriesJob::currentContentsVersion(const Collection &collection)
{
    auto *annotationsAttribute =
            collection.attribute<EntityAnnotationsAttribute>();
    if (annotationsAttribute)
        return annotationsAttribute->value(s_contentsVersionKey).toInt();
    return 0;
}

// static
QString ListEntriesJob::latestTimestamp(const Akonadi::Collection &collection, ModuleHandler *handler)
{
    auto *annotationsAttribute =
            collection.attribute<EntityAnnotationsAttribute>();
    if (annotationsAttribute) {
        const int contentsVersion = annotationsAttribute->value(s_contentsVersionKey).toInt();
        const int expected = handler->expectedContentsVersion();
        if (contentsVersion != expected) {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << handler->module() << ": contents version" << contentsVersion << "expected" << expected << "-> we'll download all items again";
            return QString();
        }

        QString timeStamp = annotationsAttribute->value(s_timeStampKey);

        // If we don't have enum definitions, go back a little to get some update
        if (!handler->hasEnumDefinitions()) {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << handler->module() << "no enum definitions, going back a bit to get something";
            KDCRMUtils::decrementTimeStamp(timeStamp);
        }
        return timeStamp;
    }
    return QString();
}

// This can be called multiple times, in case we lose connection at some point during the job
// So don't reset d->mStage here, we don't want to rewind to GetCount (which confuses itemsync)
void ListEntriesJob::startSugarTask()
{
    Q_ASSERT(d->mHandler != nullptr);

    QString errorMessage;
    switch (d->mStage) {
    case Private::GetCount: {
        int entriesCount;
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG()) << "Listing entries since" << d->mListScope.timestamp() << (d->mListScope.includeDeleted() ? "including deleted items" : "without deleted items");
        const int result = d->mHandler->getEntriesCount(d->mListScope, entriesCount, errorMessage);
        if (result == KJob::NoError) {
            d->getEntriesCountDone(entriesCount);
        } else {
            d->handleError(result, errorMessage);
        }
        break;
    }
    case Private::GetExisting:
        d->listNextEntries();
        break;
    }
}
#include "moc_listentriesjob.cpp"
