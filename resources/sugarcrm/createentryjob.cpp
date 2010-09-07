#include "createentryjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"

#include <KDSoapMessage.h>

#include <akonadi/item.h>

#include <KDebug>
#include <KLocale>

#include <QStringList>

using namespace Akonadi;

class CreateEntryJob::Private
{
    CreateEntryJob *const q;

public:
    explicit Private( CreateEntryJob *parent, const Item &item )
        : q( parent ), mItem( item ), mHandler( 0 )
    {
    }

public:
    Item mItem;
    ModuleHandler *mHandler;

public: // slots
    void setEntryDone( const TNS__Set_entry_result &callResult );
    void setEntryError( const KDSoapMessage &fault );
};

void CreateEntryJob::Private::setEntryDone( const TNS__Set_entry_result &callResult )
{
    kDebug() << "Created entry" << callResult.id() << "in module" << mHandler->moduleName();
    mItem.setRemoteId( callResult.id() );

    q->emitResult();
}

void CreateEntryJob::Private::setEntryError( const KDSoapMessage &fault )
{
    if ( !q->handleLoginError( fault ) ) {
        kWarning() << "Create Entry Error:" << fault.faultAsString();

        q->setError( SugarJob::SoapError );
        q->setErrorText( fault.faultAsString() );
        q->emitResult();
    }
}

CreateEntryJob::CreateEntryJob( const Akonadi::Item &item, SugarSession *session, QObject *parent )
    : SugarJob( session, parent ), d( new Private( this, item ) )
{
    connect( soap(), SIGNAL( set_entryDone( TNS__Set_entry_result ) ),
             this,  SLOT( setEntryDone( TNS__Set_entry_result ) ) );
    connect( soap(), SIGNAL( set_entryError( KDSoapMessage ) ),
             this,  SLOT( setEntryError( KDSoapMessage ) ) );
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

    if ( !d->mHandler->setEntry( d->mItem, soap(), sessionId() ) ) {
        setError( SugarJob::SoapError ); // TODO should be different error code
        setErrorText( i18nc( "@info:status", "Attempting to add malformed item to folder %1",
                             d->mHandler->moduleName() ) );
        emitResult();
    }
}

#include "createentryjob.moc"