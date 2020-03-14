/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "salesforceresource_debug.h"
#include "salesforcecontactshandler.h"
#include "moduledebuginterface.h"
#include "resourcedebuginterface.h"
#include "settings.h"
#include "settingsadaptor.h"
#include "salesforceconfigdialog.h"
#include "salesforcesoap.h"
using namespace KDSoapGenerated;

#include "kdcrmutils.h"

#include <AkonadiCore/ChangeRecorder>
#include <AkonadiCore/Collection>
#include <AkonadiCore/ItemFetchScope>

#include <KLocalizedString>
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
    KDCRMUtils::setupIconTheme();

    new SettingsAdaptor(Settings::self());
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Settings"),
            Settings::self(), QDBusConnection::ExportAdaptors);

    auto *debugInterface = new ResourceDebugInterface(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/CRMDebug"),
            debugInterface,
            QDBusConnection::ExportScriptableSlots);

    setNeedsNetwork(true);

    // make sure itemAdded() and itemChanged() get the full item from Akonadi before being called
    changeRecorder()->itemFetchScope().fetchFullPayload();

    // make sure these call have the collection available as well
    changeRecorder()->fetchCollection(true);

    connectSoapProxy();
}

SalesforceResource::~SalesforceResource()
{
    qDeleteAll(*mModuleHandlers);
    delete mModuleHandlers;
    delete mSoap;
}

void SalesforceResource::configure(WId windowId)
{
    SalesforceConfigDialog dialog(name());

    // make sure we are seen as a child window of the caller's window
    // otherwise focus stealing prevention might put us behind it
#if !defined(Q_OS_MACOS)
    KWindowSystem::setMainWindow(&dialog, windowId);
#endif

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
    if (host != Settings::host()) {
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

    if (user != Settings::user() || password != Settings::password()) {
        if (!mSessionId.isEmpty()) {
            mSoap->logout();
            mSessionId = QString();
        }

        newLogin = true;
    }

    Settings::setHost(host);
    Settings::setUser(user);
    Settings::setPassword(password);
    Settings::self()->save();

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
        if (Settings::host().isEmpty()) {
            const QString message = i18nc("@info:status", "No server configured");
            emit status(Broken, message);
            emit error(message);
        } else if (Settings::user().isEmpty()) {
            const QString message = i18nc("@info:status", "No user name configured");
            emit status(Broken, message);
            emit error(message);
#else
        if (Settings::user().isEmpty()) {
            const QString message = i18nc("@info:status", "No user name configured");
            emit status(Broken, message);
            emit error(message);
#endif
        } else {
            doLogin();
        }
    }
}

void SalesforceResource::doLogin()
{
    const QString username = Settings::user();
    const QString password = Settings::password();

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
        emit status(Running);
    } else {
        emit status(Broken, message);
        emit error(message);
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
        emit status(Running);
    } else {
        emit status(Broken, message);
        emit error(message);
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

    emit status(Running);
}

void SalesforceResource::connectSoapProxy()
{
    Q_ASSERT(mSoap != nullptr);

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
        } else if (Settings::user().isEmpty()) {
            message = i18nc("@info:status", "No user name configured");
        } else {
            message = i18nc("@info:status", "Unable to login to %1", Settings::host());
        }

        emit status(Broken, message);
        emit error(message);
        cancelTask(message);
    } else {
        emit status(Running, i18nc("@info:status", "Retrieving folders"));
#if 0
        const TNS__DescribeGlobalResponse callResult = mSoap->describeGlobal();
        const QString error = mSoap->lastError();
        const TNS__DescribeGlobalResult result = callResult.result();
        qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "describeGlobal: maxBatchSize=" << result.maxBatchSize()
                 << "encoding=" << result.encoding()
                 << "error=" << error;
        const QList<TNS__DescribeGlobalSObjectResult> sobjects = result.sobjects();
        qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << sobjects.count() << "SObjects";
        Q_FOREACH (const TNS__DescribeGlobalSObjectResult &object, sobjects) {
            qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "name=" << object.name() << "label=" << object.label()
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
        } else if (Settings::user().isEmpty()) {
            message = i18nc("@info:status", "No user name configured");
        } else {
            message = i18nc("@info:status", "Unable to login to %1", Settings::host());
        }

        emit status(Broken, message);
        emit error(message);
        cancelTask(message);
    } else {
        // find the handler for the module represented by the given collection and let it
        // perform the respective "list entries" operation
        ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(collection.remoteId());
        if (moduleIt != mModuleHandlers->constEnd()) {
            emit status(Running, i18nc("@info:status", "Retrieving contents of folder %1", collection.name()));

            // getting items in batches
            setItemStreamingEnabled(true);

            // results handled by slots getEntryListDone() and getEntryListError()

            TNS__QueryLocator locator;
            moduleIt.value()->listEntries(locator, mSoap);
        } else {
            qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "No module handler for collection" << collection;
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
        qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "Login succeeded: sessionId=" << mSessionId;
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

        emit status(Idle);

        synchronizeCollectionTree();
    } else {
        emit status(Broken, message);
        emit error(message);
    }
}

void SalesforceResource::loginError(const KDSoapMessage &fault)
{
    mSessionId = QString();

    const QString message = fault.faultAsString();
    qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << message;

    emit status(Broken, message);
    emit error(message);
}

void SalesforceResource::getEntryListDone(const TNS__QueryResponse &callResult)
{
    const Collection collection = currentCollection();
    qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "got Query result for module" << collection.remoteId();

    // find the handler for the module represented by the given collection and let it
    // "deserialize" the SOAP response into an item payload and perform the respective "list entries" operation
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(collection.remoteId());
    if (moduleIt != mModuleHandlers->constEnd()) {
        const TNS__QueryResult queryResult = callResult.result();
        qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "result.size=" << queryResult.size() << "done=" << queryResult.done();
        if (queryResult.size() > 0) {
            itemsRetrieved(moduleIt.value()->itemsFromListEntriesResponse(queryResult, collection));

            if (!queryResult.done()) {
                moduleIt.value()->listEntries(queryResult.queryLocator(), mSoap);
            } else {
                emit status(Idle);
                itemsRetrievalDone();
            }
        } else {
            emit status(Idle);
            itemsRetrievalDone();
        }
    } else {
        qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << "no handler for this module?";
    }
}

void SalesforceResource::getEntryListDone(const TNS__QueryMoreResponse &callResult)
{
    const Collection collection = currentCollection();
    qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "got QueryMore result for module" << collection.remoteId();

    // find the handler for the module represented by the given collection and let it
    // "deserialize" the SOAP response into an item payload and perform the respective "list entries" operation
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(collection.remoteId());
    if (moduleIt != mModuleHandlers->constEnd()) {
        const TNS__QueryResult queryResult = callResult.result();
        qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "result.size=" << queryResult.size() << "done=" << queryResult.done();
        if (queryResult.size() > 0) {
            itemsRetrieved(moduleIt.value()->itemsFromListEntriesResponse(queryResult, collection));

            if (!queryResult.done()) {
                moduleIt.value()->listEntries(queryResult.queryLocator(), mSoap);
            } else {
                emit status(Idle);
                itemsRetrievalDone();
            }
        } else {
            emit status(Idle);
            itemsRetrievalDone();
        }
    } else {
        qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << "no handler for this module?";
    }
}

void SalesforceResource::getEntryListError(const KDSoapMessage &fault)
{
    const QString message = fault.faultAsString();
    qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << message;

    emit status(Broken, message);
    emit error(message);
    cancelTask(message);
}

void SalesforceResource::setEntryDone(const TNS__UpsertResponse &callResult)
{
    QString message;

    const QList<TNS__UpsertResult> upsertResults = callResult.result();
    if (upsertResults.isEmpty()) {
        qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << "UpsertResponse does not contain any results";
        message = i18nc("@info:status", "Server did not respond as expected: result set is empty");
    } else {
        if (upsertResults.count() > 1) {
            qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << "Expecting one upsert result in response but got"
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
        qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << message;
        emit status(Broken, message);
        emit error(message);
        cancelTask(message);
    } else {
        qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "itemAdded/itemChanged done, comitting pending item (id="
                 << mPendingItem.id() << ", remoteId=" << mPendingItem.remoteId()
                 << ", mime=" << mPendingItem.mimeType();
        emit status(Idle);
        changeCommitted(mPendingItem);
    }

    mPendingItem = Item();
}

void SalesforceResource::setEntryError(const KDSoapMessage &fault)
{
    const QString message = fault.faultAsString();

    qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << message;
    emit status(Broken, message);
    emit error(message);
    cancelTask(message);

    mPendingItem = Item();
}

void SalesforceResource::deleteEntryDone(const TNS__DeleteResponse &callResult)
{
    QString message;

    const QList<TNS__DeleteResult> deleteResults = callResult.result();
    if (deleteResults.isEmpty()) {
        qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << "deleteResponse does not contain any results";
        message = i18nc("@info:status", "Server did not respond as expected: result set is empty");
    } else {
        if (deleteResults.count() > 1) {
            qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << "Expecting one delete result in response but got"
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
        qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << message;
        emit status(Broken, message);
        emit error(message);
        cancelTask(message);
    } else {
        qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "itemRemoved done, comitting pending item (id="
                 << mPendingItem.id() << ", remoteId=" << mPendingItem.remoteId()
                 << ", mime=" << mPendingItem.mimeType();
        emit status(Idle);
        changeCommitted(mPendingItem);
    }

    mPendingItem = Item();
}

void SalesforceResource::deleteEntryError(const KDSoapMessage &fault)
{
    const QString message = fault.faultAsString();
    qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << message;

    emit status(Broken, message);
    emit error(message);
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
    qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "describeGlobal: maxBatchSize=" << result.maxBatchSize()
             << "encoding=" << result.encoding();
    const QList<TNS__DescribeGlobalSObjectResult> sobjects = result.sobjects();
    qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << sobjects.count() << "SObjects";

    QStringList unknownModules;
    Q_FOREACH (const TNS__DescribeGlobalSObjectResult &object, sobjects) {
//         qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "name=" << object.name() << "label=" << object.label()
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
            SalesforceModuleHandler *handler = nullptr;
            if (module == QLatin1String("Contact")) {
                handler = new SalesforceContactsHandler;
                unknownModules << module;
            } else {
                //qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "No module handler for" << module;
                continue;
            }
            mModuleHandlers->insert(module, handler);

            auto *debugInterface = new ModuleDebugInterface(module, this);
            QDBusConnection::sessionBus().registerObject(QStringLiteral("/CRMDebug/modules/") + module,
                    debugInterface,
                    QDBusConnection::ExportScriptableSlots);
        }
    }

    qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << collections.count() << "collections"
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
    qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << message;

    emit status(Broken, message);
    emit error(message);
    cancelTask(message);
}

void SalesforceResource::describeSObjects(const QStringList &objects)
{
    qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "Getting descriptions for new modules:" << objects;

    TNS__DescribeSObjects param;
    param.setSObjectType(objects);

    mSoap->asyncDescribeSObjects(param);
}

void SalesforceResource::describeSObjectsDone(const TNS__DescribeSObjectsResponse &callResult)
{
    Collection::List collections;

    const QList<TNS__DescribeSObjectResult> resultList = callResult.result();
    Q_FOREACH (const TNS__DescribeSObjectResult &result, resultList) {
        qCDebug(FATCRM_SALESFORCERESOURCE_LOG) << "describeSObject result: name=" << result.name();
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
    qCCritical(FATCRM_SALESFORCERESOURCE_LOG) << message;

    emit status(Broken, message);
    emit error(message);
    cancelTask(message);
}

AKONADI_RESOURCE_MAIN(SalesforceResource)

