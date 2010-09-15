#include "loginjob.h"

#include <KDebug>

LoginJob::LoginJob( SugarSession *session, QObject *parent )
    : SugarJob( session, parent )
{
    if ( !sessionId().isEmpty() ) {
        kWarning() << "Creating Login job when there is a valid session ID. Will not change anything";
    }
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