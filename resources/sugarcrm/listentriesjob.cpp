#include "listentriesjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"

using namespace KDSoapGenerated;
#include <KDSoapClient/KDSoapMessage.h>

#include <akonadi/collection.h>

#include <KDebug>

#include <QStringList>

using namespace Akonadi;

class ListEntriesJob::Private
{
    ListEntriesJob *const q;

public:
    enum Stage {
        GetExisting,
        GetDeleted
    };

    explicit Private(ListEntriesJob *parent, const Akonadi::Collection &collection)
        : q(parent), mCollection(collection), mHandler(0), mStage(GetExisting)
    {
    }

public:
    const Collection mCollection;
    ModuleHandler *mHandler;
    ListEntriesScope mListScope;
    Stage mStage;

public: // slots
    void listEntriesDone(const KDSoapGenerated::TNS__Get_entry_list_result &callResult);
    void listEntriesError(const KDSoapMessage &fault);
};

void ListEntriesJob::Private::listEntriesDone(const KDSoapGenerated::TNS__Get_entry_list_result &callResult)
{
    if (callResult.result_count() > 0) {
        const Item::List items =
            mHandler->itemsFromListEntriesResponse(callResult.entry_list(), mCollection);
        switch (mStage) {
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
    connect(soap(), SIGNAL(get_entry_listDone(KDSoapGenerated::TNS__Get_entry_list_result)),
            this,  SLOT(listEntriesDone(KDSoapGenerated::TNS__Get_entry_list_result)));
    connect(soap(), SIGNAL(get_entry_listError(KDSoapMessage)),
            this,  SLOT(listEntriesError(KDSoapMessage)));
}

ListEntriesJob::~ListEntriesJob()
{
    delete d;
}

void ListEntriesJob::setModule(ModuleHandler *handler)
{
    d->mHandler = handler;
}

void ListEntriesJob::startSugarTask()
{
    Q_ASSERT(d->mCollection.isValid());
    Q_ASSERT(d->mHandler != 0);

    d->mStage = Private::GetExisting;
    d->mListScope = ListEntriesScope(d->mHandler->latestTimestamp());

    d->mHandler->listEntries(d->mListScope);
}

#include "listentriesjob.moc"
