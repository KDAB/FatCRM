#ifndef UPDATEENTRYJOB_H
#define UPDATEENTRYJOB_H

#include "sugarjob.h"

namespace Akonadi {
    class Item;
}

class ModuleHandler;
namespace KDSoapGenerated {
    class TNS__Get_entry_result;
    class TNS__Set_entry_result;
}

class UpdateEntryJob : public SugarJob
{
    Q_OBJECT

public:
    enum UpdateErrors {
        ConflictError = SugarJob::TaskError + 1
    };

    UpdateEntryJob( const Akonadi::Item &item, SugarSession *session, QObject *parent = 0 );

    ~UpdateEntryJob();

    void setModule( ModuleHandler *handler );
    ModuleHandler *module() const;

    Akonadi::Item item() const;

    Akonadi::Item conflictItem() const;

protected:
    void startSugarTask();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void getEntryDone( const KDSoapGenerated::TNS__Get_entry_result &callResult ) )
    Q_PRIVATE_SLOT( d, void getEntryError( const KDSoapMessage &fault ) )
    Q_PRIVATE_SLOT( d, void setEntryDone( const KDSoapGenerated::TNS__Set_entry_result &callResult ) )
    Q_PRIVATE_SLOT( d, void setEntryError( const KDSoapMessage &fault ) )
    Q_PRIVATE_SLOT( d, void getRevisionDone( const KDSoapGenerated::TNS__Get_entry_result &callResult ) )
    Q_PRIVATE_SLOT( d, void getRevisionError( const KDSoapMessage &fault ) )
};

#endif
