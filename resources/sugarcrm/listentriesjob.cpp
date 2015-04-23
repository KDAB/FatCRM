#include "listentriesjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"

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
    kDebug() << "About to list" << callResult.result_count() << "entries";
    emit q->totalItems( callResult.result_count() );
    mStage = GetExisting;
    mHandler->listEntries(mListScope);
}

void ListEntriesJob::Private::getEntriesCountError(const KDSoapMessage &fault)
{
    if (!q->handleLoginError(fault)) {
        kWarning() << fault.faultAsString();

        q->setError(SugarJob::SoapError);
        q->setErrorText(fault.faultAsString());
        q->emitResult();
    }
}

void ListEntriesJob::Private::listEntriesDone(const KDSoapGenerated::TNS__Get_entry_list_result &callResult)
{
    if (callResult.result_count() > 0) { // result_count is the size of entry_list, e.g. 100.
        const Item::List items =
            mHandler->itemsFromListEntriesResponse(callResult.entry_list(), mCollection);
        switch (mStage) {
        case GetCount:
            Q_ASSERT(0);
            break;
        case GetExisting:
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
    } else {
        if (mStage == GetDeleted || !mListScope.isUpdateScope()) {
            kDebug() << "List Entries for" << mHandler->moduleName() << "done";

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
        kDebug() << "Listing updates for" << mHandler->moduleName()
                 << "done, getting deletes";
    }

    mListScope.setOffset(callResult.next_offset());
    mHandler->listEntries(mListScope);
}

void ListEntriesJob::Private::listEntriesError(const KDSoapMessage &fault)
{
    if (!q->handleLoginError(fault)) {
        kWarning() << "List Entries Error:" << fault.faultAsString();

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
}

ListEntriesJob::~ListEntriesJob()
{
    delete d;
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

void ListEntriesJob::startSugarTask()
{
    Q_ASSERT(d->mCollection.isValid());
    Q_ASSERT(d->mHandler != 0);

    d->mListScope = ListEntriesScope(latestTimestamp());
    d->mStage = Private::GetCount;
    d->mHandler->getEntriesCount(d->mListScope);
}

#include "listentriesjob.moc"
