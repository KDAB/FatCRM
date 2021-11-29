/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "sugarcrmresource_debug.h"

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
#include "kdcrmutils.h"
#include "leadshandler.h"
#include "listentriesjob.h"
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
#include "tests/sugarmockprotocol.h"

#include <Akonadi/ChangeRecorder>
#include <Akonadi/Collection>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/ItemModifyJob>
#include <Akonadi/CachePolicy>
#include <Akonadi/ItemDeleteJob>
#include <Akonadi/ItemFetchJob>

#include <KContacts/Addressee>

#include <KLocalizedString>
#include <KWindowSystem>
#include <kwindowsystem_version.h>

#include <QDebug>
#include <QtDBus/QDBusConnection>

using namespace Akonadi;

SugarCRMResource::SugarCRMResource(const QString &id)
    : ResourceBase(id),
      mPasswordHandler(new PasswordHandler(id, this)),
      mCurrentJob(nullptr),
      mLoginJob(nullptr),
      mDebugInterface(new ResourceDebugInterface(this)),
      mModuleHandlers(new ModuleHandlerHash),
      mModuleDebugInterfaces(new ModuleDebugInterfaceHash),
      mConflictHandler(new ConflictHandler(ConflictHandler::BackendConflict, this)),
      mOnline(false)
{
    KDCRMUtils::setupIconTheme();

    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG);
    new SettingsAdaptor(Settings::self());
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Settings"),
            Settings::self(),
            QDBusConnection::ExportAdaptors);

    QDBusConnection::sessionBus().registerObject(QStringLiteral("/CRMDebug"),
            mDebugInterface,
            QDBusConnection::ExportScriptableSlots);

    auto *itemTransferInterface = new ItemTransferInterface(this);
    QDBusConnection::sessionBus().registerObject(QLatin1String("/ItemTransfer"),
            itemTransferInterface,
            QDBusConnection::ExportScriptableSlots);

    setNeedsNetwork(true);
    setDisableAutomaticItemDeliveryDone(true);
    setAutomaticProgressReporting(false); // because our "totalItems" includes deleted items

    // make sure itemAdded() and itemChanged() get the full item from Akonadi before being called
    changeRecorder()->itemFetchScope().fetchFullPayload();

    // make sure these call have the collection available as well
    changeRecorder()->fetchCollection(true);

    QString selectedProtocol = Settings::protocol();

    SugarProtocolBase *protocol;
    if (selectedProtocol == "Mock") {
        auto *p = new SugarMockProtocol;
        p->addData();
        protocol = p;
        mSession = new SugarSession(nullptr, this);
        mSession->setSessionParameters("user", "password", "hosttest");
    } else if (selectedProtocol == "Empty Mock") {
        protocol = new SugarMockProtocol;
        mSession = new SugarSession(nullptr, this);
        mSession->setSessionParameters("user", "password", "hosttest");
    } else {
        mSession = new SugarSession(mPasswordHandler, this);
        mSession->setSessionParameters(Settings::user(), QString() /*password not read yet*/,
                                       Settings::host());
        mSession->createSoapInterface();
        protocol = new SugarSoapProtocol;
        if (selectedProtocol != "Soap") {
            qWarning() << "protocol name incorrect:" << selectedProtocol << "is an invalid protocol name";
        }
    }

    itemTransferInterface->setSession(mSession);

    protocol->setSession(mSession);
    mSession->setProtocol(protocol);

    connect(mConflictHandler, &ConflictHandler::commitChange,
            this, &SugarCRMResource::commitChange);
    connect(mConflictHandler, &ConflictHandler::updateOnBackend,
            this, &SugarCRMResource::updateOnBackend);

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
#if !defined(Q_OS_MACOS)
#if KWINDOWSYSTEM_VERSION >= QT_VERSION_CHECK(5,62,0)
    dialog.setAttribute(Qt::WA_NativeWindow, true);
    KWindowSystem::setMainWindow(dialog.windowHandle(), windowId);
#else
    KWindowSystem::setMainWindow(&dialog, windowId);
#endif
#endif

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
    Settings::self()->save();
    mPasswordHandler->setPassword(password);

    setName(accountName);

    emit configurationDialogAccepted();
}

void SugarCRMResource::aboutToQuit()
{
    // just a curtesy to the server
    mSession->protocol()->logout();
}

void SugarCRMResource::doSetOnline(bool online)
{
    // akonadiserver calls setOnline() multiple times with the same value...
    // let's only react to real changes here
    if (online != mOnline) {
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << online;
        mOnline = online;
        if (online) {
            if (Settings::host().isEmpty()) {
                const QString message = i18nc("@info:status", "No server configured");
                emit status(Broken, message);
                emit error(message);
            } else if (Settings::user().isEmpty()) {
                const QString message = i18nc("@info:status", "No user name configured");
                emit status(Broken, message);
                emit error(message);
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
        emit status(Running);

        auto *job = new CreateEntryJob(item, mSession, this);
        Q_ASSERT(!mCurrentJob);
        mCurrentJob = job;
        job->setModule(handler);
        connect(job, &KJob::result, this, &SugarCRMResource::createEntryResult);
        job->start();
    } else {
        const QString message = i18nc("@info:status", "Cannot add items to folder %1",
                                      collection.name());
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << message;

        emit status(Broken, message);
        cancelTask(message);
    }
}

void SugarCRMResource::itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts)
{
    //if (item.hasPayload<SugarOpportunity>()) {
    //    SugarOpportunity opp = item.payload<SugarOpportunity>();
    //    qDebug() << opp.id() << opp.name() << parts;
    //}

    // find the handler for the module represented by the given collection and let it
    // perform the respective "set entry" operation
    const Collection collection = item.parentCollection();
    ModuleHandler *handler = mModuleHandlers->value(collection.remoteId());
    if (handler) {
        if (!handler->needBackendChange(item, parts)) {
            qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "Handler for module" << handler->module()
                       << "indicates that backend change for item id=" << item.id()
                       << ", remoteId=" << item.remoteId()
                       << "is not required for given modified parts: " << parts;
            changeCommitted(item);
            return;
        }
        emit status(Running);

        updateItem(item, handler);
    } else {
        const QString message = i18nc("@info:status", "Cannot modify items in folder %1",
                                      collection.name());
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << message;

        emit status(Broken, message);
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

    emit status(Running);

#if 0
    const QString message = "disabled for safety reasons";
    emit status(Broken, message);
    cancelTask(message);
#else
    SugarJob *job = new DeleteEntryJob(item, mSession, nameToModule(collection.remoteId()), this);
    Q_ASSERT(!mCurrentJob);
    mCurrentJob = job;
    connect(job, &KJob::result, this, &SugarCRMResource::deleteEntryResult);
    job->start();
#endif
}

void SugarCRMResource::retrieveCollections()
{
    emit status(Running, i18nc("@info:status", "Retrieving folders"));

    SugarJob *job = new ListModulesJob(mSession, this);
    Q_ASSERT(!mCurrentJob);
    mCurrentJob = job;
    connect(job, &KJob::result, this, &SugarCRMResource::listModulesResult);
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

        auto *job = new ListEntriesJob(collection, mSession, this);
        job->setModule(handler);
        job->setLatestTimestamp(ListEntriesJob::latestTimestamp(collection, handler));
        Q_ASSERT(!mCurrentJob);
        mCurrentJob = job;

        const QString message = job->isUpdateJob()
                ? i18nc("@info:status", "Updating contents of folder %1", collection.name())
                : i18nc("@info:status", "Retrieving contents of folder %1", collection.name());
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << message;
        emit status(Running, message);
        emit percent(0);

        connect(job, &ListEntriesJob::totalItems, this, &SugarCRMResource::slotTotalItems);
        connect(job, &ListEntriesJob::progress, this, &SugarCRMResource::slotProgress);
        connect(job, &ListEntriesJob::itemsReceived, this, &SugarCRMResource::slotItemsReceived);
        connect(job, &KJob::result, this, &SugarCRMResource::listEntriesResult);
        job->start();
    } else {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "No module handler for collection" << collection;
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << mModuleHandlers->keys();
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
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << message;
        emit status(Running, message);

        auto *job = new FetchEntryJob(item, mSession, this);
        Q_ASSERT(!mCurrentJob);
        mCurrentJob = job;
        job->setModule(handler);
        connect(job, &KJob::result, this, &SugarCRMResource::fetchEntryResult);
        job->start();
        return true;
    } else {
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "No module handler for collection" << collection;
        return false;
    }
}

void SugarCRMResource::startExplicitLogin()
{
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG);
    Q_ASSERT(!mLoginJob); // didn't we kill it in doSetOnline(false) already?
    mLoginJob = new LoginJob(mSession, this);
    connect(mLoginJob, &KJob::result, this, &SugarCRMResource::explicitLoginResult);
    mLoginJob->start();
}

void SugarCRMResource::explicitLoginResult(KJob *job)
{
    Q_ASSERT(mLoginJob == job);
    mLoginJob = nullptr;
    if (job->error() == SugarJob::LoginError) {
        QString message = job->errorText();
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "error=" << job->error() << ":" << message;

        if (Settings::host().isEmpty()) {
            message = i18nc("@info:status", "No server configured");
        } else if (Settings::user().isEmpty()) {
            message = i18nc("@info:status", "No user name configured");
        } else {
            message = i18nc("@info:status", "Unable to login user %1 on %2: %3",
                            Settings::user(), Settings::host(), message);
        }

        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << message;
        emit status(Broken, message);
        cancelTask(message);
        return;
    }
    if (hasInvalidSessionError(job)) {
        qDebug() << "Retrying explicit login";
        startExplicitLogin();
        return;
    }
    if (handleError(job, CancelTaskOnError)) {
        return;
    }

    taskDone();
    emit status(Idle);
    synchronizeCollectionTree();
}

void SugarCRMResource::listModulesResult(KJob *job)
{
    Q_ASSERT(mCurrentJob == job);
    mCurrentJob = nullptr;

    if (hasInvalidSessionError(job)) {
        qDebug() << "Retrying listing modules";
        retrieveCollections();
        return;
    }
    if (handleError(job, CancelTaskOnError)) {
        return;
    }

    auto *listJob = qobject_cast<ListModulesJob *>(job);
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

            // Let's also get the fields for each module
            const KDSoapGenerated::TNS__Field_list fields = handler->listAvailableFields(mSession, module);
            if (handler->parseFieldList(collection, fields)) {
                handler->modifyCollection(collection);
            }

            collections << collection;
        }
    }

    Settings::setAvailableModules(availableModules);
    Settings::self()->save();

    collectionsRetrieved(collections);
    emit status(Idle);
}

void SugarCRMResource::slotTotalItems(int count)
{
    mTotalItems = count;
    //setTotalItems(count);
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
    auto *listEntriesJob = static_cast<ListEntriesJob *>(job);

    emit percent(100);

    Q_ASSERT(mCurrentJob == job);
    mCurrentJob = nullptr;

    if (hasInvalidSessionError(job)) {
        qDebug() << "Retrying to list the collection";
        retrieveItems(listEntriesJob->collection());
        return;
    }
    if (handleError(job, CancelTaskOnError)) {
        return;
    }

    if (listEntriesJob->isUpdateJob()) {
        // ensure the incremental mode is ON even if there were neither an update nor a delete
        itemsRetrievedIncremental(Item::List(), Item::List());
    }
    itemsRetrievalDone();

    // Commit attribute changes
    if (listEntriesJob->collectionAttributesChanged()) {
        listEntriesJob->module()->modifyCollection(listEntriesJob->collection());
    }

    // Next step: handle deleted items (must be done outside of the ItemSync)
    if (listEntriesJob->isUpdateJob() && !listEntriesJob->deletedItems().isEmpty()) {
        HandleDeletedItemsArg arg;
        arg.collection = listEntriesJob->collection();
        Q_ASSERT(arg.collection.isValid());
        arg.deletedItems = listEntriesJob->deletedItems();
        arg.module = listEntriesJob->module();
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Scheduling handleDeletedItems" << arg.collection.name();
        scheduleCustomTask(this, "handleDeletedItems", QVariant::fromValue(arg));
    }

    emit status(Idle);
}

void SugarCRMResource::handleDeletedItems(const QVariant &val)
{
    // If listing gives a SOAP error, don't hide it just because we went on with handling deleted items
    if (status() == Broken)
        return;

    const HandleDeletedItemsArg arg = val.value<HandleDeletedItemsArg>();

    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Handling" << arg.deletedItems.count() << "deleted items";
    // don't set mCurrentJob, can run in parallel to e.g. retrieveCollections()
    const QString message = i18nc("@info:status", "Retrieving deleted items for folder %1", arg.collection.name());
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << message;
    emit status(Running, message);

    // Check whether those items still exist, it's an error in akonadi to delete items that don't exist anymore.
    auto *fetchJob = new Akonadi::ItemFetchJob(arg.deletedItems, this);
    fetchJob->setCollection(arg.collection);
    fetchJob->fetchScope().fetchAllAttributes(false);
    fetchJob->fetchScope().fetchFullPayload(false);
    fetchJob->fetchScope().setCacheOnly(true);
    connect(fetchJob, &KJob::result, this, [this, fetchJob]() {
        if (fetchJob->error()) {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG()) << fetchJob->errorString();
            taskDone();
            emit status(Idle);
        } else {
            const Akonadi::Item::List items = fetchJob->items();
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG()) << "Resolved to" << items.size() << "items by the fetch job";
            auto *deleteJob = new Akonadi::ItemDeleteJob(items, this);
            connect(deleteJob, &KJob::result, this, &SugarCRMResource::slotDeleteEntriesResult);
        }
    });
}


void SugarCRMResource::slotDeleteEntriesResult(KJob *job)
{
    if (job->error()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << job->errorString();
    }

    taskDone();
    emit status(Idle);
}

void SugarCRMResource::createEntryResult(KJob *job)
{
    Q_ASSERT(mCurrentJob == job);
    mCurrentJob = nullptr;
    if (handleError(job, DeferTaskOnError)) {
        return;
    }

    auto *createJob = qobject_cast<CreateEntryJob *>(job);
    Q_ASSERT(createJob != nullptr);

    changeCommitted(createJob->item());
    emit status(Idle);

    // commit does not update payload, so we modify as well
    ItemModifyJob *modifyJob = new ItemModifyJob(createJob->item(), this);
    // this job will fail if the user has other changes pending
    // (e.g. because he created+modified the item while the resource was offline)
    // For that reason we also do this in updateEntryResult.
    modifyJob->disableRevisionCheck();
}

void SugarCRMResource::deleteEntryResult(KJob *job)
{
    Q_ASSERT(mCurrentJob == job);
    mCurrentJob = nullptr;
    if (handleError(job, DeferTaskOnError)) {
        return;
    }

    changeProcessed();
    emit status(Idle);
}

void SugarCRMResource::fetchEntryResult(KJob *job)
{
    auto *fetchJob = qobject_cast<FetchEntryJob *>(job);

    Q_ASSERT(mCurrentJob == job);
    mCurrentJob = nullptr;

    if (hasInvalidSessionError(job)) {
        qDebug() << "Retrying to list the collection";
        retrieveItem(fetchJob->item(), {});
        return;
    }

    if (handleError(job, CancelTaskOnError)) {
        return;
    }

    itemRetrieved(fetchJob->item());
    emit status(Idle);
}

void SugarCRMResource::updateEntryResult(KJob *job)
{
    Q_ASSERT(mCurrentJob == job);
    mCurrentJob = nullptr;

    auto *updateJob = qobject_cast<UpdateEntryJob *>(job);
    Q_ASSERT(updateJob != nullptr);

    if (job->error() == UpdateEntryJob::ConflictError) {
        const Item localItem = updateJob->item();
        const Item remoteItem = updateJob->conflictItem();

        mConflictHandler->setConflictingItems(localItem, remoteItem);
        mConflictHandler->setDifferencesInterface(updateJob->module());
        mConflictHandler->setParentWindowId(winIdForDialogs());
        mConflictHandler->setParentName(name());
        mConflictHandler->start();
        return;
    }

    if (handleError(job, DeferTaskOnError)) {
        return;
    }

    changeCommitted(updateJob->item()); // this clears the dirty flag of the item in the DB
    emit status(Idle);

    // Save the date_modified change (see also the comment in createEntryResult)
    ItemModifyJob *modifyJob = new ItemModifyJob(updateJob->item(), this);
    modifyJob->disableRevisionCheck();
}

void SugarCRMResource::commitChange(const Akonadi::Item &item)
{
    changeCommitted(item);
    emit status(Idle);
}

void SugarCRMResource::updateOnBackend(const Akonadi::Item &item)
{
    const Collection collection = item.parentCollection();

    ModuleHandler *handler = mModuleHandlers->value(collection.remoteId());
    if (handler) {
        updateItem(item, handler);
    } else {
        qCCritical(FATCRM_SUGARCRMRESOURCE_LOG) << "No module handler for collection" << collection.remoteId();
    }
}

void SugarCRMResource::updateItem(const Akonadi::Item &item, ModuleHandler *handler)
{
    auto *job = new UpdateEntryJob(item, mSession, this);
    Q_ASSERT(!mCurrentJob);
    mCurrentJob = job;
    job->setModule(handler);
    connect(job, &KJob::result, this, &SugarCRMResource::updateEntryResult);
    job->start();
}

void SugarCRMResource::createModuleHandlers(const QStringList &availableModules)
{
    Q_FOREACH(const QString &name, availableModules) {
        Module module = nameToModule(name);
        // check if we have a corresponding module handler already
        // if not see if we can create one
        ModuleHandler* handler = mModuleHandlers->value(moduleToName(module));
        if (handler == nullptr) {
            if (module == Module::Contacts) {
                handler = new ContactsHandler(mSession);
            } else if (module == Module::Accounts) {
                auto *accountsHandler = new AccountsHandler(mSession);
                accountsHandler->fillAccountsCache();
                handler = accountsHandler;
            } else if (module == Module::Opportunities) {
                handler = new OpportunitiesHandler(mSession);
#if 0 // we don't use this, so skip it
            } else if (module == ModuleName::Leads) {
                handler = new LeadsHandler(mSession);
            } else if (module == ModuleName::Campaigns) {
                handler = new CampaignsHandler(mSession);
            } else if (module == ModuleName::Tasks) {
                handler = new TasksHandler(mSession);
#endif
            } else if (module == Module::Notes) {
                handler = new NotesHandler(mSession);
            } else if (module == Module::Emails) {
                handler = new EmailsHandler(mSession);
            } else if (module == Module::Documents) {
                handler = new DocumentsHandler(mSession);
            } else {
                //qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "No module handler for" << module;
                continue;
            }

            handler->initialCheck();

            mModuleHandlers->insert(moduleToName(module), handler);

            // create a debug interface for the module, if we haven't done so already
            ModuleDebugInterface *debugInterface = mModuleDebugInterfaces->value(moduleToName(module));
            if (!debugInterface) {
                debugInterface = new ModuleDebugInterface(module, this);
                QDBusConnection::sessionBus().registerObject(QLatin1String("/CRMDebug/modules/") + moduleToName(module),
                                                             debugInterface,
                                                             QDBusConnection::ExportScriptableSlots);
                mModuleDebugInterfaces->insert(moduleToName(module), debugInterface);
            }
        }
    }

}

bool SugarCRMResource::hasInvalidSessionError(KJob *job)
{
    if (job->error() == SugarJob::SoapError &&
            (job->errorString().contains(QLatin1String("The session ID is invalid")) || job->errorString().contains(QLatin1String("Invalid Session ID")))) {
        mSession->forgetSession();
        return true;
    }
    return false;
}

// We want to defer anything that has a change of working if we try again,
// and especially any task which makes changes on the server, so they don't get lost.
// On the other hand, we should never defer a SyncCollection, that's unsupported (!)
//
// Returns true on error, false otherwise.
bool SugarCRMResource::handleError(KJob *job, ActionOnError action)
{
    auto deferOrCancel = [action, this](const QString &message){
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << message << "=> we have to" << (action == CancelTaskOnError ? "cancel" : "defer");
        if (action == CancelTaskOnError) {
            emit status(Broken, message);
            cancelTask(message);
        } else {
            emit status(Idle, message);
            emit error(message);
            deferTask();
        }
    };
    if (job->error()) {
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "job->error()=" << job->error() << job->errorString();
    }
    switch (job->error()) {
    case KJob::NoError:
        return false;
    case SugarJob::LoginError:
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "LoginError! Going to Broken state";
        setOnline( false );
        deferOrCancel(job->errorText());
        break;
    case SugarJob::CouldNotConnectError: // transient error, try again later
        emit status( Idle, i18n( "Server is not available." ) );
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "deferring task";
        deferOrCancel(job->errorText());
        if (action == DeferTaskOnError)
            setTemporaryOffline(300); // this calls doSetOnline(false)
        break;
    case SugarJob::SoapError: // this could be transient too, e.g. capturing portal. Or it could be real...
        if (job->errorString() == QLatin1String("You do not have access")) { // that's when the object we're modifying has been deleted on the server meanwhile. Real error, let's move on.
            // No point in trying this one again, it has to be cancelled.
            cancelTask(job->errorString());
        } else if (hasInvalidSessionError(job)) {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Forgetting invalid session ID, the next attempt will login again";
            deferOrCancel(job->errorText());
        } else {
            deferOrCancel(job->errorText());
            if (action == DeferTaskOnError) {
                qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "deferring task";
                setTemporaryOffline(300); // this calls doSetOnline(false)
            }
        }
        break;
    default:
        deferOrCancel(job->errorText());
        break;
    }
    return true;
}

