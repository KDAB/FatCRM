#include "loginjob.h"

LoginJob::LoginJob( SugarSession *session, QObject *parent )
    : SugarJob( session, parent )
{
}

LoginJob::~LoginJob()
{
}

void LoginJob::startSugarTask()
{
    // Don't have to do anything, base class already handles login
    emitResult();
}

#include "loginjob.moc"