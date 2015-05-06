#ifndef SUGARCRMRESOURCE_H
#define SUGARCRMRESOURCE_H

#include <akonadi/resourcebase.h>

#include <QStringList>

class ConflictHandler;
class KJob;
class ResourceDebugInterface;
class ModuleDebugInterface;
class ModuleHandler;
class SugarSession;
class LoginJob;
class SugarJob;

template <typename U, typename V> class QHash;

class SugarCRMResource : public Akonadi::ResourceBase, public Akonadi::AgentBase::Observer
{
    friend class ModuleDebugInterface;
    friend class ResourceDebugInterface;

    Q_OBJECT

public:
    SugarCRMResource(const QString &id);
    ~SugarCRMResource();

public Q_SLOTS:
    virtual void configure(WId windowId);

private:
    SugarSession *mSession;
    SugarJob *mCurrentJob; // do we ever run two jobs in parallel? in that case make it a list
    LoginJob *mLoginJob; // this one can happen in parallel, e.g. start listjob, setonline(false), setonline(true) -> LoginJob is created, and only afterwards the listjob finishes.

    ResourceDebugInterface *mDebugInterface;

    typedef QHash<QString, ModuleHandler *> ModuleHandlerHash;
    ModuleHandlerHash *mModuleHandlers;
    typedef QHash<QString, ModuleDebugInterface *> ModuleDebugInterfaceHash;
    ModuleDebugInterfaceHash *mModuleDebugInterfaces;

    ConflictHandler *mConflictHandler;
    bool mOnline;

private:
    void aboutToQuit();
    void doSetOnline(bool online);

    void itemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection);
    void itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts);
    void itemRemoved(const Akonadi::Item &item);

private Q_SLOTS:
    void retrieveCollections();
    void retrieveItems(const Akonadi::Collection &col);
    bool retrieveItem(const Akonadi::Item &item, const QSet<QByteArray> &parts);

    void startExplicitLogin();
    void explicitLoginResult(KJob *job);

    void listModulesResult(KJob *job);

    void slotTotalItems(int count);
    void itemsReceived(const Akonadi::Item::List &items);
    void deletedReceived(const Akonadi::Item::List &items);
    void listEntriesResult(KJob *job);

    void createEntryResult(KJob *job);

    void deleteEntryResult(KJob *job);

    void fetchEntryResult(KJob *job);

    void updateEntryResult(KJob *job);

    void commitChange(const Akonadi::Item &item);

    void updateOnBackend(const Akonadi::Item &item);

private:
    void updateItem(const Akonadi::Item &item, ModuleHandler *handler);
    void createModuleHandlers(const QStringList &availableModules);

    bool handleLoginError(KJob *job);
};

#endif
