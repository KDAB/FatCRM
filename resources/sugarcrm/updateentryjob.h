#ifndef UPDATEENTRYJOB_H
#define UPDATEENTRYJOB_H

#include "sugarjob.h"

namespace Akonadi {
    class Item;
}

class ModuleHandler;
class QStringList;
class TNS__Get_entry_result;
class TNS__Set_entry_result;

class UpdateEntryJob : public SugarJob
{
    Q_OBJECT

public:
    UpdateEntryJob( const Akonadi::Item &item, SugarSession *session, QObject *parent = 0 );

    ~UpdateEntryJob();

    void setModule( ModuleHandler *handler );
    ModuleHandler *module() const;

    Akonadi::Item item() const;

    bool hasConflict() const;

    Akonadi::Item conflictItem() const;

protected:
    void startSugarTask();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void getEntryDone( const TNS__Get_entry_result &callResult ) )
    Q_PRIVATE_SLOT( d, void getEntryError( const KDSoapMessage &fault ) )
    Q_PRIVATE_SLOT( d, void setEntryDone( const TNS__Set_entry_result &callResult ) )
    Q_PRIVATE_SLOT( d, void setEntryError( const KDSoapMessage &fault ) )
};

#endif
