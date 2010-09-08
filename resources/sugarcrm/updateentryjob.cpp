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
    enum Stage {
        Init,
        GetEntry,
        UpdateEntry,
        GetRevision
    };

    explicit Private( UpdateEntryJob *parent, const Item &item )
        : q( parent ), mItem( item ), mHandler( 0 ), mHasConflict( false ), mStage( Init )
    {
    }

public:
    Item mItem;
    ModuleHandler *mHandler;

    bool mHasConflict;
    Item mConflictItem;

    Stage mStage;

public: // slots
    void getEntryDone( const TNS__Get_entry_result &callResult );
    void getEntryError( const KDSoapMessage &fault );
    void setEntryDone( const TNS__Set_entry_result &callResult );
    void setEntryError( const KDSoapMessage &fault );
    void getRevisionDone( const TNS__Get_entry_result &callResult );
    void getRevisionError( const KDSoapMessage &fault );
};

void UpdateEntryJob::Private::getEntryDone( const TNS__Get_entry_result &callResult )
{
    // check if this is our signal
    if ( mStage != GetEntry ) {
        return;
    }

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
        mHasConflict = !remoteItem.remoteRevision().isEmpty();
    } else if ( remoteItem.remoteRevision().isEmpty() ) {
        kWarning() << "remote item (id=" << remoteItem.id()
                   << ", remoteId=" << remoteItem.remoteId()
                   << ") in collection=" << mHandler->moduleName()
                   << "does not have remoteRevision";
    } else {
        // remoteRevision is an ISO date, so string comparisons are accurate for < or >
        mHasConflict = ( remoteItem.remoteRevision() > mItem.remoteRevision() );
    }

    if ( mHasConflict ) {
        mConflictItem = remoteItem;
        q->emitResult(); // TODO should we set an error?
    } else {
        mStage = UpdateEntry;

        mHandler->setEntry( mItem, q->soap(), q->sessionId() );
    }
}

void UpdateEntryJob::Private::getEntryError( const KDSoapMessage &fault )
{
    // check if this is our signal
    if ( mStage != GetEntry ) {
        return;
    }

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

    mStage = Private::GetRevision;

    TNS__Select_fields selectedFields;
    selectedFields.setItems( QStringList() << QLatin1String( "date_modified" ) );

    q->soap()->asyncGet_entry( q->sessionId(), mHandler->moduleName(), mItem.remoteId(), selectedFields );
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

void UpdateEntryJob::Private::getRevisionDone( const TNS__Get_entry_result &callResult )
{
    // check if this is our signal
    if ( mStage != GetRevision ) {
        return;
    }

    const Akonadi::Item::List items =
        mHandler->itemsFromListEntriesResponse( callResult.entry_list(), mItem.parentCollection() );
    Q_ASSERT( items.count() == 1 );

    mItem.setRemoteRevision( items[ 0 ].remoteRevision() );
    kDebug() << "Got remote revision" << mItem.remoteRevision();

    q->emitResult();
}

void UpdateEntryJob::Private::getRevisionError( const KDSoapMessage &fault )
{
    // check if this is our signal
    if ( mStage != GetRevision ) {
        return;
    }

    kWarning() << "Error when getting remote revision:" << fault.faultAsString();

    // the item has been added we just don't have a server side datetime
    q->emitResult();
}

UpdateEntryJob::UpdateEntryJob( const Akonadi::Item &item, SugarSession *session, QObject *parent )
    : SugarJob( session, parent ), d( new Private( this, item ) )
{
    connect( soap(), SIGNAL( get_entryDone( TNS__Get_entry_result ) ),
             this,  SLOT( getEntryDone( TNS__Get_entry_result ) ) );
    connect( soap(), SIGNAL( get_entryError( KDSoapMessage ) ),
             this,  SLOT( getEntryError( KDSoapMessage ) ) );

    connect( soap(), SIGNAL( set_entryDone( TNS__Set_entry_result ) ),
             this,  SLOT( setEntryDone( TNS__Set_entry_result ) ) );
    connect( soap(), SIGNAL( set_entryError( KDSoapMessage ) ),
             this,  SLOT( setEntryError( KDSoapMessage ) ) );

    connect( soap(), SIGNAL( get_entryDone( TNS__Get_entry_result ) ),
             this,  SLOT( getRevisionDone( TNS__Get_entry_result ) ) );
    connect( soap(), SIGNAL( get_entryError( KDSoapMessage ) ),
             this,  SLOT( getRevisionError( KDSoapMessage ) ) );
}

UpdateEntryJob::~UpdateEntryJob()
{
    delete d;
}

void UpdateEntryJob::setModule( ModuleHandler *handler )
{
    d->mHandler = handler;
}

ModuleHandler *UpdateEntryJob::module() const
{
    return d->mHandler;
}

Item UpdateEntryJob::item() const
{
    return d->mItem;
}

bool UpdateEntryJob::hasConflict() const
{
    return d->mHasConflict;
}

Item UpdateEntryJob::conflictItem() const
{
    return d->mConflictItem;
}

void UpdateEntryJob::startSugarTask()
{
    Q_ASSERT( d->mItem.isValid() );
    Q_ASSERT( d->mHandler != 0 );

    d->mStage = Private::GetEntry;

    if ( !d->mHandler->getEntry( d->mItem, soap(), sessionId() ) ) {
        setError( SugarJob::SoapError ); // TODO should be a different error code
        setErrorText( i18nc( "@info:status", "Attempting to modify a malformed item in folder %1",
                             d->mHandler->moduleName() ) );
        emitResult();
    }
}

#include "updateentryjob.moc"