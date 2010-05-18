#include "sugarcrmresource.h"

#include "contactshandler.h"
#include "moduledebuginterface.h"
#include "resourcedebuginterface.h"
#include "settings.h"
#include "settingsadaptor.h"
#include "sugarconfigdialog.h"
#include "sugarsoap.h"

#include <akonadi/changerecorder.h>
#include <akonadi/collection.h>
#include <akonadi/itemfetchscope.h>

#include <kabc/addressee.h>

#include <KLocale>
#include <KWindowSystem>

#include <KDSoapMessage.h>

#include <QtDBus/QDBusConnection>

using namespace Akonadi;

static QString endPointFromHostString( const QString &host )
{
    KUrl url( host );
    url.setFileName( QLatin1String( "soap.php" ) );
    url.setQuery( QString() );

    return url.url();
}

static QString nameFromHostString( const QString &host )
{
    KUrl url( host );

    return i18nc("@title user visible resource identifier, including host name", "SugarCRM on %1", url.host() );
}

SugarCRMResource::SugarCRMResource( const QString &id )
    : ResourceBase( id ),
      mSoap( new Sugarsoap ),
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

    connectSoapProxy();

    mSoap->setEndPoint( endPointFromHostString( Settings::self()->host() ) );
    setName( Settings::self()->user() + QLatin1Char( '@' ) + nameFromHostString( Settings::self()->host() ) );
}

SugarCRMResource::~SugarCRMResource()
{
    qDeleteAll( *mModuleHandlers );
    delete mModuleHandlers;
    delete mSoap;
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

    bool newLogin = false;

    // change of host requires new instance of the SOAP client as its setEndPoint() method
    // does not change the internal client interface which actually handles the communication
    if ( host != Settings::self()->host() ) {
        if ( !mSessionId.isEmpty() ) {
            mSoap->logout( mSessionId );
            mSessionId = QString();
        }

        mSoap->disconnect();
        mSoap->deleteLater();

        setName( user + QLatin1Char( '@' ) + nameFromHostString( host ) );

        mSoap = new Sugarsoap;
        mSoap->setEndPoint( endPointFromHostString( host ) );
        connectSoapProxy();

        newLogin = true;
    }

    if ( user != Settings::self()->user() || password != Settings::self()->password() ) {
        if ( !mSessionId.isEmpty() ) {
            mSoap->logout( mSessionId );
            mSessionId = QString();
        }

        setName( user + QLatin1Char( '@' ) + nameFromHostString( host ) );

        newLogin = true;
    }

    Settings::self()->setHost( host );
    Settings::self()->setUser( user );
    Settings::self()->setPassword( password );
    Settings::self()->writeConfig();

    emit configurationDialogAccepted();

    if ( newLogin && isOnline() ) {
        doLogin();
    }
}

void SugarCRMResource::aboutToQuit()
{
    if ( !mSessionId.isEmpty() ) {
        // just a curtesy to the server
        mSoap->asyncLogout( mSessionId );
    }
}

void SugarCRMResource::doSetOnline( bool online )
{
    ResourceBase::doSetOnline( online );

    if ( online ) {
        if ( Settings::self()->host().isEmpty() ) {
            const QString message = i18nc( "@status", "No server configured" );
            status( Broken, message );
            error( message );
        } else if ( Settings::self()->user().isEmpty() ) {
            const QString message = i18nc( "@status", "No user name configured" );
            status( Broken, message );
            error( message );
        } else {
            doLogin();
        }
    }
}

void SugarCRMResource::doLogin()
{
    const QString username = Settings::self()->user();
    const QString password = Settings::self()->password();

    // TODO krake: SugarCRM docs say that login wants an MD5 hash but it only works with clear text
    // might depend on SugarCRM configuration
    // would have the additional advantage of not having to save the password in clear text

    //const QByteArray passwordHash = QCryptographicHash::hash( password.toUtf8(), QCryptographicHash::Md5 );
    const QByteArray passwordHash = password.toUtf8();

    TNS__User_auth userAuth;
    userAuth.setUser_name( username );
    userAuth.setPassword( QString::fromAscii( passwordHash ) );
    userAuth.setVersion( QLatin1String( ".01"  ) );

    mSessionId = QString();

    // results handled by slots loginDone() and loginError()
    mSoap->asyncLogin( userAuth, QLatin1String( "SugarClient" ) );
}

void SugarCRMResource::itemAdded( const Akonadi::Item &item, const Akonadi::Collection &collection )
{
    // TODO check if mSessionId is valid?

    QString message;

    // find the handler for the module represented by the given collection and let it
    // perform the respective "set entry" operation
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind( collection.remoteId() );
    if ( moduleIt != mModuleHandlers->constEnd() ) {
        // save item so we can reference it in the result slots
        mPendingItem = item;

        // results handled by slots setEntryDone() and setEntryError()
        if ( !moduleIt.value()->setEntry( item, mSoap, mSessionId ) ) {
            mPendingItem = Item();
            message = i18nc( "@status", "Attempting to add malformed item to folder %1", collection.name() );
        }
    } else {
        message = i18nc( "@status", "Cannot add items to folder %1", collection.name() );
    }

    if ( message.isEmpty() ) {
        status( Running );
    } else {
        status( Broken, message );
        error( message );
        cancelTask( message );
    }
}

void SugarCRMResource::itemChanged( const Akonadi::Item &item, const QSet<QByteArray> &parts )
{
    // TODO maybe we can use parts to get only a subset of fields in ModuleHandler::setEntry()
    Q_UNUSED( parts );

    // TODO check if mSessionId is valid?
    QString message;

    // find the handler for the module represented by the given collection and let it
    // perform the respective "set entry" operation
    const Collection collection = item.parentCollection();
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind( collection.remoteId() );
    if ( moduleIt != mModuleHandlers->constEnd() ) {
        // save item so we can reference it in the result slots
        mPendingItem = item;

        // results handled by slots setEntryDone() and setEntryError()
        if ( !moduleIt.value()->setEntry( item, mSoap, mSessionId ) ) {
            mPendingItem = Item();
            message = i18nc( "@status", "Attempting to modify a malformed item in folder %1", collection.name() );
        }
    } else {
        message = i18nc( "@status", "Cannot modify items in folder %1", collection.name() );
    }

    if ( message.isEmpty() ) {
        status( Running );
    } else {
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

    // TODO check if mSessionId is valid?

    // delete just required identifier and "deleted" field
    // no need for type specific code
    TNS__Name_value idField;
    idField.setName( QLatin1String( "id" ) );
    idField.setValue( item.remoteId() );

    TNS__Name_value deletedField;
    deletedField.setName( QLatin1String( "deleted" ) );
    deletedField.setValue( QLatin1String( "1" ) );

    TNS__Name_value_list valueList;
    valueList.setItems( QList<TNS__Name_value>() << idField << deletedField );

    // save item so we can reference it in the result slots
    mPendingItem = item;

    // results handled by slots getEntryDone() and getEntryError()
    mSoap->asyncSet_entry( mSessionId, collection.remoteId(), valueList );

    status( Running );
}

void SugarCRMResource::connectSoapProxy()
{
    Q_ASSERT( mSoap != 0 );

    connect( mSoap, SIGNAL( loginDone( TNS__Set_entry_result ) ), this, SLOT( loginDone( TNS__Set_entry_result ) ) );
    connect( mSoap, SIGNAL( loginError( KDSoapMessage ) ), this, SLOT( loginError( KDSoapMessage ) ) );

    connect( mSoap, SIGNAL( get_available_modulesDone( TNS__Module_list ) ),
             this,  SLOT( getAvailableModulesDone( TNS__Module_list ) ) );
    connect( mSoap, SIGNAL( get_available_modulesError( KDSoapMessage ) ),
             this,  SLOT( getAvailableModulesError( KDSoapMessage ) ) );

    connect( mSoap, SIGNAL( get_entry_listDone( TNS__Get_entry_list_result ) ),
             this,  SLOT( getEntryListDone( TNS__Get_entry_list_result ) ) );
    connect( mSoap, SIGNAL( get_entry_listError( KDSoapMessage ) ),
             this,  SLOT( getEntryListError( KDSoapMessage ) ) );

    connect( mSoap, SIGNAL( set_entryDone( TNS__Set_entry_result ) ),
             this,  SLOT( setEntryDone( TNS__Set_entry_result ) ) );
    connect( mSoap, SIGNAL( set_entryError( KDSoapMessage ) ),
             this,  SLOT( setEntryError( KDSoapMessage ) ) );
}

void SugarCRMResource::retrieveCollections()
{
    // TODO could attempt automatic login
    if ( mSessionId.isEmpty() ) {
        QString message;
        if ( Settings::host().isEmpty() ) {
            message = i18nc( "@status", "No server configured" );
        } else if ( Settings::self()->user().isEmpty() ) {
            message = i18nc( "@status", "No user name configured" );
        } else {
            message = i18nc( "@status", "Unable to login to %1", Settings::host() );
        }

        status( Broken, message );
        error( message );
        cancelTask( message );
    } else {
        status( Running, i18nc( "@status", "Retrieving folders" ) );
        // results handled by slots getAvailableModulesDone() and getAvailableModulesError()
        mSoap->asyncGet_available_modules( mSessionId );
    }
}

void SugarCRMResource::retrieveItems( const Akonadi::Collection &collection )
{
    if ( collection.parentCollection() == Collection::root() ) {
        itemsRetrieved( Item::List() );
        return;
    }

    // TODO could attempt automatic login
    if ( mSessionId.isEmpty() ) {
        QString message;
        if ( Settings::host().isEmpty() ) {
            message = i18nc( "@status", "No server configured" );
        } else if ( Settings::self()->user().isEmpty() ) {
            message = i18nc( "@status", "No user name configured" );
        } else {
            message = i18nc( "@status", "Unable to login to %1", Settings::host() );
        }

        status( Broken, message );
        error( message );
        cancelTask( message );
    } else {
        // find the handler for the module represented by the given collection and let it
        // perform the respective "list entries" operation
        ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind( collection.remoteId() );
        if ( moduleIt != mModuleHandlers->constEnd() ) {
            status( Running, i18nc( "@status", "Retrieving contents of folder %1", collection.name() ) );

            // getting items in batches
            setItemStreamingEnabled( true );

            // results handled by slots getEntryListDone() and getEntryListError()
            moduleIt.value()->listEntries( 0, mSoap, mSessionId );
        } else {
            kDebug() << "No module handler for collection" << collection;
            itemsRetrieved( Item::List() );
        }
    }
}

bool SugarCRMResource::retrieveItem( const Akonadi::Item &item, const QSet<QByteArray> &parts )
{
    Q_UNUSED( item );
    Q_UNUSED( parts );

    // TODO not implemented yet
    // retrieveItems() provides full items anyway so this one is not called
    // (no need for getting additional data)
    // should be implemented for consistency though

    return true;
}

void SugarCRMResource::loginDone( const TNS__Set_entry_result &callResult )
{
    QString message;
    mSessionId = QString();

    const TNS__Error_value errorValue = callResult.error();
    if ( !errorValue.number().isEmpty() && errorValue.number() != QLatin1String( "0" ) ) {
        kError() << "SOAP Error: number=" << errorValue.number()
                 << ", name=" << errorValue.name() << ", description=" << errorValue.description();

        message = errorValue.description();
    } else {
        const QString sessionId = callResult.id();
        if ( sessionId.isEmpty() ) {
            message = i18nc( "@status", "Login failed: server returned an empty session identifier" );
        } else if ( mSessionId == QLatin1String( "-1" ) ) {
            message = i18nc( "@status", "Login failed: server returned an invalid session identifier" );
        } else {
            mSessionId = sessionId;
            kDebug() << "Login succeeded: sessionId=" << mSessionId;
        }
    }

    if ( message.isEmpty() ) {
        status( Idle );

        synchronizeCollectionTree();
    } else {
        status( Broken, message );
        error( message );
    }
}

void SugarCRMResource::loginError( const KDSoapMessage &fault )
{
    mSessionId = QString();

    const QString message = fault.faultAsString();

    status( Broken, message );
    error( message );
}

void SugarCRMResource::getAvailableModulesDone( const TNS__Module_list &callResult )
{
    QString message;
    Collection::List collections;

    const TNS__Error_value errorValue = callResult.error();
    if ( !errorValue.number().isEmpty() && errorValue.number() != QLatin1String( "0" ) ) {
        kError() << "SOAP Error: number=" << errorValue.number()
                 << ", name=" << errorValue.name() << ", description=" << errorValue.description();

        message = errorValue.description();
    } else {
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
        const TNS__Select_fields moduleNames = callResult.modules();
        Q_FOREACH( const QString &module, moduleNames.items() ) {
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
                } else {
                    //kDebug() << "No module handler for" << module;
                    continue;
                }
                mModuleHandlers->insert( module, handler );

                collection = handler->collection();
            }

            collection.setParentCollection( topLevelCollection );
            collections << collection;
        }
    }

    if ( message.isEmpty() ) {
        collectionsRetrieved( collections );
        status( Idle );
    } else {
        status( Broken, message );
        error( message );
        cancelTask( message );
    }
}

void SugarCRMResource::getAvailableModulesError( const KDSoapMessage &fault )
{
    const QString message = fault.faultAsString();

    status( Broken, message );
    error( message );
    cancelTask( message );
}

void SugarCRMResource::getEntryListDone( const TNS__Get_entry_list_result &callResult )
{
    const Collection collection = currentCollection();

    QString message;
    Item::List items;

    const TNS__Error_value errorValue = callResult.error();
    if ( !errorValue.number().isEmpty() && errorValue.number() != QLatin1String( "0" ) ) {
        kError() << "SOAP Error: number=" << errorValue.number()
                 << ", name=" << errorValue.name() << ", description=" << errorValue.description();

        message = errorValue.description();
    } else {
        // find the handler for the module represented by the given collection and let it
        // "deserialize" the SOAP response into an item payload and perform the respective "list entries" operation
        ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind( collection.remoteId() );
        if ( moduleIt != mModuleHandlers->constEnd() ) {
            if ( callResult.result_count() > 0 ) {
                itemsRetrieved( moduleIt.value()->itemsFromListEntriesResponse( callResult.entry_list(), collection ) );

                moduleIt.value()->listEntries( callResult.next_offset(), mSoap, mSessionId );
            } else {
                status( Idle );
                itemsRetrievalDone();
            }
        }
    }

    if ( !message.isEmpty() ) {
        status( Broken, message );
        error( message );
        cancelTask( message );
    }
}

void SugarCRMResource::getEntryListError( const KDSoapMessage &fault )
{
    const QString message = fault.faultAsString();

    status( Broken, message );
    error( message );
    cancelTask( message );
}

void SugarCRMResource::setEntryDone( const TNS__Set_entry_result &callResult )
{
    const TNS__Error_value errorValue = callResult.error();
    if ( !errorValue.number().isEmpty() && errorValue.number() != QLatin1String( "0" ) ) {
        kError() << "SOAP Error: number=" << errorValue.number()
                 << ", name=" << errorValue.name() << ", description=" << errorValue.description();

        const QString message = errorValue.description();

        status( Broken, message );
        error( message );
        cancelTask( message );
    } else {
        status( Idle );

        // setting the remoteId is technically only required for the handling the result of
        // itemAdded() so Akonadi knows which identifier was assigned to the item on the server.
        mPendingItem.setRemoteId( callResult.id() );
        changeCommitted( mPendingItem );
    }

    mPendingItem = Item();
}

void SugarCRMResource::setEntryError( const KDSoapMessage &fault )
{
    const QString message = fault.faultAsString();

    status( Broken, message );
    error( message );
    cancelTask( message );

    mPendingItem = Item();
}

AKONADI_RESOURCE_MAIN( SugarCRMResource )

#include "sugarcrmresource.moc"
