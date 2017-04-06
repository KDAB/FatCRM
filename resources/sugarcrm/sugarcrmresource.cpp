/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sugarcrmresource.h"

#include "accountshandler.h"
#include "campaignshandler.h"
#include "conflicthandler.h"
#include "contactshandler.h"
#include "createentryjob.h"
#include "deleteentryjob.h"
#include "documentshandler.h"
#include "emailshandler.h"
#include "fetchentryjob.h"
#include "itemtransferinterface.h"
#include "leadshandler.h"
#include "listentriesjob.h"
#include "listdeletedentriesjob.h"
#include "listmodulesjob.h"
#include "loginjob.h"
#include "moduledebuginterface.h"
#include "noteshandler.h"
#include "opportunitieshandler.h"
#include "resourcedebuginterface.h"
#include "settings.h"
#include "settingsadaptor.h"
#include "sugarconfigdialog.h"
#include "sugarsession.h"
#include "taskshandler.h"
#include "updateentryjob.h"
#include "passwordhandler.h"
#include "sugarsoapprotocol.h"


#include <Akonadi/ChangeRecorder>
#include <Akonadi/Collection>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/ItemModifyJob>
#include <Akonadi/CachePolicy>

#include <KABC/Addressee>

#include <KLocale>
#include <KWindowSystem>

#include <QtDBus/QDBusConnection>

using namespace Akonadi;

SugarCRMResource::SugarCRMResource(const QString &id)
    : ResourceBase(id),
      mPasswordHandler(new PasswordHandler(id, this)),
      mSession(new SugarSession(mPasswordHandler, this)),
      mCurrentJob(nullptr),
      mLoginJob(nullptr),
      mDebugInterface(new ResourceDebugInterface(this)),
      mModuleHandlers(new ModuleHandlerHash),
      mModuleDebugInterfaces(new ModuleDebugInterfaceHash),
      mConflictHandler(new ConflictHandler(ConflictHandler::BackendConflict, this)),
      mOnline(false)
{
    new SettingsAdaptor(Settings::self());
    QDBusConnection::sessionBus().registerObject(QLatin1String("/Settings"),
            Settings::self(),
            QDBusConnection::ExportAdaptors);

    QDBusConnection::sessionBus().registerObject(QLatin1String("/CRMDebug"),
            mDebugInterface,
            QDBusConnection::ExportScriptableSlots);

    ItemTransferInterface *itemDownloadInterface = new ItemTransferInterface(this);
    QDBusConnection::sessionBus().registerObject(QLatin1String("/ItemTransfer"),
            itemDownloadInterface,
            QDBusConnection::ExportScriptableSlots);

    setNeedsNetwork(true);
#if KDE_IS_VERSION(4, 14, 0)
    setDisableAutomaticItemDeliveryDone(true);
#endif

    // make sure itemAdded() and itemChanged() get the full item from Akonadi before being called
    changeRecorder()->itemFetchScope().fetchFullPayload();

    // make sure these call have the collection available as well
    changeRecorder()->fetchCollection(true);

    mSession->setSessionParameters(Settings::user(), QString() /*password not read yet*/,
                                   Settings::host());
    mSession->createSoapInterface();

    SugarSoapProtocol *protocol = new SugarSoapProtocol;
    protocol->setSession(mSession);
    mSession->setProtocol(protocol);

    connect(mConflictHandler, SIGNAL(commitChange(Akonadi::Item)),
            this, SLOT(commitChange(Akonadi::Item)));
    connect(mConflictHandler, SIGNAL(updateOnBackend(Akonadi::Item)),
            this, SLOT(updateOnBackend(Akonadi::Item)));

    createModuleHandlers(Settings::availableModules());
}

SugarCRMResource::~SugarCRMResource()
{
    qDeleteAll(*mModuleHandlers);
    delete mModuleHandlers;
    delete mModuleDebugInterfaces; // interface instances destroyed by parent QObject
}

void SugarCRMResource::configure(WId windowId)
{
    SugarConfigDialog dialog(mPasswordHandler, name());

    // make sure we are seen as a child window of the caller's window
    // otherwise focus stealing prevention might put us behind it
    KWindowSystem::setMainWindow(&dialog, windowId);

    int result = dialog.exec();

    if (result == QDialog::Rejected) {
        emit configurationDialogRejected();
        return;
    }

    const QString host = dialog.host();
    const QString user = dialog.user();
    const QString password = dialog.password();
    const QString accountName = dialog.accountName();
    const int intervalCheckTime = dialog.intervalCheckTime();

    SugarSession::RequiredAction action = mSession->setSessionParameters(user, password, host);
    switch (action) {
    case SugarSession::None:
        break;

    case SugarSession::NewLogin:
        mSession->createSoapInterface();
    // fall through
    case SugarSession::ReLogin:
        if (isOnline()) {
            // schedule login as a prepended custom task to hold all other until finished
            scheduleCustomTask(this, "startExplicitLogin", QVariant(), ResourceBase::Prepend);
        }
        break;
    }

    Settings::setHost(host);
    Settings::setUser(user);
    Settings::setIntervalCheckTime(intervalCheckTime);
    Settings::self()->writeConfig();
    mPasswordHandler->setPassword(password);

    setName(accountName);

    emit configurationDialogAccepted();
}

void SugarCRMResource::aboutToQuit()
{
    // just a curtesy to the server
    mSession->logout();
}

void SugarCRMResource::doSetOnline(bool online)
{
    // akonadiserver calls setOnline() multiple times with the same value...
    // let's only react to real changes here
    if (online != mOnline) {
        kDebug() << online;
        mOnline = online;
        if (online) {
            if (Settings::host().isEmpty()) {
                const QString message = i18nc("@info:status", "No server configured");
                status(Broken, message);
                error(message);
            } else if (Settings::user().isEmpty()) {
                const QString message = i18nc("@info:status", "No user name configured");
                status(Broken, message);
                error(message);
            } else {
                // schedule login as a prepended custom task to hold all other until finished
                scheduleCustomTask(this, "startExplicitLogin", QVariant(), ResourceBase::Prepend);
            }
        } else {
            // Abort current job, given that the resource scheduler aborted the current task
            if (mCurrentJob) {
                mCurrentJob->kill(KJob::Quietly);
                mCurrentJob = nullptr;
            }
            if (mLoginJob) {
                mLoginJob->kill(KJob::Quietly);
                mLoginJob = nullptr;
            }

            // "Log out", but no point in trying to tell the server, we're offline.
            mSession->forgetSession();
        }
    }

    ResourceBase::doSetOnline(online);
}

void SugarCRMResource::itemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection)
{
    // find the handler for the module represented by the given collection and let it
    // perform the respective "set entry" operation
    ModuleHandler *handler = mModuleHandlers->value(collection.remoteId());
    if (handler) {
        status(Running);

        CreateEntryJob *job = new CreateEntryJob(item, mSession, this);
        Q_ASSERT(!mCurrentJob);
        mCurrentJob = job;
        job->setModule(handler);
        connect(job, SIGNAL(result(KJob*)), this, SLOT(createEntryResult(KJob*)));
        job->start();
    } else {
        const QString message = i18nc("@info:status", "Cannot add items to folder %1",
                                      collection.name());
        kWarning() << message;

        status(Broken, message);
        error(message);
        cancelTask(message);
    }
}

void SugarCRMResource::itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    // find the handler for the module represented by the given collection and let it
    // perform the respective "set entry" operation
    const Collection collection = item.parentCollection();
    ModuleHandler *handler = mModuleHandlers->value(collection.remoteId());
    if (handler) {
        if (!handler->needBackendChange(item, parts)) {
            kWarning() << "Handler for module" << handler->moduleName()
                       << "indicates that backend change for item id=" << item.id()
                       << ", remoteId=" << item.remoteId()
                       << "is not required for given modified parts: " << parts;
            changeCommitted(item);
            return;
        }
        status(Running);

        updateItem(item, handler);
    } else {
        const QString message = i18nc("@info:status", "Cannot modify items in folder %1",
                                      collection.name());
        kWarning() << message;

        status(Broken, message);
        error(message);
        cancelTask(message);
    }
}

void SugarCRMResource::itemRemoved(const Akonadi::Item &item)
{
    const Collection collection = item.parentCollection();

    // not uploaded yet?
    if (item.remoteId().isEmpty() || collection.remoteId().isEmpty()) {
        changeCommitted(item);
        return;
    }

    status(Running);

#if 0
    const QString message = "disabled for safety reasons";
    status(Broken, message);
    error(message);
    cancelTask(message);
#else
    SugarJob *job = new DeleteEntryJob(item, mSession, this);
    Q_ASSERT(!mCurrentJob);
    mCurrentJob = job;
    connect(job, SIGNAL(result(KJob*)), this, SLOT(deleteEntryResult(KJob*)));
    job->start();
#endif
}

void SugarCRMResource::retrieveCollections()
{
    status(Running, i18nc("@info:status", "Retrieving folders"));

    SugarJob *job = new ListModulesJob(mSession, this);
    Q_ASSERT(!mCurrentJob);
    mCurrentJob = job;
    connect(job, SIGNAL(result(KJob*)), this, SLOT(listModulesResult(KJob*)));
    job->start();
}

void SugarCRMResource::retrieveItems(const Akonadi::Collection &collection)
{
    if (collection.parentCollection() == Collection::root()) {
        itemsRetrieved(Item::List());
        return;
    }

    // find the handler for the module represented by the given collection and let it
    // perform the respective "list entries" operation
    ModuleHandler *handler = mModuleHandlers->value(collection.remoteId());
    if (handler) {
        // getting items in batches
        setItemStreamingEnabled(true);

        ListEntriesJob *job = new ListEntriesJob(collection, mSession, this);
        job->setModule(handler);
        job->setLatestTimestamp(ListEntriesJob::latestTimestamp(collection, handler));
        Q_ASSERT(!mCurrentJob);
        mCurrentJob = job;

        const QString message = job->isUpdateJob()
                ? i18nc("@info:status", "Updating contents of folder %1", collection.name())
                : i18nc("@info:status", "Retrieving contents of folder %1", collection.name());
        kDebug() << message;
        status(Running, message);

        connect(job, SIGNAL(totalItems(int)),
                this, SLOT(slotTotalItems(int)));
        connect(job, SIGNAL(progress(int)),
                this, SLOT(slotProgress(int)));
        connect(job, SIGNAL(itemsReceived(Akonadi::Item::List,bool)),
                this, SLOT(slotItemsReceived(Akonadi::Item::List,bool)));
        connect(job, SIGNAL(result(KJob*)), this, SLOT(listEntriesResult(KJob*)));
        job->start();
    } else {
        kDebug() << "No module handler for collection" << collection;
        kDebug() << mModuleHandlers->keys();
        itemsRetrieved(Item::List());
    }
}

bool SugarCRMResource::retrieveItem(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    Q_UNUSED(parts);

    const Collection collection = item.parentCollection();

    // find the handler for the module represented by the given collection and let it
    // perform the respective "get entry" operation
    ModuleHandler *handler = mModuleHandlers->value(collection.remoteId());
    if (handler) {
        const QString message = i18nc("@info:status", "Retrieving entry from folder %1",
                                      collection.name());
        kDebug() << message;
        status(Running, message);

        FetchEntryJob *job = new FetchEntryJob(item, mSession, this);
        Q_ASSERT(!mCurrentJob);
        mCurrentJob = job;
        job->setModule(handler);
        connect(job, SIGNAL(result(KJob*)), this, SLOT(fetchEntryResult(KJob*)));
        job->start();
        return true;
    } else {
        kDebug() << "No module handler for collection" << collection;
        return false;
    }
}

void SugarCRMResource::startExplicitLogin()
{
    kDebug();
    Q_ASSERT(!mLoginJob); // didn't we kill it in doSetOnline(false) already?
    mLoginJob = new LoginJob(mSession, this);
    connect(mLoginJob, SIGNAL(result(KJob*)), this, SLOT(explicitLoginResult(KJob*)));
    mLoginJob->start();
}

void SugarCRMResource::explicitLoginResult(KJob *job)
{
    Q_ASSERT(mLoginJob == job);
    mLoginJob = nullptr;
    if (handleLoginError(job)) {
        return;
    }

    if (job->error() != 0) {
        QString message = job->errorText();
        kWarning() << "error=" << job->error() << ":" << message;

        if (Settings::host().isEmpty()) {
            message = i18nc("@info:status", "No server configured");
        } else if (Settings::user().isEmpty()) {
            message = i18nc("@info:status", "No user name configured");
        } else {
            message = i18nc("@info:status", "Unable to login user %1 on %2: %3",
                            Settings::user(), Settings::host(), message);
        }

        kWarning() << message;
        status(Broken, message);
        error(message);
        cancelTask(message);
        return;
    }

    taskDone();
    status(Idle);
    synchronizeCollectionTree();
}

void SugarCRMResource::listModulesResult(KJob *job)
{
    Q_ASSERT(mCurrentJob == job);
    mCurrentJob = nullptr;

    if (handleLoginError(job)) {
        return;
    }

    if (job->error() != 0) {
        const QString message = job->errorText();
        kWarning() << "error=" << job->error() << ":" << message;

        status(Broken, message);
        error(message);
        cancelTask(message);
        return;
    }

    ListModulesJob *listJob = qobject_cast<ListModulesJob *>(job);
    Q_ASSERT(listJob != nullptr);

    Collection::List collections;

    Collection topLevelCollection;
    topLevelCollection.setRemoteId(identifier());
    topLevelCollection.setName(name());
    topLevelCollection.setParentCollection(Collection::root());

    // Our top level collection only contains other collections (no items) and cannot be
    // modified by clients
    topLevelCollection.setContentMimeTypes(QStringList() << Collection::mimeType());
    topLevelCollection.setRights(Collection::ReadOnly);

    Akonadi::CachePolicy policy;
    policy.setInheritFromParent( false );
    policy.setIntervalCheckTime( Settings::intervalCheckTime() );
    topLevelCollection.setCachePolicy( policy );

    collections << topLevelCollection;

    const QStringList availableModules = listJob->modules();

    createModuleHandlers(availableModules);

    Q_FOREACH (const QString &module, availableModules) {
        ModuleHandler* handler = mModuleHandlers->value(module);
        if (handler) {
            Collection collection = handler->collection();
            collection.setParentCollection(topLevelCollection);
            collections << collection;
        }
    }

    Settings::setAvailableModules(availableModules);
    Settings::self()->writeConfig();

    collectionsRetrieved(collections);
    status(Idle);
}

void SugarCRMResource::slotTotalItems(int count)
{
    mTotalItems = count;
    setTotalItems(count);
}

void SugarCRMResource::slotProgress(int count)
{
    // only emitted in the non-incremental case
    emit percent(100 * count / mTotalItems);
}

void SugarCRMResource::slotItemsReceived(const Item::List &items, bool isUpdateJob)
{
    if (isUpdateJob) {
        itemsRetrievedIncremental(items, Item::List());
    } else {
        itemsRetrieved(items);
    }
}

void SugarCRMResource::listEntriesResult(KJob *job)
{
    ListEntriesJob *listEntriesJob = static_cast<ListEntriesJob *>(job);

    Q_ASSERT(mCurrentJob == job);
    mCurrentJob = nullptr;
    if (handleLoginError(job)) {
        return;
    }

    kDebug() << job;
    if (job->error() != 0) {
        const QString message = job->errorText();
        kWarning() << "error=" << job->error() << ":" << message;

        status(Broken, message);
        error(message);
        cancelTask(message);
        return;
    }

    if (listEntriesJob->isUpdateJob()) {
        // ensure the incremental mode is ON even if there were neither an update nor a delete
        itemsRetrievedIncremental(Item::List(), Item::List());
    }
    itemsRetrievalDone();

    // Next step: list deleted items (must be done outside of the ItemSync)
    ListDeletedItemsArg arg;
    arg.collection = listEntriesJob->collection();
    arg.module = listEntriesJob->module();
    arg.collectionAttributesChanged = listEntriesJob->collectionAttributesChanged();
    arg.isUpdateJob = listEntriesJob->isUpdateJob();
    arg.fullSyncTimestamp = listEntriesJob->newTimestamp();
    scheduleCustomTask(this, "listDeletedItems", QVariant::fromValue(arg));

    status(Idle);
}

void SugarCRMResource::listDeletedItems(const QVariant &val)
{
    const ListDeletedItemsArg arg = val.value<ListDeletedItemsArg>();
    ListDeletedEntriesJob *ldeJob = new ListDeletedEntriesJob(arg.collection, mSession, this);
    ldeJob->setModule(arg.module);
    ldeJob->setCollectionAttributesChanged(arg.collectionAttributesChanged);
    ldeJob->setLatestTimestamp(ListDeletedEntriesJob::latestTimestamp(arg.collection));

    if (!arg.isUpdateJob) {
        // Set initial timestamp for "deleted items" based on the time of the full sync
        // (no need to get all old deletions). No need to even run the job either, in that case.
        ldeJob->setInitialTimestamp(arg.fullSyncTimestamp);

        // Commit attribute changes
        if (ldeJob->collectionAttributesChanged()) {
            ldeJob->module()->modifyCollection(ldeJob->collection());
        }

        delete ldeJob;
        taskDone();
        status(Idle);
        return;
    }

    connect(ldeJob, SIGNAL(result(KJob*)), this, SLOT(slotListDeletedEntriesResult(KJob*)));
    mCurrentJob = ldeJob;
    // don't set mCurrentJob, can run in parallel to e.g. retrieveCollections()
    const QString message = i18nc("@info:status", "Retrieving deleted items for folder %1", arg.collection.name());
    kDebug() << message;
    status(Running, message);
    ldeJob->start();
}

void SugarCRMResource::slotListDeletedEntriesResult(KJob *job)
{
    if (job->error()) {
        kWarning() << job->errorString();
    }
    mCurrentJob = nullptr;

    // Commit attribute changes
    ListDeletedEntriesJob *listDelEntriesJob = static_cast<ListDeletedEntriesJob *>(job);
    if (listDelEntriesJob->collectionAttributesChanged()) {
        listDelEntriesJob->module()->modifyCollection(listDelEntriesJob->collection());
    }

    taskDone();
    status(Idle);
}

void SugarCRMResource::createEntryResult(KJob *job)
{
    Q_ASSERT(mCurrentJob == job);
    mCurrentJob = nullptr;
    if (handleLoginError(job)) {
        return;
    }

    if (job->error() != 0) {
        const QString message = job->errorText();
        kWarning() << "error=" << job->error() << ":" << message;

        status(Broken, message);
        error(message);
        cancelTask(message);
        return;
    }

    CreateEntryJob *createJob = qobject_cast<CreateEntryJob *>(job);
    Q_ASSERT(createJob != nullptr);

    changeCommitted(createJob->item());
    status(Idle);

    // commit does not update payload, so we modify as well
    ItemModifyJob *modifyJob = new ItemModifyJob(createJob->item(), this);
    modifyJob->disableRevisionCheck();
}

void SugarCRMResource::deleteEntryResult(KJob *job)
{
    Q_ASSERT(mCurrentJob == job);
    mCurrentJob = nullptr;
    if (handleLoginError(job)) {
        return;
    }

    if (job->error() != 0) {
        const QString message = job->errorText();
        kWarning() << "error=" << job->error() << ":" << message;

        status(Broken, message);
        error(message);
        cancelTask(message);
        return;
    }

    DeleteEntryJob *deleteJob = qobject_cast<DeleteEntryJob *>(job);
    Q_ASSERT(deleteJob != nullptr);

    changeCommitted(deleteJob->item());
    status(Idle);
}

void SugarCRMResource::fetchEntryResult(KJob *job)
{
    Q_ASSERT(mCurrentJob == job);
    mCurrentJob = nullptr;
    if (handleLoginError(job)) {
        return;
    }

    if (job->error() != 0) {
        const QString message = job->errorText();
        kWarning() << "error=" << job->error() << ":" << message;

        status(Broken, message);
        error(message);
        cancelTask(message);
        return;
    }

    FetchEntryJob *fetchJob = qobject_cast<FetchEntryJob *>(job);
    Q_ASSERT(fetchJob != nullptr);

    itemRetrieved(fetchJob->item());
    status(Idle);
}

void SugarCRMResource::updateEntryResult(KJob *job)
{
    Q_ASSERT(mCurrentJob == job);
    mCurrentJob = nullptr;
    if (handleLoginError(job)) {
        return;
    }

    UpdateEntryJob *updateJob = qobject_cast<UpdateEntryJob *>(job);
    Q_ASSERT(updateJob != nullptr);

    if (job->error() != 0) {
        if (job->error() != UpdateEntryJob::ConflictError) {
            const QString message = job->errorText();
            kWarning() << "error=" << job->error() << ":" << message;

            status(Broken, message);
            error(message);
            cancelTask(message);
            return;
        }

        const Item localItem = updateJob->item();
        const Item remoteItem = updateJob->conflictItem();

        mConflictHandler->setConflictingItems(localItem, remoteItem);
        mConflictHandler->setDifferencesInterface(updateJob->module());
        mConflictHandler->setParentWindowId(winIdForDialogs());
        mConflictHandler->setParentName(name());
        mConflictHandler->start();
    } else {
        changeCommitted(updateJob->item());
        status(Idle);
    }
}

void SugarCRMResource::commitChange(const Akonadi::Item &item)
{
    changeCommitted(item);
    status(Idle);
}

void SugarCRMResource::updateOnBackend(const Akonadi::Item &item)
{
    const Collection collection = item.parentCollection();

    ModuleHandler *handler = mModuleHandlers->value(collection.remoteId());
    if (handler) {
        updateItem(item, handler);
    } else {
        kError() << "No module handler for collection" << collection.remoteId();
    }
}

void SugarCRMResource::updateItem(const Akonadi::Item &item, ModuleHandler *handler)
{
    UpdateEntryJob *job = new UpdateEntryJob(item, mSession, this);
    Q_ASSERT(!mCurrentJob);
    mCurrentJob = job;
    job->setModule(handler);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(updateEntryResult(KJob*)));
    job->start();
}

void SugarCRMResource::createModuleHandlers(const QStringList &availableModules)
{
    Q_FOREACH(const QString &module, availableModules) {
        // check if we have a corresponding module handler already
        // if not see if we can create one
        ModuleHandler* handler = mModuleHandlers->value(module);
        if (handler == nullptr) {
            if (module == QLatin1String("Contacts")) {
                handler = new ContactsHandler(mSession);
            } else if (module == QLatin1String("Accounts")) {
                handler = new AccountsHandler(mSession);
            } else if (module == QLatin1String("Opportunities")) {
                handler = new OpportunitiesHandler(mSession);
#if 0 // we don't use this, so skip it
            } else if (module == QLatin1String("Leads")) {
                handler = new LeadsHandler(mSession);
            } else if (module == QLatin1String("Campaigns")) {
                handler = new CampaignsHandler(mSession);
            } else if (module == QLatin1String("Tasks")) {
                handler = new TasksHandler(mSession);
#endif
            } else if (module == QLatin1String("Notes")) {
                handler = new NotesHandler(mSession);
            } else if (module == QLatin1String("Emails")) {
                handler = new EmailsHandler(mSession);
            } else if (module == QLatin1String("Documents")) {
                handler = new DocumentsHandler(mSession);
            } else {
                //kDebug() << "No module handler for" << module;
                continue;
            }

            handler->initialCheck();

            mModuleHandlers->insert(module, handler);

            // create a debug interface for the module, if we haven't done so already
            ModuleDebugInterface *debugInterface = mModuleDebugInterfaces->value(module);
            if (!debugInterface) {
                debugInterface = new ModuleDebugInterface(module, this);
                QDBusConnection::sessionBus().registerObject(QLatin1String("/CRMDebug/modules/") + module,
                                                             debugInterface,
                                                             QDBusConnection::ExportScriptableSlots);
                mModuleDebugInterfaces->insert(module, debugInterface);
            }
        }
    }

}

bool SugarCRMResource::handleLoginError(KJob *job)
{
    kDebug() << "job->error()=" << job->error();
    switch (job->error()) {
    case SugarJob::LoginError:
        kDebug() << "LoginError! Going to Broken state";
        setOnline( false );
        emit status( Broken, job->errorText() );
        // if this is any other job than an explicit login, defer to next attempt
        if (qobject_cast<LoginJob *>(job) == nullptr) {
            deferTask();
        } else {
            taskDone();
        }
        return true;
    case SugarJob::CouldNotConnectError: // transient error, try again later
        kDebug() << job->errorString();
        emit status( Idle, i18n( "Server is not available." ) );
        kDebug() << "deferring task";
        deferTask();
        setTemporaryOffline(300); // this calls doSetOnline(false)
        return true;
    case SugarJob::SoapError: // this could be transient too, e.g. capturing portal. Or it could be real...
        if (job->errorString() == "You do not have access") // that's when the object we're modifying has been deleted on the server meanwhile. Real error, let's move on.
            return false;
        emit status( Idle, job->errorString() );
        kDebug() << "deferring task";
        deferTask();
        setTemporaryOffline(300); // this calls doSetOnline(false)
        return true;
    default:
        // no error, or another kind of error, which the caller will have to handle
        return false;
    }
}

#include "sugarcrmresource.moc"
