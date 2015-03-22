#ifndef FETCHENTRYJOB_H
#define FETCHENTRYJOB_H

#include "sugarjob.h"

namespace Akonadi
{
class Item;
}

class ModuleHandler;
namespace KDSoapGenerated
{
class TNS__Get_entry_result;
}

class FetchEntryJob : public SugarJob
{
    Q_OBJECT

public:
    FetchEntryJob(const Akonadi::Item &item, SugarSession *session, QObject *parent = 0);

    ~FetchEntryJob();

    void setModule(ModuleHandler *handler);

    Akonadi::Item item() const;

protected:
    void startSugarTask();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void getEntryDone(const KDSoapGenerated::TNS__Get_entry_result &callResult))
    Q_PRIVATE_SLOT(d, void getEntryError(const KDSoapMessage &fault))
};

#endif
