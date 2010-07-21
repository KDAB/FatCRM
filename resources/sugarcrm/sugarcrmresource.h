#ifndef SUGARCRMRESOURCE_H
#define SUGARCRMRESOURCE_H

#include <akonadi/resourcebase.h>

#include <QStringList>

class KDSoapMessage;
class ModuleHandler;
class TNS__Get_entry_list_result;
class TNS__Get_entry_result;
class TNS__Module_list;
class TNS__Set_entry_result;
class Sugarsoap;

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
    Sugarsoap* mSoap;

    QString mSessionId;

    Akonadi::Item mPendingItem;

    QStringList mAvailableModules;
    typedef QHash<QString, ModuleHandler*> ModuleHandlerHash;
    ModuleHandlerHash *mModuleHandlers;

protected:
    void aboutToQuit();
    void doSetOnline( bool online );

    void doLogin();

    void itemAdded( const Akonadi::Item &item, const Akonadi::Collection &collection );
    void itemChanged( const Akonadi::Item &item, const QSet<QByteArray> &parts );
    void itemRemoved( const Akonadi::Item &item );

    void connectSoapProxy();

protected Q_SLOTS:
    void retrieveCollections();
    void retrieveItems( const Akonadi::Collection &col );
    bool retrieveItem( const Akonadi::Item &item, const QSet<QByteArray> &parts );

    void loginDone( const TNS__Set_entry_result &callResult );
    void loginError( const KDSoapMessage &fault );

    void getAvailableModulesDone( const TNS__Module_list &callResult );
    void getAvailableModulesError( const KDSoapMessage &fault );

    void getEntryListDone( const TNS__Get_entry_list_result &callResult );
    void getEntryListError( const KDSoapMessage &fault );

    void setEntryDone( const TNS__Set_entry_result &callResult );
    void setEntryError( const KDSoapMessage &fault );

    void getEntryDone( const TNS__Get_entry_result &callResult );
    void getEntryError( const KDSoapMessage &fault );
};

#endif
