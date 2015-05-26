/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "salesforceresource.h"

#include "salesforcecontactshandler.h"
#include "moduledebuginterface.h"
#include "resourcedebuginterface.h"
#include "settings.h"
#include "settingsadaptor.h"
#include "salesforceconfigdialog.h"
#include "salesforcesoap.h"

#include <akonadi/changerecorder.h>
#include <akonadi/collection.h>
#include <akonadi/itemfetchscope.h>

#include <KLocale>
#include <KWindowSystem>

#include <KDSoapClient/KDSoapMessage.h>

#include <QtDBus/QDBusConnection>

using namespace Akonadi;

#if 0
static QString endPointFromHostString(const QString &host)
{
    KUrl url(host);
    url.setFileName(QLatin1String("soap.php"));
    url.setQuery(QString());

    return url.url();
}
#endif

SalesforceResource::SalesforceResource(const QString &id)
    : ResourceBase(id),
      mSoap(new SforceService),
      mModuleHandlers(new ModuleHandlerHash)
{
    new SettingsAdaptor(Settings::self());
    QDBusConnection::sessionBus().registerObject(QLatin1String("/Settings"),
            Settings::self(), QDBusConnection::ExportAdaptors);

    ResourceDebugInterface *debugInterface = new ResourceDebugInterface(this);
    QDBusConnection::sessionBus().registerObject(QLatin1String("/CRMDebug"),
            debugInterface,
            QDBusConnection::ExportScriptableSlots);

    setNeedsNetwork(true);

    // make sure itemAdded() and itemChanged() get the full item from Akonadi before being called
    changeRecorder()->itemFetchScope().fetchFullPayload();

    // make sure these call have the collection available as well
    changeRecorder()->fetchCollection(true);

    connectSoapProxy();

#if 0
    mSoap->setEndPoint(endPointFromHostString(Settings::self()->host()));
#endif
}

SalesforceResource::~SalesforceResource()
{
    qDeleteAll(*mModuleHandlers);
    delete mModuleHandlers;
    delete mSoap;
}

void SalesforceResource::configure(WId windowId)
{
    SalesforceConfigDialog dialog(Settings::self(), name());

    // make sure we are seen as a child window of the caller's window
    // otherwise focus stealing prevention might put us behind it
    KWindowSystem::setMainWindow(&dialog, windowId);

    int result = dialog.exec();

    if (result == QDialog::Rejected) {
        emit configurationDialogRejected();
        return;
    }

    const QString accountName = dialog.accountName();
    setName(accountName);

    const QString host = dialog.host();
    const QString user = dialog.user();
    const QString password = dialog.password();

    bool newLogin = false;

    // change of host requires new instance of the SOAP client as its setEndPoint() method
    // does not change the internal client interface which actually handles the communication
    if (host != Settings::self()->host()) {
        if (!mSessionId.isEmpty()) {
            mSoap->logout();
            mSessionId = QString();
        }

        mSoap->disconnect();
        mSoap->deleteLater();

        mSoap = new SforceService;
#if 0
        mSoap->setEndPoint(endPointFromHostString(host));
#endif
        connectSoapProxy();

        newLogin = true;
    }

    if (user != Settings::self()->user() || password != Settings::self()->password()) {
        if (!mSessionId.isEmpty()) {
            mSoap->logout();
            mSessionId = QString();
        }

        newLogin = true;
    }

    Settings::self()->setHost(host);
    Settings::self()->setUser(user);
    Settings::self()->setPassword(password);
    Settings::self()->writeConfig();

    emit configurationDialogAccepted();

    if (newLogin && isOnline()) {
        doLogin();
    }
}

void SalesforceResource::aboutToQuit()
{
    if (!mSessionId.isEmpty()) {
        // just a curtesy to the server
        mSoap->asyncLogout();
    }
}

void SalesforceResource::doSetOnline(bool online)
{
    ResourceBase::doSetOnline(online);

    if (online) {
#if 0
        if (Settings::self()->host().isEmpty()) {
            const QString message = i18nc("@info:status", "No server configured");
            status(Broken, message);
            error(message);
        } else if (Settings::self()->user().isEmpty()) {
            const QString message = i18nc("@info:status", "No user name configured");
            status(Broken, message);
            error(message);
#else
        if (Settings::self()->user().isEmpty()) {
            const QString message = i18nc("@info:status", "No user name configured");
            status(Broken, message);
            error(message);
#endif
        } else {
            doLogin();
        }
    }
}

void SalesforceResource::doLogin()
{
    const QString username = Settings::self()->user();
    const QString password = Settings::self()->password();

    TNS__Login userAuth;
    userAuth.setUsername(username);
    userAuth.setPassword(password);

    mSessionId = QString();

    // results handled by slots loginDone() and loginError()
    mSoap->asyncLogin(userAuth);
}

void SalesforceResource::itemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection)
{
    // TODO check if mSessionId is valid?

    QString message;

    // find the handler for the module represented by the given collection and let it
    // perform the respective "set entry" operation
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(collection.remoteId());
    if (moduleIt != mModuleHandlers->constEnd()) {
        // save item so we can reference it in the result slots
        mPendingItem = item;

        // results handled by slots setEntryDone() and setEntryError()
        if (!moduleIt.value()->setEntry(item, mSoap)) {
            mPendingItem = Item();
            message = i18nc("@info:status", "Attempting to add malformed item to folder %1", collection.name());
        }
    } else {
        message = i18nc("@info:status", "Cannot add items to folder %1", collection.name());
    }

    if (message.isEmpty()) {
        status(Running);
    } else {
        status(Broken, message);
        error(message);
        cancelTask(message);
    }
}

void SalesforceResource::itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    // TODO maybe we can use parts to get only a subset of fields in ModuleHandler::setEntry()
    Q_UNUSED(parts);

    // TODO check if mSessionId is valid?
    QString message;

    // find the handler for the module represented by the given collection and let it
    // perform the respective "set entry" operation
    const Collection collection = item.parentCollection();
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(collection.remoteId());
    if (moduleIt != mModuleHandlers->constEnd()) {
        // save item so we can reference it in the result slots
        mPendingItem = item;

        // results handled by slots setEntryDone() and setEntryError()
        if (!moduleIt.value()->setEntry(item, mSoap)) {
            mPendingItem = Item();
            message = i18nc("@info:status", "Attempting to modify a malformed item in folder %1", collection.name());
        }
    } else {
        message = i18nc("@info:status", "Cannot modify items in folder %1", collection.name());
    }

    if (message.isEmpty()) {
        status(Running);
    } else {
        status(Broken, message);
        error(message);
        cancelTask(message);
    }
}

void SalesforceResource::itemRemoved(const Akonadi::Item &item)
{
    const Collection collection = item.parentCollection();

    // not uploaded yet?
    if (item.remoteId().isEmpty() || collection.remoteId().isEmpty()) {
        changeCommitted(item);
        return;
    }

    // TODO check if mSessionId is valid?

    // delete just required identifiers
    // no need for type specific code
    TNS__ID idField;
    idField.setValue(item.remoteId());

    TNS__Delete deleteParams;
    deleteParams.setIds(QList<TNS__ID>() << idField);

    // save item so we can reference it in the result slots
    mPendingItem = item;

    // results handled by slots getEntryDone() and getEntryError()
    mSoap->asyncDelete(deleteParams);

    status(Running);
}

void SalesforceResource::connectSoapProxy()
{
    Q_ASSERT(mSoap != 0);

    connect(mSoap, SIGNAL(loginDone(TNS__LoginResponse)), this, SLOT(loginDone(TNS__LoginResponse)));
    connect(mSoap, SIGNAL(loginError(KDSoapMessage)), this, SLOT(loginError(KDSoapMessage)));

    connect(mSoap, SIGNAL(describeGlobalDone(TNS__DescribeGlobalResponse)),
            this,  SLOT(describeGlobalDone(TNS__DescribeGlobalResponse)));
    connect(mSoap, SIGNAL(describeGlobalError(KDSoapMessage)),
            this, SLOT(describeGlobalError(KDSoapMessage)));

    connect(mSoap, SIGNAL(describeSObjectsDone(TNS__DescribeSObjectsResponse)),
            this,  SLOT(describeSObjectsDone(TNS__DescribeSObjectsResponse)));
    connect(mSoap, SIGNAL(describeSObjectsError(KDSoapMessage)),
            this, SLOT(describeSObjectsError(KDSoapMessage)));

    connect(mSoap, SIGNAL(queryDone(TNS__QueryResponse)),
            this,  SLOT(getEntryListDone(TNS__QueryResponse)));
    connect(mSoap, SIGNAL(queryError(KDSoapMessage)),
            this,  SLOT(getEntryListError(KDSoapMessage)));
    connect(mSoap, SIGNAL(queryMoreDone(TNS__QueryMoreResponse)),
            this,  SLOT(getEntryListDone(TNS__QueryMoreResponse)));
    connect(mSoap, SIGNAL(queryMoreError(KDSoapMessage)),
            this,  SLOT(getEntryListError(KDSoapMessage)));

    connect(mSoap, SIGNAL(upsertDone(TNS__UpsertResponse)),
            this,  SLOT(setEntryDone(TNS__UpsertResponse)));
    connect(mSoap, SIGNAL(upsertError(KDSoapMessage)),
            this,  SLOT(setEntryError(KDSoapMessage)));

    connect(mSoap, SIGNAL(deleteDone(TNS__DeleteResponse)),
            this,  SLOT(deleteEntryDone(TNS__DeleteResponse)));
    connect(mSoap, SIGNAL(deleteError(KDSoapMessage)),
            this,  SLOT(deleteEntryError(KDSoapMessage)));
}

void SalesforceResource::retrieveCollections()
{
    // TODO could attempt automatic login
    if (mSessionId.isEmpty()) {
        QString message;
        if (Settings::host().isEmpty()) {
#if 0
            message = i18nc("@info:status", "No server configured");
#endif
        } else if (Settings::self()->user().isEmpty()) {
            message = i18nc("@info:status", "No user name configured");
        } else {
            message = i18nc("@info:status", "Unable to login to %1", Settings::host());
        }

        status(Broken, message);
        error(message);
        cancelTask(message);
    } else {
        status(Running, i18nc("@info:status", "Retrieving folders"));
#if 0
        const TNS__DescribeGlobalResponse callResult = mSoap->describeGlobal();
        const QString error = mSoap->lastError();
        const TNS__DescribeGlobalResult result = callResult.result();
        kDebug() << "describeGlobal: maxBatchSize=" << result.maxBatchSize()
                 << "encoding=" << result.encoding()
                 << "error=" << error;
        const QList<TNS__DescribeGlobalSObjectResult> sobjects = result.sobjects();
        kDebug() << sobjects.count() << "SObjects";
        Q_FOREACH (const TNS__DescribeGlobalSObjectResult &object, sobjects) {
            kDebug() << "name=" << object.name() << "label=" << object.label()
                     << "keyPrefix=" << object.keyPrefix();
        }

        Collection topLevelCollection;
        topLevelCollection.setRemoteId(identifier());
        topLevelCollection.setName(name());
        topLevelCollection.setParentCollection(Collection::root());

        // Our top level collection only contains other collections (no items) and cannot be
        // modified by clients
        topLevelCollection.setContentMimeTypes(QStringList() << Collection::mimeType());
        topLevelCollection.setRights(Collection::ReadOnly);

        Collection::List collections;
        collections << topLevelCollection;

        // create just SalesforceContactsHandler
        ModuleHandler *handler = new SalesforceContactsHandler;
        mModuleHandlers->insert(handler->moduleName(), handler);

        Collection collection = handler->collection();

        collection.setParentCollection(topLevelCollection);
        collections << collection;

        collectionsRetrieved(collections);
#else
        // results handled by slots describeGlobalDone() and describeGlobalError()
        mSoap->asyncDescribeGlobal();
#endif
    }
}

void SalesforceResource::retrieveItems(const Akonadi::Collection &collection)
{
    if (collection.parentCollection() == Collection::root()) {
        itemsRetrieved(Item::List());
        return;
    }

    // TODO could attempt automatic login
    if (mSessionId.isEmpty()) {
        QString message;
        if (Settings::host().isEmpty()) {
            message = i18nc("@info:status", "No server configured");
        } else if (Settings::self()->user().isEmpty()) {
            message = i18nc("@info:status", "No user name configured");
        } else {
            message = i18nc("@info:status", "Unable to login to %1", Settings::host());
        }

        status(Broken, message);
        error(message);
        cancelTask(message);
    } else {
        // find the handler for the module represented by the given collection and let it
        // perform the respective "list entries" operation
        ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(collection.remoteId());
        if (moduleIt != mModuleHandlers->constEnd()) {
            status(Running, i18nc("@info:status", "Retrieving contents of folder %1", collection.name()));

            // getting items in batches
            setItemStreamingEnabled(true);

            // results handled by slots getEntryListDone() and getEntryListError()

            TNS__QueryLocator locator;
            moduleIt.value()->listEntries(locator, mSoap);
        } else {
            kDebug() << "No module handler for collection" << collection;
            itemsRetrieved(Item::List());
        }
    }
}

bool SalesforceResource::retrieveItem(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    Q_UNUSED(item);
    Q_UNUSED(parts);

    // TODO not implemented yet
    // retrieveItems() provides full items anyway so this one is not called
    // (no need for getting additional data)
    // should be implemented for consistency though

    return false;
}

void SalesforceResource::loginDone(const TNS__LoginResponse &callResult)
{
    const TNS__LoginResult loginResult = callResult.result();

    QString message;
    mSessionId = QString();

    const QString sessionId = loginResult.sessionId();
    if (sessionId.isEmpty()) {
        message = i18nc("@info:status", "Login failed: server returned an empty session identifier");
    } else if (mSessionId == QLatin1String("-1")) {
        message = i18nc("@info:status", "Login failed: server returned an invalid session identifier");
    } else {
        mSessionId = sessionId;
        kDebug() << "Login succeeded: sessionId=" << mSessionId;
    }

    if (message.isEmpty()) {
        // salesforce might issue a redirect on login so set a new endpoint
        mSoap->deleteLater();
        mSoap = new SforceService;
        mSoap->setEndPoint(loginResult.serverUrl());
        connectSoapProxy();

        TNS__SessionHeader sessionHeader;
        sessionHeader.setSessionId(mSessionId);
        mSoap->setSessionHeader(sessionHeader);

        status(Idle);

        synchronizeCollectionTree();
    } else {
        status(Broken, message);
        error(message);
    }
}

void SalesforceResource::loginError(const KDSoapMessage &fault)
{
    mSessionId = QString();

    const QString message = fault.faultAsString();
    kError() << message;

    status(Broken, message);
    error(message);
}

void SalesforceResource::getEntryListDone(const TNS__QueryResponse &callResult)
{
    const Collection collection = currentCollection();
    kDebug() << "got Query result for module" << collection.remoteId();

    Item::List items;

    // find the handler for the module represented by the given collection and let it
    // "deserialize" the SOAP response into an item payload and perform the respective "list entries" operation
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(collection.remoteId());
    if (moduleIt != mModuleHandlers->constEnd()) {
        const TNS__QueryResult queryResult = callResult.result();
        kDebug() << "result.size=" << queryResult.size() << "done=" << queryResult.done();
        if (queryResult.size() > 0) {
            itemsRetrieved(moduleIt.value()->itemsFromListEntriesResponse(queryResult, collection));

            if (!queryResult.done()) {
                moduleIt.value()->listEntries(queryResult.queryLocator(), mSoap);
            } else {
                status(Idle);
                itemsRetrievalDone();
            }
        } else {
            status(Idle);
            itemsRetrievalDone();
        }
    } else {
        kError() << "no handler for this module?";
    }
}

void SalesforceResource::getEntryListDone(const TNS__QueryMoreResponse &callResult)
{
    const Collection collection = currentCollection();
    kDebug() << "got QueryMore result for module" << collection.remoteId();

    Item::List items;

    // find the handler for the module represented by the given collection and let it
    // "deserialize" the SOAP response into an item payload and perform the respective "list entries" operation
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(collection.remoteId());
    if (moduleIt != mModuleHandlers->constEnd()) {
        const TNS__QueryResult queryResult = callResult.result();
        kDebug() << "result.size=" << queryResult.size() << "done=" << queryResult.done();
        if (queryResult.size() > 0) {
            itemsRetrieved(moduleIt.value()->itemsFromListEntriesResponse(queryResult, collection));

            if (!queryResult.done()) {
                moduleIt.value()->listEntries(queryResult.queryLocator(), mSoap);
            } else {
                status(Idle);
                itemsRetrievalDone();
            }
        } else {
            status(Idle);
            itemsRetrievalDone();
        }
    } else {
        kError() << "no handler for this module?";
    }
}

void SalesforceResource::getEntryListError(const KDSoapMessage &fault)
{
    const QString message = fault.faultAsString();
    kError() << message;

    status(Broken, message);
    error(message);
    cancelTask(message);
}

void SalesforceResource::setEntryDone(const TNS__UpsertResponse &callResult)
{
    QString message;

    const QList<TNS__UpsertResult> upsertResults = callResult.result();
    if (upsertResults.isEmpty()) {
        kError() << "UpsertResponse does not contain any results";
        message = i18nc("@info:status", "Server did not respond as expected: result set is empty");
    } else {
        if (upsertResults.count() > 1) {
            kError() << "Expecting one upsert result in response but got"
                     << upsertResults.count() << ". Will just take first one";
        }

        const TNS__UpsertResult upsertResult = upsertResults[ 0 ];
        if (!upsertResult.success()) {
            const QList<TNS__Error> errors = upsertResult.errors();
            if (!errors.isEmpty()) {
                message = errors[ 0 ].message();
            } else {
                // that can probably not be reached, just to be sure
                if (mPendingItem.remoteId().isEmpty()) {
                    message = i18nc("@info:status", "Creation of an item failed for unspecified reasons");
                } else {
                    message = i18nc("@info:status", "Modification of an item failed for unspecified reasons");
                }
            }
        } else {
            // setting the remoteId is technically only required for the handling the result of
            // itemAdded() so Akonadi knows which identifier was assigned to the item on the server.
            mPendingItem.setRemoteId(upsertResult.id().value());
        }
    }

    if (!message.isEmpty()) {
        kError() << message;
        status(Broken, message);
        error(message);
        cancelTask(message);
    } else {
        kDebug() << "itemAdded/itemChanged done, comitting pending item (id="
                 << mPendingItem.id() << ", remoteId=" << mPendingItem.remoteId()
                 << ", mime=" << mPendingItem.mimeType();
        status(Idle);
        changeCommitted(mPendingItem);
    }

    mPendingItem = Item();
}

void SalesforceResource::setEntryError(const KDSoapMessage &fault)
{
    const QString message = fault.faultAsString();

    kError() << message;
    status(Broken, message);
    error(message);
    cancelTask(message);

    mPendingItem = Item();
}

void SalesforceResource::deleteEntryDone(const TNS__DeleteResponse &callResult)
{
    QString message;

    const QList<TNS__DeleteResult> deleteResults = callResult.result();
    if (deleteResults.isEmpty()) {
        kError() << "deleteResponse does not contain any results";
        message = i18nc("@info:status", "Server did not respond as expected: result set is empty");
    } else {
        if (deleteResults.count() > 1) {
            kError() << "Expecting one delete result in response but got"
                     << deleteResults.count() << ". Will just take first one";
        }

        const TNS__DeleteResult deleteResult = deleteResults[ 0 ];
        if (!deleteResult.success()) {
            const QList<TNS__Error> errors = deleteResult.errors();
            if (!errors.isEmpty()) {
                message = errors[ 0 ].message();
            } else {
                // that can probably not be reached, just to be sure
                message = i18nc("@info:status", "Deletion of an item failed for unspecified reasons");
            }
        }
    }

    if (!message.isEmpty()) {
        kError() << message;
        status(Broken, message);
        error(message);
        cancelTask(message);
    } else {
        kDebug() << "itemRemoved done, comitting pending item (id="
                 << mPendingItem.id() << ", remoteId=" << mPendingItem.remoteId()
                 << ", mime=" << mPendingItem.mimeType();
        status(Idle);
        changeCommitted(mPendingItem);
    }

    mPendingItem = Item();
}

void SalesforceResource::deleteEntryError(const KDSoapMessage &fault)
{
    const QString message = fault.faultAsString();
    kError() << message;

    status(Broken, message);
    error(message);
    cancelTask(message);

    mPendingItem = Item();
}

void SalesforceResource::describeGlobalDone(const TNS__DescribeGlobalResponse &callResult)
{
    mTopLevelCollection.setRemoteId(identifier());
    mTopLevelCollection.setName(name());
    mTopLevelCollection.setParentCollection(Collection::root());

    // Our top level collection only contains other collections (no items) and cannot be
    // modified by clients
    mTopLevelCollection.setContentMimeTypes(QStringList() << Collection::mimeType());
    mTopLevelCollection.setRights(Collection::ReadOnly);

    Collection::List collections;
    collections << mTopLevelCollection;

    const TNS__DescribeGlobalResult result = callResult.result();
    kDebug() << "describeGlobal: maxBatchSize=" << result.maxBatchSize()
             << "encoding=" << result.encoding();
    const QList<TNS__DescribeGlobalSObjectResult> sobjects = result.sobjects();
    kDebug() << sobjects.count() << "SObjects";

    QStringList unknownModules;
    Q_FOREACH (const TNS__DescribeGlobalSObjectResult &object, sobjects) {
//         kDebug() << "name=" << object.name() << "label=" << object.label()
//                  << "keyPrefix=" << object.keyPrefix();

        // assume for now that each "sobject" describes a module
        const QString module = object.name();
        mAvailableModules << module;

        Collection collection;

        // check if we have a corresponding module handler already
        // if not see if we can create one
        ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(module);
        if (moduleIt != mModuleHandlers->constEnd()) {
            collection = moduleIt.value()->collection();
            collection.setParentCollection(mTopLevelCollection);
            collections << collection;
        } else {
            SalesforceModuleHandler *handler = 0;
            if (module == QLatin1String("Contact")) {
                handler = new SalesforceContactsHandler;
                unknownModules << module;
            } else {
                //kDebug() << "No module handler for" << module;
                continue;
            }
            mModuleHandlers->insert(module, handler);

            ModuleDebugInterface *debugInterface = new ModuleDebugInterface(module, this);
            QDBusConnection::sessionBus().registerObject(QLatin1String("/CRMDebug/modules/") + module,
                    debugInterface,
                    QDBusConnection::ExportScriptableSlots);
        }
    }

    kDebug() << collections.count() << "collections"
             << unknownModules.count() << "new modules";

    if (!unknownModules.isEmpty()) {
        // we have new modules, we need to get their object description
        // report all collections we've got so far, provide the newly ones later
        setCollectionStreamingEnabled(true);
        collectionsRetrieved(collections);

        QMetaObject::invokeMethod(this, "describeSObjects", Qt::QueuedConnection,
                                  Q_ARG(QStringList, unknownModules));
    } else {
        collectionsRetrieved(collections);
    }
}

void SalesforceResource::describeGlobalError(const KDSoapMessage &fault)
{
    const QString message = fault.faultAsString();
    kError() << message;

    status(Broken, message);
    error(message);
    cancelTask(message);
}

void SalesforceResource::describeSObjects(const QStringList &objects)
{
    kDebug() << "Getting descriptions for new modules:" << objects;

    TNS__DescribeSObjects param;
    param.setSObjectType(objects);

    mSoap->asyncDescribeSObjects(param);
}

void SalesforceResource::describeSObjectsDone(const TNS__DescribeSObjectsResponse &callResult)
{
    Collection::List collections;

    const QList<TNS__DescribeSObjectResult> resultList = callResult.result();
    Q_FOREACH (const TNS__DescribeSObjectResult &result, resultList) {
        kDebug() << "describeSObject result: name=" << result.name();
        ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(result.name());
        if (moduleIt != mModuleHandlers->constEnd()) {
            moduleIt.value()->setDescriptionResult(result);

            Collection collection = moduleIt.value()->collection();
            collection.setParentCollection(mTopLevelCollection);

            collections << collection;
        }
    }

    collectionsRetrieved(collections);
    collectionsRetrievalDone();
}

void SalesforceResource::describeSObjectsError(const KDSoapMessage &fault)
{
    const QString message = fault.faultAsString();
    kError() << message;

    status(Broken, message);
    error(message);
    cancelTask(message);
}

AKONADI_RESOURCE_MAIN(SalesforceResource)

#include "salesforceresource.moc"
