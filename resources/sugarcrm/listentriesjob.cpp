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

#include "listentriesjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"
#include "listentriesscope.h"
using namespace KDSoapGenerated;

#include "kdcrmdata/kdcrmutils.h"

#include <KDSoapClient/KDSoapMessage.h>

#include <AkonadiCore/Collection>
#include <AkonadiCore/EntityAnnotationsAttribute>
#include <AkonadiCore/ItemFetchJob>
#include <AkonadiCore/ItemFetchScope>

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
          mHandler(0),
          mStage(GetCount),
          mCollectionAttributesChanged(false)
    {
    }

public:
    Collection mCollection;
    ModuleHandler *mHandler;
    ListEntriesScope mListScope;
    Stage mStage;
    QString mLatestTimestampFromItems;
    Akonadi::Item::List mFullItems;
    bool mCollectionAttributesChanged;

public: // slots
    void getEntriesCountDone(const KDSoapGenerated::TNS__Get_entries_count_result &callResult);
    void getEntriesCountError(const KDSoapMessage &fault);
    void listEntriesDone(const KDSoapGenerated::TNS__Get_entry_list_result &callResult);
    void listEntriesError(const KDSoapMessage &fault);
};

void ListEntriesJob::Private::getEntriesCountDone(const TNS__Get_entries_count_result &callResult)
{
    if (q->handleError(callResult.error())) {
        return;
    }

    const int count = callResult.result_count();
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << q << "About to list" << count << "entries";
    emit q->totalItems( count );
    if (count == 0) {
        q->emitResult();
    } else {
        mStage = GetExisting;
        q->startSugarTask(); // proceed to next stage
    }
}

void ListEntriesJob::Private::getEntriesCountError(const KDSoapMessage &fault)
{
    if (!q->handleLoginError(fault)) {
        qWarning() << q << fault.faultAsString();

        q->setError(SugarJob::SoapError);
        q->setErrorText(fault.faultAsString());
        q->emitResult();
    }
}

static const char s_timeStampKey[] = "timestamp";
static const char s_supportedFieldsKey[] = "supportedFields"; // duplicated in page.cpp

// When the resource evolves and can store more things (or if e.g. encoding bugs are fixed)
// then increasing the expected version ensures that old caches are thrown out.
static const char s_contentsVersionKey[] = "contentsVersion";

void ListEntriesJob::Private::listEntriesDone(const KDSoapGenerated::TNS__Get_entry_list_result &callResult)
{
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << q << "stage" << mStage << "error" << callResult.error().number();
    if (q->handleError(callResult.error())) {
        return;
    }
    if (callResult.result_count() > 0) { // result_count is the size of entry_list, e.g. 100.
        mCollectionAttributesChanged = mHandler->parseFieldList(mCollection, callResult.field_list());

        Item::List items =
            mHandler->itemsFromListEntriesResponse(callResult.entry_list(), mCollection, &mLatestTimestampFromItems);

        if (mHandler->needsExtraInformation())
            mHandler->getExtraInformation(items);
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "List Entries for" << mHandler->moduleName()
                 << "received" << items.count() << "items.";

        if (mListScope.isUpdateScope()) {
            emit q->itemsReceived(items);
        } else {
            mFullItems.append(items);
            emit q->progress(mFullItems.count());
        }

        mListScope.setOffset(callResult.next_offset());
        mHandler->listEntries(mListScope);
    } else {
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << q << "List Entries for" << mHandler->moduleName() << "done. Latest timestamp=" << mLatestTimestampFromItems;

        // Store timestamp into DB, to persist it across restarts
        // Add one second, so we don't get the same stuff all over again every time
        KDCRMUtils::incrementTimeStamp(mLatestTimestampFromItems);
        EntityAnnotationsAttribute *annotationsAttribute =
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
        const QString fields = mHandler->supportedCRMFields().join(",");
        if (annotationsAttribute->value(s_supportedFieldsKey) != fields) {
            annotationsAttribute->insert(s_supportedFieldsKey, fields);
            changed = true;
        }

        mCollectionAttributesChanged = mCollectionAttributesChanged || changed;
        q->emitResult();
    }
}

void ListEntriesJob::Private::listEntriesError(const KDSoapMessage &fault)
{
    if (!q->handleLoginError(fault)) {
        qWarning() << q << "List Entries Error:" << fault.faultAsString();

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

Item::List ListEntriesJob::fullItems() const
{
    return d->mFullItems;
}

int ListEntriesJob::currentContentsVersion(const Collection &collection)
{
    EntityAnnotationsAttribute *annotationsAttribute =
            collection.attribute<EntityAnnotationsAttribute>();
    if (annotationsAttribute)
        return annotationsAttribute->value(s_contentsVersionKey).toInt();
    return 0;
}

QString ListEntriesJob::latestTimestamp(const Akonadi::Collection &collection, ModuleHandler *handler)
{
    EntityAnnotationsAttribute *annotationsAttribute =
            collection.attribute<EntityAnnotationsAttribute>();
    if (annotationsAttribute) {
        const int contentsVersion = annotationsAttribute->value(s_contentsVersionKey).toInt();
        const int expected = handler->expectedContentsVersion();
        if (contentsVersion != expected) {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << handler->moduleName() << ": contents version" << contentsVersion << "expected" << expected << "-> we'll download all items again";
            return QString();
        }

        QString timeStamp = annotationsAttribute->value(s_timeStampKey);

        // If we don't have enum definitions, go back a little to get some update
        if (!handler->hasEnumDefinitions()) {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << handler->moduleName() << "no enum definitions, going back a bit to get something";
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
    Q_ASSERT(d->mCollection.isValid());
    Q_ASSERT(d->mHandler != 0);

    switch (d->mStage) {
    case Private::GetCount:
        d->mHandler->getEntriesCount(d->mListScope);
        break;
    case Private::GetExisting:
        d->mHandler->listEntries(d->mListScope);
        break;
    }
}
#include "moc_listentriesjob.cpp"
