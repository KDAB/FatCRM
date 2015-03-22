#ifndef LISTMODULESJOB_H
#define LISTMODULESJOB_H

#include "sugarjob.h"

class QStringList;

namespace KDSoapGenerated {
    class TNS__Module_list;
}

class ListModulesJob : public SugarJob
{
    Q_OBJECT

public:
    explicit ListModulesJob( SugarSession *session, QObject *parent = 0 );

    ~ListModulesJob();

    QStringList modules() const;

protected:
    void startSugarTask();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void listModulesDone( const KDSoapGenerated::TNS__Module_list &callResult ) )
    Q_PRIVATE_SLOT( d, void listModulesError( const KDSoapMessage &fault ) )
};

#endif
