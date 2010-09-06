#include "updateentryjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"

#include <KDSoapMessage.h>

#include <akonadi/item.h>

#include <KDebug>
#include <KLocale>

#include <QStringList>

using namespace Akonadi;

class UpdateEntryJob::Private
{
    UpdateEntryJob *const q;

public:
    explicit Private( UpdateEntryJob *parent, const Item &item )
        : q( parent ), mItem( item ), mHandler( 0 )
    {
    }

public:
    Item mItem;
    ModuleHandler *mHandler;

public: // slots
    void getEntryDone( const TNS__Get_entry_result &callResult );
    void getEntryError( const KDSoapMessage &fault );
    void setEntryDone( const TNS__Set_entry_result &callResult );
    void setEntryError( const KDSoapMessage &fault );
};

void UpdateEntryJob::Private::getEntryDone( const TNS__Get_entry_result &callResult )
{
    const Akonadi::Item::List items =
        mHandler->itemsFromListEntriesResponse( callResult.entry_list(), mItem.parentCollection() );
    Q_ASSERT( items.count() == 1 );

    const Akonadi::Item remoteItem = items.first();

    kDebug() << "remote=" << remoteItem.remoteRevision()
             << "local="  << mItem.remoteRevision();
    if ( mItem.remoteRevision().isEmpty() ) {
        kWarning() << "local item (id=" << mItem.id()
                   << ", remoteId=" << mItem.remoteId()
                   << ") in collection=" << mHandler->moduleName()
                   << "does not have remoteRevision";
    } else if ( remoteItem.remoteRevision().isEmpty() ) {
        kWarning() << "remote item (id=" << remoteItem.id()
                   << ", remoteId=" << remoteItem.remoteId()
                   << ") in collection=" << mHandler->moduleName()
                   << "does not have remoteRevision";
    } else if ( remoteItem.remoteRevision() > mItem.remoteRevision() ) {
        // remoteRevision is an ISO date, so string comparisons are accurate for < or >
        emit q->conflictDetected( mItem, remoteItem );
        q->emitResult(); // TODO should we set an error?
    } else {
        mHandler->setEntry( mItem, q->soap(), q->sessionId() );
    }
}

void UpdateEntryJob::Private::getEntryError( const KDSoapMessage &fault )
{
    if ( !q->handleLoginError( fault ) ) {
        kWarning() << "Update Entry Error:" << fault.faultAsString();

        q->setError( SugarJob::SoapError );
        q->setErrorText( fault.faultAsString() );
        q->emitResult();
    }
}

void UpdateEntryJob::Private::setEntryDone( const TNS__Set_entry_result &callResult )
{
    kDebug() << "Updated entry" << callResult.id() << "in module" << mHandler->moduleName();
    mItem.setRemoteId( callResult.id() );

    q->emitResult();
}

void UpdateEntryJob::Private::setEntryError( const KDSoapMessage &fault )
{
    if ( !q->handleLoginError( fault ) ) {
        kWarning() << "Update Entry Error:" << fault.faultAsString();

        q->setError( SugarJob::SoapError );
        q->setErrorText( fault.faultAsString() );
        q->emitResult();
    }
}

UpdateEntryJob::UpdateEntryJob( const Akonadi::Item &item, SugarSession *session, QObject *parent )
    : SugarJob( session, parent ), d( new Private( this, item ) )
{
    connect( soap(), SIGNAL( set_entryDone( TNS__Set_entry_result ) ),
             this,  SLOT( setEntryDone( TNS__Set_entry_result ) ) );
    connect( soap(), SIGNAL( set_entryError( KDSoapMessage ) ),
             this,  SLOT( setEntryError( KDSoapMessage ) ) );
}

UpdateEntryJob::~UpdateEntryJob()
{
    delete d;
}

void UpdateEntryJob::setModule( ModuleHandler *handler )
{
    d->mHandler = handler;
}

Item UpdateEntryJob::item() const
{
    return d->mItem;
}

void UpdateEntryJob::startSugarTask()
{
    Q_ASSERT( d->mItem.isValid() );
    Q_ASSERT( d->mHandler != 0 );

    if ( !d->mHandler->getEntry( d->mItem, soap(), sessionId() ) ) {
        setError( SugarJob::SoapError ); // TODO should be a different error code
        setErrorText( i18nc( "@info:status", "Attempting to modify a malformed item in folder %1",
                             d->mHandler->moduleName() ) );
        emitResult();
    }
}

#include "updateentryjob.moc"