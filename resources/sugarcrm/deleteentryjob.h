#ifndef DELETEENTRYJOB_H
#define DELETEENTRYJOB_H

#include "sugarjob.h"

namespace Akonadi
{
class Item;
}

class ModuleHandler;
namespace KDSoapGenerated
{
class TNS__Set_entry_result;
}

class DeleteEntryJob : public SugarJob
{
    Q_OBJECT

public:
    DeleteEntryJob(const Akonadi::Item &item, SugarSession *session, QObject *parent = 0);

    ~DeleteEntryJob();

    Akonadi::Item item() const;

protected:
    void startSugarTask();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void setEntryDone(const KDSoapGenerated::TNS__Set_entry_result &callResult))
    Q_PRIVATE_SLOT(d, void setEntryError(const KDSoapMessage &fault))
};

#endif
