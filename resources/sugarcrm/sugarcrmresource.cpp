#include "sugarcrmresource.h"

#include "accountshandler.h"
#include "campaignshandler.h"
#include "conflicthandler.h"
#include "contactshandler.h"
#include "createentryjob.h"
#include "deleteentryjob.h"
#include "fetchentryjob.h"
#include "leadshandler.h"
#include "listentriesjob.h"
#include "listmodulesjob.h"
#include "loginerrordialog.h"
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
#include <akonadi/itemmodifyjob.h>
#include <akonadi/entityannotationsattribute.h>

#include <kabc/addressee.h>

#include <KLocale>
#include <KWindowSystem>

#include <QtDBus/QDBusConnection>

using namespace Akonadi;

SugarCRMResource::SugarCRMResource(const QString &id)
    : ResourceBase(id),
      mSession(new SugarSession(this)),
      mModuleHandlers(new ModuleHandlerHash),
      mModuleDebugInterfaces(new ModuleDebugInterfaceHash),
      mConflictHandler(new ConflictHandler(ConflictHandler::BackendConflict, this))
{
    new SettingsAdaptor(Settings::self());
    QDBusConnection::sessionBus().registerObject(QLatin1String("/Settings"),
            Settings::self(),
            QDBusConnection::ExportAdaptors);

    ResourceDebugInterface *debugInterface = new ResourceDebugInterface(this);
    QDBusConnection::sessionBus().registerObject(QLatin1String("/CRMDebug"),
            debugInterface,
            QDBusConnection::ExportScriptableSlots);

    setNeedsNetwork(true);

    // make sure itemAdded() and itemChanged() get the full item from Akonadi before being called
    changeRecorder()->itemFetchScope().fetchFullPayload();

    // make sure these call have the collection available as well
    changeRecorder()->fetchCollection(true);

    mSession->setSessionParameters(Settings::self()->user(), Settings::self()->password(),
                                   Settings::self()->host());
    mSession->createSoapInterface();

    connect(mConflictHandler, SIGNAL(commitChange(Akonadi::Item)),
            this, SLOT(commitChange(Akonadi::Item)));
    connect(mConflictHandler, SIGNAL(updateOnBackend(Akonadi::Item)),
            this, SLOT(updateOnBackend(Akonadi::Item)));
}

SugarCRMResource::~SugarCRMResource()
{
    qDeleteAll(*mModuleHandlers);
    delete mModuleHandlers;
    delete mModuleDebugInterfaces; // interface instances destroyed by parent QObject
}

void SugarCRMResource::configure(WId windowId)
{
    SugarConfigDialog dialog(Settings::self(), name());

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

    Settings::self()->setHost(host);
    Settings::self()->setUser(user);
    Settings::self()->setPassword(password);
    Settings::self()->writeConfig();

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
    if (online) {
        if (Settings::self()->host().isEmpty()) {
            const QString message = i18nc("@info:status", "No server configured");
            status(Broken, message);
            error(message);
        } else if (Settings::self()->user().isEmpty()) {
            const QString message = i18nc("@info:status", "No user name configured");
            status(Broken, message);
            error(message);
        } else {
            // schedule login as a prepended custom task to hold all other until finished
            scheduleCustomTask(this, "startExplicitLogin", QVariant(), ResourceBase::Prepend);
        }
    } else {
        mSession->logout();
    }

    ResourceBase::doSetOnline(online);
}

void SugarCRMResource::itemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection)
{
    // find the handler for the module represented by the given collection and let it
    // perform the respective "set entry" operation
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(collection.remoteId());
    if (moduleIt != mModuleHandlers->constEnd()) {
        status(Running);

        CreateEntryJob *job = new CreateEntryJob(item, mSession, this);
        job->setModule(*moduleIt);
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
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(collection.remoteId());
    if (moduleIt != mModuleHandlers->constEnd()) {
        if (!(*moduleIt)->needBackendChange(item, parts)) {
            kWarning() << "Handler for module" << (*moduleIt)->moduleName()
                       << "indicates that backend change for item id=" << item.id()
                       << ", remoteId=" << item.remoteId()
                       << "is not required for given modified parts: " << parts;
            changeCommitted(item);
            return;
        }
        status(Running);

        updateItem(item, *moduleIt);
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

    SugarJob *job = new DeleteEntryJob(item, mSession, this);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(deleteEntryResult(KJob*)));
    job->start();
}

void SugarCRMResource::retrieveCollections()
{
    status(Running, i18nc("@info:status", "Retrieving folders"));

    SugarJob *job = new ListModulesJob(mSession, this);
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
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(collection.remoteId());
    if (moduleIt != mModuleHandlers->constEnd()) {
        const QString message = i18nc("@info:status", "Retrieving contents of folder %1",
                                      collection.name());
        kDebug() << message;
        status(Running, message);

        // getting items in batches
        setItemStreamingEnabled(true);

        ListEntriesJob *job = new ListEntriesJob(collection, mSession, this);
        job->setModule(*moduleIt);
        connect(job, SIGNAL(itemsReceived(Akonadi::Item::List)),
                this, SLOT(itemsReceived(Akonadi::Item::List)));
        connect(job, SIGNAL(deletedReceived(Akonadi::Item::List)),
                this, SLOT(deletedReceived(Akonadi::Item::List)));
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
    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(collection.remoteId());
    if (moduleIt != mModuleHandlers->constEnd()) {
        const QString message = i18nc("@info:status", "Retrieving entry from folder %1",
                                      collection.name());
        kDebug() << message;
        status(Running, message);

        FetchEntryJob *job = new FetchEntryJob(item, mSession, this);
        job->setModule(*moduleIt);
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
    SugarJob *job = new LoginJob(mSession, this);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(explicitLoginResult(KJob*)));
    job->start();
}

void SugarCRMResource::explicitLoginResult(KJob *job)
{
    if (handleLoginError(job)) {
        return;
    }

    if (job->error() != 0) {
        QString message = job->errorText();
        kWarning() << "error=" << job->error() << ":" << message;

        if (Settings::host().isEmpty()) {
            message = i18nc("@info:status", "No server configured");
        } else if (Settings::self()->user().isEmpty()) {
            message = i18nc("@info:status", "No user name configured");
        } else {
            message = i18nc("@info:status", "Unable to login user %1 on %2: %3",
                            Settings::self()->user(), Settings::self()->host(), message);
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
    Q_ASSERT(listJob != 0);

    Collection::List collections;

    Collection topLevelCollection;
    topLevelCollection.setRemoteId(identifier());
    topLevelCollection.setName(name());
    topLevelCollection.setParentCollection(Collection::root());

    // Our top level collection only contains other collections (no items) and cannot be
    // modified by clients
    topLevelCollection.setContentMimeTypes(QStringList() << Collection::mimeType());
    topLevelCollection.setRights(Collection::ReadOnly);

    collections << topLevelCollection;

    QSet<QString> availableHandlers;
    mAvailableModules.clear();
    Q_FOREACH (const QString &module, listJob->modules()) {
        mAvailableModules << module;

        // check if we already created one for the module
        ModuleDebugInterfaceHash::const_iterator debugIt = mModuleDebugInterfaces->constFind(module);
        if (debugIt == mModuleDebugInterfaces->constEnd()) {
            ModuleDebugInterface *debugInterface = new ModuleDebugInterface(module, this);
            QDBusConnection::sessionBus().registerObject(QLatin1String("/CRMDebug/modules/") + module,
                    debugInterface,
                    QDBusConnection::ExportScriptableSlots);
            mModuleDebugInterfaces->insert(module, debugInterface);
        }

        Collection collection;

        // check if we have a corresponding module handler already
        // if not see if we can create one
        ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(module);
        if (moduleIt != mModuleHandlers->constEnd()) {
            collection = moduleIt.value()->collection();
            moduleIt.value()->resetLatestTimestamp();
        } else {
            ModuleHandler *handler = 0;
            if (module == QLatin1String("Contacts")) {
                handler = new ContactsHandler(mSession);
            } else if (module == QLatin1String("Accounts")) {
                handler = new AccountsHandler(mSession);
            } else if (module == QLatin1String("Opportunities")) {
                handler = new OpportunitiesHandler(mSession);
            } else if (module == QLatin1String("Leads")) {
                handler = new LeadsHandler(mSession);
            } else if (module == QLatin1String("Campaigns")) {
                handler = new CampaignsHandler(mSession);
            } else {
                //kDebug() << "No module handler for" << module;
                continue;
            }
            mModuleHandlers->insert(module, handler);

            collection = handler->collection();

            /* This would be a (hackish?) way to send the user id of the current user
             * (to be retrieved by a User_listJob with query user_name = 'dfaure')
             * to the Sugar client, for filtering "my opps"...
             * Tried it on the parent collection, but it's not fetched right now...
            EntityAnnotationsAttribute* annotationsAttribute = new EntityAnnotationsAttribute;
            annotationsAttribute->insert("userid", "12345");
            collection.addAttribute(annotationsAttribute);
            */
        }
        availableHandlers << module;

        collection.setParentCollection(topLevelCollection);
        collections << collection;
    }

    Q_FOREACH (const QString &module, mAvailableModules) {
        if (!availableHandlers.contains(module)) {
            ModuleHandler *handler = mModuleHandlers->take(module);
            delete handler;
        }
    }

    collectionsRetrieved(collections);
    status(Idle);
}

void SugarCRMResource::itemsReceived(const Item::List &items)
{
    itemsRetrievedIncremental(items, Item::List());
}

void SugarCRMResource::deletedReceived(const Item::List &items)
{
    itemsRetrievedIncremental(Item::List(), items);
}

void SugarCRMResource::listEntriesResult(KJob *job)
{
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

    // ensure the incremental mode is ON even if there were neither an update nor a delete
    itemsRetrievedIncremental(Item::List(), Item::List());

    itemsRetrievalDone();
    status(Idle);
}

void SugarCRMResource::createEntryResult(KJob *job)
{
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
    Q_ASSERT(createJob != 0);

    changeCommitted(createJob->item());
    status(Idle);

    // commit does not update payload, so we modify as well
    ItemModifyJob *modifyJob = new ItemModifyJob(createJob->item(), this);
    modifyJob->disableRevisionCheck();
}

void SugarCRMResource::deleteEntryResult(KJob *job)
{
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
    Q_ASSERT(deleteJob != 0);

    changeCommitted(deleteJob->item());
    status(Idle);
}

void SugarCRMResource::fetchEntryResult(KJob *job)
{
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
    Q_ASSERT(fetchJob != 0);

    itemRetrieved(fetchJob->item());
    status(Idle);
}

void SugarCRMResource::updateEntryResult(KJob *job)
{
    if (handleLoginError(job)) {
        return;
    }

    UpdateEntryJob *updateJob = qobject_cast<UpdateEntryJob *>(job);
    Q_ASSERT(updateJob != 0);

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

    ModuleHandlerHash::const_iterator moduleIt = mModuleHandlers->constFind(collection.remoteId());
    if (moduleIt != mModuleHandlers->constEnd()) {
        updateItem(item, *moduleIt);
    } else {
        kError() << "No module handler for collection" << collection.remoteId();
    }
}

void SugarCRMResource::updateItem(const Akonadi::Item &item, ModuleHandler *handler)
{
    UpdateEntryJob *job = new UpdateEntryJob(item, mSession, this);
    job->setModule(handler);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(updateEntryResult(KJob*)));
    job->start();
}

bool SugarCRMResource::handleLoginError(KJob *job)
{
    if (job->error() == SugarJob::LoginError) {
        setOnline( false );
        emit status( Broken, job->errorText() );
        // if this is any other job than an explicit login, defer to next attempt
        if (qobject_cast<LoginJob *>(job) == 0) {
            deferTask();
        } else {
            taskDone();
        }
    } else if (job->error() == SugarJob::CouldNotConnectError) {
        emit status( Idle, i18n( "Server is not available." ) );
        deferTask();
        setTemporaryOffline(300);
    } else {
        return false;
    }

    // The popup is really annoying when simply not having internet access
    // Let's try to do like the imap resource, with the code above.
#if 0
    const QString message = job->errorText();
    kWarning() << "error=" << job->error() << ":" << message;

    LoginErrorDialog dialog(job, mSession, this);

    WId windowId = winIdForDialogs();
    if (windowId != 0) {
        KWindowSystem::setMainWindow(&dialog, windowId);
    }

    if (dialog.exec() == QDialog::Rejected) {
        setOnline(false);

        status(Broken, message);
        error(message);

        // if this is any other job than an explicit login, defer to next attempt
        if (qobject_cast<LoginJob *>(job) == 0) {
            deferTask();
        } else {
            taskDone();
        }
    } else {
        // handleLoginError is called in the jobs result slot
        // we can not restart before processing that has ended, otherwise
        // auto delete will delete the job prematurely
        QMetaObject::invokeMethod(job, "restart", Qt::QueuedConnection);
    }
#endif

    return true;
}

AKONADI_RESOURCE_MAIN(SugarCRMResource)

#include "sugarcrmresource.moc"
