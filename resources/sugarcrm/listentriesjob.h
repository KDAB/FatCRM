#ifndef LISTENTRIESJOB_H
#define LISTENTRIESJOB_H

#include "sugarjob.h"

#include <akonadi/item.h>

namespace Akonadi {
    class Collection;
}

class ModuleHandler;
class QStringList;
class TNS__Get_entry_list_result;

class ListEntriesJob : public SugarJob
{
    Q_OBJECT

public:
    ListEntriesJob( const Akonadi::Collection &collection, SugarSession *session, QObject *parent = 0 );

    ~ListEntriesJob();

    void setModule( ModuleHandler *handler );

Q_SIGNALS:
    void itemsReceived( const Akonadi::Item::List &items );

protected:
    void startSugarTask();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void listEntriesDone( const TNS__Get_entry_list_result &callResult ) )
    Q_PRIVATE_SLOT( d, void listEntriesError( const KDSoapMessage &fault ) )
};

#endif
