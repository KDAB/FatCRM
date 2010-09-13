#ifndef SUGARCRMRESOURCE_H
#define SUGARCRMRESOURCE_H

#include <akonadi/resourcebase.h>

#include <QStringList>

class ConflictHandler;
class KJob;
class ModuleHandler;
class SugarSession;

template <typename U, typename V> class QHash;

class SugarCRMResource : public Akonadi::ResourceBase, public Akonadi::AgentBase::Observer
{
    friend class ModuleDebugInterface;
    friend class ResourceDebugInterface;

    Q_OBJECT

public:
    SugarCRMResource( const QString &id );
    ~SugarCRMResource();

public Q_SLOTS:
    virtual void configure( WId windowId );

protected:
    SugarSession *mSession;

    QStringList mAvailableModules;
    typedef QHash<QString, ModuleHandler*> ModuleHandlerHash;
    ModuleHandlerHash *mModuleHandlers;

    ConflictHandler *mConflictHandler;

protected:
    void aboutToQuit();
    void doSetOnline( bool online );

    void itemAdded( const Akonadi::Item &item, const Akonadi::Collection &collection );
    void itemChanged( const Akonadi::Item &item, const QSet<QByteArray> &parts );
    void itemRemoved( const Akonadi::Item &item );

protected Q_SLOTS:
    void retrieveCollections();
    void retrieveItems( const Akonadi::Collection &col );
    bool retrieveItem( const Akonadi::Item &item, const QSet<QByteArray> &parts );

    void explicitLoginResult( KJob *job );

    void listModulesResult( KJob *job );

    void itemsReceived( const Akonadi::Item::List &items );
    void deletedReceived( const Akonadi::Item::List &items );
    void listEntriesResult( KJob *job );

    void createEntryResult( KJob *job );

    void deleteEntryResult( KJob *job );

    void updateEntryResult( KJob *job );

    void commitChange( const Akonadi::Item &item );

    void updateOnBackend( const Akonadi::Item &item );

    void userOrHostChanged( const QString &user, const QString &host );

private:
    void updateItem( const Akonadi::Item &item, ModuleHandler *handler );

    bool handleLoginError( KJob *job );
};

#endif
