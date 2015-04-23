#ifndef LISTENTRIESJOB_H
#define LISTENTRIESJOB_H

#include "sugarjob.h"

#include <akonadi/item.h>

namespace Akonadi
{
class Collection;
}

class ModuleHandler;
namespace KDSoapGenerated
{
class TNS__Get_entries_count_result;
class TNS__Get_entry_list_result;
}

class ListEntriesJob : public SugarJob
{
    Q_OBJECT

public:
    ListEntriesJob(const Akonadi::Collection &collection, SugarSession *session, QObject *parent = 0);

    ~ListEntriesJob();

    Akonadi::Collection collection() const;
    void setModule(ModuleHandler *handler);

    QString latestTimestamp() const;

Q_SIGNALS:
    void totalItems(int count);
    void itemsReceived(const Akonadi::Item::List &items);
    void deletedReceived(const Akonadi::Item::List &items);

protected:
    void startSugarTask();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void getEntriesCountDone(const KDSoapGenerated::TNS__Get_entries_count_result &callResult))
    Q_PRIVATE_SLOT(d, void getEntriesCountError(const KDSoapMessage &fault))
    Q_PRIVATE_SLOT(d, void listEntriesDone(const KDSoapGenerated::TNS__Get_entry_list_result &callResult))
    Q_PRIVATE_SLOT(d, void listEntriesError(const KDSoapMessage &fault))
};

#endif
