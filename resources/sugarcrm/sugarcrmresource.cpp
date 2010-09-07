#include "sugarcrmresource.h"

#include "accountshandler.h"
#include "campaignshandler.h"
#include "contactshandler.h"
#include "createentryjob.h"
#include "deleteentryjob.h"
#include "leadshandler.h"
#include "listentriesjob.h"
#include "listmodulesjob.h"
#include "loginjob.h"
#include "moduledebuginterface.h"
#include "opportunitieshandler.h"
#include "resourcedebuginterface.h"
#include "settings.h"
#include "settingsadaptor.h"
#include "sugarconfigdialog.h"
#include "sugarsession.h"
#include "updateentryjob.h"

#include <akonadi/changerecorder.h>
#include <akonadi/collection.h>
#include <akonadi/itemfetchscope.h>

#include <kabc/addressee.h>

#include <KLocale>
#include <KWindowSystem>

#include <KDSoapMessage.h>

#include <QtDBus/QDBusConnection>

using namespace Akonadi;

static QString nameFromHostString( const QString &host )
{
    KUrl url( host );

    return i18nc("@title user visible resource identifier, including host name", "SugarCRM on %1", url.host() );
}

SugarCRMResource::SugarCRMResource( const QString &id )
    : ResourceBase( id ),
      mSession( new SugarSession( this ) ),
      mModuleHandlers( new ModuleHandlerHash )
{
    new SettingsAdaptor( Settings::self() );
    QDBusConnection::sessionBus().registerObject( QLatin1String( "/Settings" ),
                                                  Settings::self(),
                                                  QDBusConnection::ExportAdaptors );

    ResourceDebugInterface *debugInterface = new ResourceDebugInterface( this );
    QDBusConnection::sessionBus().registerObject( QLatin1String( "/CRMDebug" ),
                                                  debugInterface,
                                                  QDBusConnection::ExportScriptableSlots );

    setNeedsNetwork( true );

    // make sure itemAdded() and itemChanged() get the full item from Akonadi before being called
    changeRecorder()->itemFetchScope().fetchFullPayload();

    // make sure these call have the collection available as well
    changeRecorder()->fetchCollection( true );

    mSession->setSessionParameters( Settings::self()->user(), Settings::self()->password(),
                                    Settings::self()->host() );
    mSession->createSoapInterface();
}

SugarCRMResource::~SugarCRMResource()
{
    qDeleteAll( *mModuleHandlers );
    delete mModuleHandlers;
}

void SugarCRMResource::configure( WId windowId )
{
    SugarConfigDialog dialog( Settings::self() );

    // make sure we are seen as a child window of the caller's window
    // otherwise focus stealing prevention might put us behind it
    KWindowSystem::setMainWindow( &dialog, windowId );

    int result = dialog.exec();

    if ( result == QDialog::Rejected ) {
        emit configurationDialogRejected();
        return;
    }

    const QString host = dialog.host();
    const QString user = dialog.user();
    const QString password = dialog.password();

    SugarSession::RequiredAction action = mSession->setSessionParameters( user, password, host );
    switch ( action ) {
        case SugarSession::None:
            break;

        case SugarSession::NewLogin:
            mSession->createSoapInterface();
            // fall through
        case SugarSession::ReLogin:
            setName( user + QLatin1Char( '@' ) + nameFromHostString( host ) );
            if ( isOnline() ) {
                SugarJob *job = new LoginJob( mSession, this );
                connect( job, SIGNAL( result( KJob* ) ), this, SLOT( explicitLoginResult( KJob* ) ) );
                job->start();
            }
            break;
    }

    Settings::self()->setHost( host );
    Settings::self()->setUser( user );
    Settings::self()->setPassword( password );
    Settings::self()->writeConfig();

    emit configurationDialogAccepted();
}

void SugarCRMResource::aboutToQuit()
{
    // just a curtesy to the server
    mSession->logout();
}

void SugarCRMResource::doSetOnline( bool online )
{
    ResourceBase::doSetOnline( online );

    if ( online ) {
        if ( Settings::self()->host().isEmpty() ) {
            const QString message = i18nc( "@info:status", "No server configured" );
            status( Broken, message );
            error( message );
        } else if ( Settings::self()->user().isEmpty() ) {
            const QString message = i18nc( "@info:status", "No user name configured" );
            status( Broken, message );
            error( message );
        } else {
            SugarJob *job = new LoginJob( mSession, this );
            connect( job, SIGNAL( result( KJob* ) ), this, SLOT( explicitLoginResult( KJob* ) ) );
            job->start();
        }
    }
}

void SugarCRMResource::itemAdded( const Akonadi::Item &item, const Akonadi::Collection &collection )
{
    // find the handler for the module represented by the given collection and let it
    // perform the respective "set entry" operation
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind( collection.remoteId() );
    if ( moduleIt != mModuleHandlers->constEnd() ) {
        status( Running );

        CreateEntryJob *job = new CreateEntryJob( item, mSession, this );
        job->setModule( *moduleIt );
        connect( job, SIGNAL( result( KJob* ) ), this, SLOT( createEntryResult( KJob* ) ) );
        job->start();
    } else {
        const QString message = i18nc( "@info:status", "Cannot add items to folder %1",
                                       collection.name() );
        kWarning() << message;

        status( Broken, message );
        error( message );
        cancelTask( message );
    }
}

void SugarCRMResource::itemChanged( const Akonadi::Item &item, const QSet<QByteArray> &parts )
{
    // TODO maybe we can use parts to get only a subset of fields in ModuleHandler::setEntry()
    Q_UNUSED( parts );

    // find the handler for the module represented by the given collection and let it
    // perform the respective "set entry" operation
    const Collection collection = item.parentCollection();
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind( collection.remoteId() );
    if ( moduleIt != mModuleHandlers->constEnd() ) {
        status( Running );

        UpdateEntryJob *job = new UpdateEntryJob( item, mSession, this );
        job->setModule( *moduleIt );
        connect( job, SIGNAL( conflictDetected( Akonadi::Item, Akonadi::Item ) ),
                 this, SLOT( updateConflict( Akonadi::Item, Akonadi::Item ) ) );
        connect( job, SIGNAL( result( KJob* ) ), this, SLOT( updateEntryResult( KJob* ) ) );
        job->start();
    } else {
        const QString message = i18nc( "@info:status", "Cannot modify items in folder %1",
                                       collection.name() );
        kWarning() << message;

        status( Broken, message );
        error( message );
        cancelTask( message );
    }
}

void SugarCRMResource::itemRemoved( const Akonadi::Item &item )
{
    const Collection collection = item.parentCollection();

    // not uploaded yet?
    if ( item.remoteId().isEmpty() || collection.remoteId().isEmpty() ) {
        changeCommitted( item );
        return;
    }

    status( Running );

    SugarJob *job = new DeleteEntryJob( item, mSession, this );
    connect( job, SIGNAL( result( KJob* ) ), this, SLOT( deleteEntryResult( KJob* ) ) );
    job->start();
}

void SugarCRMResource::retrieveCollections()
{
    status( Running, i18nc( "@info:status", "Retrieving folders" ) );

    SugarJob *job = new ListModulesJob( mSession, this );
    connect( job, SIGNAL( result( KJob* ) ), this, SLOT( listModulesResult( KJob* ) ) );
    job->start();
}

void SugarCRMResource::retrieveItems( const Akonadi::Collection &collection )
{
    if ( collection.parentCollection() == Collection::root() ) {
        itemsRetrieved( Item::List() );
        return;
    }

    // find the handler for the module represented by the given collection and let it
    // perform the respective "list entries" operation
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind( collection.remoteId() );
    if ( moduleIt != mModuleHandlers->constEnd() ) {
        status( Running, i18nc( "@info:status", "Retrieving contents of folder %1",
                                collection.name() ) );

        // getting items in batches
        setItemStreamingEnabled( true );

        ListEntriesJob *job = new ListEntriesJob( collection, mSession, this );
        job->setModule( *moduleIt );
        connect( job, SIGNAL( itemsReceived( Akonadi::Item::List ) ),
                 this, SLOT( itemsReceived( Akonadi::Item::List ) ) );
        connect( job, SIGNAL( result( KJob* ) ), this, SLOT( listEntriesResult( KJob* ) ) );
        job->start();
    } else {
        kDebug() << "No module handler for collection" << collection;
        itemsRetrieved( Item::List() );
    }
}

bool SugarCRMResource::retrieveItem( const Akonadi::Item &item, const QSet<QByteArray> &parts )
{
    Q_UNUSED( parts );

    // TODO not implemented yet
    // retrieveItems() provides full items anyway so this one is not called
    // (no need for getting additional data)
    // should be implemented for consistency though
    kError() << "Akonadi requesting item, module handler should have delivered full items already";
    kError() << "item.remoteId=" << item.remoteId()
             << "item.mimeType=" << item.mimeType()
             << "parentCollection.remoteId=" << item.parentCollection().remoteId();
    return false;
}

void SugarCRMResource::explicitLoginResult( KJob *job )
{
    if ( job->error() != 0 ) {
        QString message = job->errorText();
        kWarning() << "error=" << job->error() << ":" << message;

        if ( Settings::host().isEmpty() ) {
            message = i18nc( "@info:status", "No server configured" );
        } else if ( Settings::self()->user().isEmpty() ) {
            message = i18nc( "@info:status", "No user name configured" );
        } else {
            message = i18nc( "@info:status", "Unable to login %1 to %2",
                             Settings::self()->user(), Settings::self()->host(), message );
        }

        status( Broken, message );
        error( message );
        return;
    }

    status( Idle );
    synchronizeCollectionTree();
}

void SugarCRMResource::listModulesResult( KJob *job )
{
    if ( job->error() != 0 ) {
        const QString message = job->errorText();
        kWarning() << "error=" << job->error() << ":" << message;

        status( Broken, message );
        error( message );
        cancelTask( message );
        return;
    }

    ListModulesJob *listJob = qobject_cast<ListModulesJob*>( job );
    Q_ASSERT( listJob != 0 );

    Collection::List collections;

    Collection topLevelCollection;
    topLevelCollection.setRemoteId( identifier() );
    topLevelCollection.setName( name() );
    topLevelCollection.setParentCollection( Collection::root() );

    // Our top level collection only contains other collections (no items) and cannot be
    // modified by clients
    topLevelCollection.setContentMimeTypes( QStringList() << Collection::mimeType() );
    topLevelCollection.setRights( Collection::ReadOnly );

    collections << topLevelCollection;

    mAvailableModules.clear();
    Q_FOREACH( const QString &module, listJob->modules() ) {
        mAvailableModules << module;

        ModuleDebugInterface *debugInterface = new ModuleDebugInterface( module, this );
        QDBusConnection::sessionBus().registerObject( QLatin1String( "/CRMDebug/modules/" ) + module,
                                                    debugInterface,
                                                    QDBusConnection::ExportScriptableSlots );

        Collection collection;

        // check if we have a corresponding module handler already
        // if not see if we can create one
        ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind( module );
        if ( moduleIt != mModuleHandlers->constEnd() ) {
            collection = moduleIt.value()->collection();
        } else {
            ModuleHandler* handler = 0;
            if ( module == QLatin1String( "Contacts" ) ) {
                handler = new ContactsHandler;
            } else if ( module == QLatin1String( "Accounts" ) ) {
                handler = new AccountsHandler;
            } else if ( module == QLatin1String( "Opportunities" ) ) {
                handler = new OpportunitiesHandler;
            } else if ( module == QLatin1String( "Leads" ) ) {
                handler = new LeadsHandler;
            } else if ( module == QLatin1String( "Campaigns" ) ) {
                handler = new CampaignsHandler;
            }
            else {
                //kDebug() << "No module handler for" << module;
                continue;
            }
            mModuleHandlers->insert( module, handler );

            collection = handler->collection();
        }

        collection.setParentCollection( topLevelCollection );
        collections << collection;
    }

    collectionsRetrieved( collections );
    status( Idle );
}

void SugarCRMResource::itemsReceived( const Akonadi::Item::List &items )
{
    itemsRetrieved( items );
}

void SugarCRMResource::listEntriesResult( KJob *job )
{
    if ( job->error() != 0 ) {
        const QString message = job->errorText();
        kWarning() << "error=" << job->error() << ":" << message;

        status( Broken, message );
        error( message );
        cancelTask( message );
        return;
    }

    itemsRetrievalDone();
    status( Idle );
}

void SugarCRMResource::createEntryResult( KJob *job )
{
    if ( job->error() != 0 ) {
        const QString message = job->errorText();
        kWarning() << "error=" << job->error() << ":" << message;

        status( Broken, message );
        error( message );
        cancelTask( message );
        return;
    }

    CreateEntryJob *createJob = qobject_cast<CreateEntryJob*>( job );
    Q_ASSERT( createJob != 0 );

    changeCommitted( createJob->item() );
    status( Idle );
}

void SugarCRMResource::deleteEntryResult( KJob *job )
{
    if ( job->error() != 0 ) {
        const QString message = job->errorText();
        kWarning() << "error=" << job->error() << ":" << message;

        status( Broken, message );
        error( message );
        cancelTask( message );
        return;
    }

    DeleteEntryJob *deleteJob = qobject_cast<DeleteEntryJob*>( job );
    Q_ASSERT( deleteJob != 0 );

    changeCommitted( deleteJob->item() );
    status( Idle );
}

void SugarCRMResource::updateConflict( const Item &localItem, const Item &remoteItem )
{
    // TODO
}

void SugarCRMResource::updateEntryResult( KJob *job )
{
    if ( job->error() != 0 ) {
        const QString message = job->errorText();
        kWarning() << "error=" << job->error() << ":" << message;

        status( Broken, message );
        error( message );
        cancelTask( message );
        return;
    }

    UpdateEntryJob *updateJob = qobject_cast<UpdateEntryJob*>( job );
    Q_ASSERT( updateJob != 0 );

    changeCommitted( updateJob->item() );
    status( Idle );
}

AKONADI_RESOURCE_MAIN( SugarCRMResource )

#include "sugarcrmresource.moc"
