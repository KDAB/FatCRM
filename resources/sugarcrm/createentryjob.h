#ifndef CREATEENTRYJOB_H
#define CREATEENTRYJOB_H

#include "sugarjob.h"

namespace Akonadi {
    class Item;
}

class ModuleHandler;
class QStringList;
class TNS__Get_entry_result;
class TNS__Set_entry_result;

class CreateEntryJob : public SugarJob
{
    Q_OBJECT

public:
    CreateEntryJob( const Akonadi::Item &item, SugarSession *session, QObject *parent = 0 );

    ~CreateEntryJob();

    void setModule( ModuleHandler *handler );

    Akonadi::Item item() const;

protected:
    void startSugarTask();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void setEntryDone( const TNS__Set_entry_result &callResult ) )
    Q_PRIVATE_SLOT( d, void setEntryError( const KDSoapMessage &fault ) )
    Q_PRIVATE_SLOT( d, void getEntryDone( const TNS__Get_entry_result &callResult ) )
    Q_PRIVATE_SLOT( d, void getEntryError( const KDSoapMessage &fault ) )
};

#endif
