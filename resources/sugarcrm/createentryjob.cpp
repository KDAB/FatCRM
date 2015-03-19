#include "createentryjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"

using namespace KDSoapGenerated;
#include <KDSoapClient/KDSoapMessage.h>

#include <akonadi/item.h>

#include <KDebug>
#include <KLocale>

#include <QStringList>

using namespace Akonadi;

class CreateEntryJob::Private
{
    CreateEntryJob *const q;

public:
    enum Stage {
        Init,
        CreateEntry,
        GetEntry
    };

    explicit Private( CreateEntryJob *parent, const Item &item )
        : q( parent ), mItem( item ), mHandler( 0 ), mStage( Init )
    {
    }

public:
    Item mItem;
    ModuleHandler *mHandler;
    Stage mStage;

public: // slots
    void setEntryDone( const TNS__Set_entry_result &callResult );
    void setEntryError( const KDSoapMessage &fault );
    void getEntryDone( const TNS__Get_entry_result &callResult );
    void getEntryError( const KDSoapMessage &fault );
};

void CreateEntryJob::Private::setEntryDone( const TNS__Set_entry_result &callResult )
{
    Q_ASSERT( mStage == CreateEntry );

    kDebug() << "Created entry" << callResult.id() << "in module" << mHandler->moduleName();
    mItem.setRemoteId( callResult.id() );

    mStage = Private::GetEntry;

    if ( !mHandler->getEntry( mItem ) ) {
        // the item has been added we just don't have a server side datetime
        q->emitResult();
    }
}

void CreateEntryJob::Private::setEntryError( const KDSoapMessage &fault )
{
    Q_ASSERT( mStage == CreateEntry );

    if ( !q->handleLoginError( fault ) ) {
        kWarning() << "Create Entry Error:" << fault.faultAsString();

        q->setError( SugarJob::SoapError );
        q->setErrorText( fault.faultAsString() );
        q->emitResult();
    }
}

void CreateEntryJob::Private::getEntryDone( const TNS__Get_entry_result &callResult )
{
    Q_ASSERT( mStage == GetEntry );

    const QList<TNS__Entry_value> entries = callResult.entry_list().items();
    Q_ASSERT( entries.count() == 1 );
    const Akonadi::Item remoteItem = mHandler->itemFromEntry( entries.first(), mItem.parentCollection() );

    Item item = remoteItem;
    item.setId( mItem.id() );
    item.setRevision( mItem.revision() );
    mItem = item;
    kDebug() << "Got entry with revision" << mItem.remoteRevision();

    q->emitResult();
}

void CreateEntryJob::Private::getEntryError( const KDSoapMessage &fault )
{
    Q_ASSERT( mStage == GetEntry );

    kWarning() << "Error when getting remote version:" << fault.faultAsString();

    // the item has been added we just don't have a server side datetime
    q->emitResult();
}

CreateEntryJob::CreateEntryJob( const Akonadi::Item &item, SugarSession *session, QObject *parent )
    : SugarJob( session, parent ), d( new Private( this, item ) )
{
    connect( soap(), SIGNAL( set_entryDone( TNS__Set_entry_result ) ),
             this,  SLOT( setEntryDone( TNS__Set_entry_result ) ) );
    connect( soap(), SIGNAL( set_entryError( KDSoapMessage ) ),
             this,  SLOT( setEntryError( KDSoapMessage ) ) );
    connect( soap(), SIGNAL( get_entryDone( TNS__Get_entry_result ) ),
             this,  SLOT( getEntryDone( TNS__Get_entry_result ) ) );
    connect( soap(), SIGNAL( get_entryError( KDSoapMessage ) ),
             this,  SLOT( getEntryError( KDSoapMessage ) ) );
}

CreateEntryJob::~CreateEntryJob()
{
    delete d;
}

void CreateEntryJob::setModule( ModuleHandler *handler )
{
    d->mHandler = handler;
}

Item CreateEntryJob::item() const
{
    return d->mItem;
}

void CreateEntryJob::startSugarTask()
{
    Q_ASSERT( d->mItem.isValid() );
    Q_ASSERT( d->mHandler != 0 );

    d->mStage = Private::CreateEntry;

    if ( !d->mHandler->setEntry( d->mItem ) ) {
        setError( SugarJob::InvalidContextError );
        setErrorText( i18nc( "@info:status", "Attempting to add malformed item to folder %1",
                             d->mHandler->moduleName() ) );
        emitResult();
    }
}

#include "createentryjob.moc"