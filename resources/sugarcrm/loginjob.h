#ifndef LOGINJOB_H
#define LOGINJOB_H

#include "sugarjob.h"

class LoginJob : public SugarJob
{
    Q_OBJECT

public:
    explicit LoginJob( SugarSession *session, QObject *parent = 0 );

    ~LoginJob();

protected:
    void startSugarTask();
};

#endif