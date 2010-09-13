#include "listentriesjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"

#include <KDSoapMessage.h>

#include <akonadi/collection.h>

#include <KDebug>

#include <QStringList>

using namespace Akonadi;

class ListEntriesJob::Private
{
    ListEntriesJob *const q;

public:
    explicit Private( ListEntriesJob *parent, const Akonadi::Collection &collection )
        : q( parent ), mCollection( collection ), mHandler( 0 ),
          mListScope( ListEntriesScope::scopeForAll() )
    {
    }

public:
    const Collection mCollection;
    ModuleHandler *mHandler;
    ListEntriesScope mListScope;

public: // slots
    void listEntriesDone( const TNS__Get_entry_list_result &callResult );
    void listEntriesError( const KDSoapMessage &fault );
};

void ListEntriesJob::Private::listEntriesDone( const TNS__Get_entry_list_result &callResult )
{
    if ( callResult.result_count() > 0 ) {
        const Item::List items =
            mHandler->itemsFromListEntriesResponse( callResult.entry_list(), mCollection );
        kDebug() << "List Entries for" << mHandler->moduleName()
                 << "received" << items.count() << "items";
        emit q->itemsReceived( items );

        mListScope.setOffset( callResult.next_offset() );
        mHandler->listEntries( mListScope, q->soap(), q->sessionId() );
    } else {
        kDebug() << "List Entries for" << mHandler->moduleName() << "done";
        q->emitResult();
    }
}

void ListEntriesJob::Private::listEntriesError( const KDSoapMessage &fault )
{
    if ( !q->handleLoginError( fault ) ) {
        kWarning() << "List Entries Error:" << fault.faultAsString();

        q->setError( SugarJob::SoapError );
        q->setErrorText( fault.faultAsString() );
        q->emitResult();
    }
}

ListEntriesJob::ListEntriesJob( const Akonadi::Collection &collection, SugarSession *session, QObject *parent )
    : SugarJob( session, parent ), d( new Private( this, collection ) )
{
    connect( soap(), SIGNAL( get_entry_listDone( TNS__Get_entry_list_result ) ),
             this,  SLOT( listEntriesDone( TNS__Get_entry_list_result ) ) );
    connect( soap(), SIGNAL( get_entry_listError( KDSoapMessage ) ),
             this,  SLOT( listEntriesError( KDSoapMessage ) ) );
}

ListEntriesJob::~ListEntriesJob()
{
    delete d;
}

void ListEntriesJob::setModule( ModuleHandler *handler )
{
    d->mHandler = handler;
}

void ListEntriesJob::startSugarTask()
{
    Q_ASSERT( d->mCollection.isValid() );
    Q_ASSERT( d->mHandler != 0 );

    d->mListScope.setOffset( 0 );
    d->mHandler->listEntries( d->mListScope, soap(), sessionId() );
}

#include "listentriesjob.moc"