#include "conflicthandler.h"

#include "conflictresolvedialog.h"

#include <akonadi/collection.h>
#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>
#include <akonadi/session.h>

#include <KLocale>
#include <KWindowSystem>

using namespace Akonadi;

class ConflictHandler::Private
{
    ConflictHandler *const q;
public:
    Private( ConflictHandler *parent, ConflictType type )
        : q( parent ), mType( type ), mDiffInterface( 0 ), mWindowId( 0 ),
          mSession( new Session( Session::defaultSession()->sessionId() + "-conflicthandler", parent ) )
    {
    }

public:
    ConflictType mType;

    Item mLocalItem;
    Item mRemoteItem;

    DifferencesAlgorithmInterface *mDiffInterface;
    WId mWindowId;
    QString mName;

    Session *mSession;

public:
    void resolve();
    void createDuplicate( const Item &item );

public: // slots
    void duplicateLocalItemResult( KJob *job );
};

void ConflictHandler::Private::resolve()
{
    ConflictResolveDialog dialog;

    if ( mWindowId != 0 ) {
        // if we have a "parent" window id, use it
        // otherwise focus stealing prevention might put us behind it
        KWindowSystem::setMainWindow( &dialog, mWindowId );
    }

    if ( mName.isEmpty() ) {
        dialog.setWindowTitle( i18nc( "@title:window", "Conflict Resolution" ) );
    } else {
        dialog.setWindowTitle( i18nc( "@title:window", "%1: Conflict Resolution", mName ) );
    }

    dialog.setConflictingItems( mLocalItem, mRemoteItem );
    dialog.setDifferencesInterface( mDiffInterface );

    dialog.exec();

    ConflictHandler::ResolveStrategy solution = dialog.resolveStrategy();
    switch ( solution ) {
        case ConflictHandler::UseLocalItem:
            // update the local items remote revision and try again
            mLocalItem.setRemoteRevision( mRemoteItem.remoteRevision() );
            emit q->updateOnBackend( mLocalItem );
            break;

        case ConflictHandler::UseOtherItem: {
            // use remote item to acknowledge change
            mRemoteItem.setId( mLocalItem.id() );
            mRemoteItem.setRevision( mLocalItem.revision() );
            emit q->commitChange( mRemoteItem );

            // commit does not update payload, so we modify as well
            ItemModifyJob *modifyJob = new ItemModifyJob( mRemoteItem, q );
            modifyJob->disableRevisionCheck();
            break;
        }
        case ConflictHandler::UseBothItems: {
            // use remote item to acknowledge change and duplicate local item
            mRemoteItem.setId( mLocalItem.id() );
            mRemoteItem.setRevision( mLocalItem.revision() );
            emit q->commitChange( mRemoteItem );

            // commit does not update payload, so we modify as well
            ItemModifyJob *modifyJob = new ItemModifyJob( mRemoteItem, q );
            modifyJob->disableRevisionCheck();

            createDuplicate( mLocalItem );
            break;
        }
    }
}

void ConflictHandler::Private::createDuplicate( const Item &item )
{
    kDebug() << "Creating duplicate of item: id=" << item.id() << ", remoteId=" << item.remoteId();
    Item duplicate( item );
    duplicate.setId( -1 );
    duplicate.setRemoteId( QString() );
    duplicate.setRemoteRevision( QString() );

    ItemCreateJob *job = new ItemCreateJob( duplicate, item.parentCollection(), mSession );
    connect( job, SIGNAL( result( KJob* ) ), q, SLOT( duplicateLocalItemResult( KJob* ) ) );
}

void ConflictHandler::Private::duplicateLocalItemResult( KJob *job )
{
    ItemCreateJob *createJob = qobject_cast<ItemCreateJob*>( job );
    Q_ASSERT( createJob != 0 );

    const Item item = createJob->item();

    if ( createJob->error() != 0 ) {
        kWarning() << "Duplicating local item" << item.id()
                   << ", collection" << item.parentCollection().name()
                   << "for conflict resolution failed on local create: "
                   << "error=" << createJob->error() << "message=" << createJob->errorText();
    } else {
        kDebug() << "Duplicating local item" << item.id()
                 << ", collection" << item.parentCollection().name()
                 << "for conflict resolution succeeded:";
    }
}

ConflictHandler::ConflictHandler( ConflictType type, QObject *parent )
    : QObject( parent ), d( new Private( this, type ) )
{
}

ConflictHandler::~ConflictHandler()
{
    delete d;
}

void ConflictHandler::setConflictingItems( const Item &changedItem, const Item &conflictingItem )
{
    d->mLocalItem = changedItem;
    d->mRemoteItem = conflictingItem;
}

void ConflictHandler::setDifferencesInterface( DifferencesAlgorithmInterface *interface )
{
    Q_ASSERT( interface != 0 );
    d->mDiffInterface = interface;
}

void ConflictHandler::setParentName( const QString &name )
{
    d->mName = name;
}

void ConflictHandler::setParentWindowId( WId windowId )
{
    d->mWindowId = windowId;
}

void ConflictHandler::start()
{
    Q_ASSERT( d->mType == BackendConflict );

    d->resolve();
}

#include "conflicthandler.moc"