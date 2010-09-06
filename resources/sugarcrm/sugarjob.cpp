#include "sugarjob.h"

#include "sugarsession.h"
#include "sugarsoap.h"

#include <KDSoapMessage.h>

#include <KDebug>
#include <KLocale>

class SugarJob::Private
{
    SugarJob *const q;
public:
    Private( SugarJob *parent, SugarSession* session )
        : q( parent ), mSession( session )
    {
    }

public:
    SugarSession* mSession;

public: // slots
    void startLogin();

    void startTask()
    {
        q->startSugarTask();
    }

    void loginDone( const TNS__Set_entry_result &callResult );
    void loginError( const KDSoapMessage &fault );
};

void SugarJob::Private::startLogin()
{
    Sugarsoap *soap = mSession->soap();
    Q_ASSERT( soap != 0 );

    const QString username = mSession->userName();
    const QString password = mSession->password();

    // TODO krake: SugarCRM docs say that login wants an MD5 hash but it only works with clear text
    // might depend on SugarCRM configuration
    // would have the additional advantage of not having to save the password in clear text

    //const QByteArray passwordHash = QCryptographicHash::hash( password.toUtf8(), QCryptographicHash::Md5 );
    const QByteArray passwordHash = password.toUtf8();

    TNS__User_auth userAuth;
    userAuth.setUser_name( username );
    userAuth.setPassword( QString::fromAscii( passwordHash ) );
    userAuth.setVersion( QLatin1String( ".01"  ) );

    mSession->setSessionId( QString() );

    // results handled by slots loginDone() and loginError()
    soap->asyncLogin( userAuth, QLatin1String( "SugarClient" ) );
}

void SugarJob::Private::loginDone( const TNS__Set_entry_result &callResult )
{
    const QString sessionId = callResult.id();

    QString message;
    if ( sessionId.isEmpty() ) {
        message = i18nc( "@info:status", "server returned an empty session identifier" );
    } else if ( sessionId == QLatin1String( "-1" ) ) {
        message = i18nc( "@info:status", "server returned an invalid session identifier" );
    } else {
        kDebug() << "Login succeeded: sessionId=" << sessionId;
        mSession->setSessionId( sessionId );
        q->startSugarTask();
        return;
    }

    q->setError( LoginError );
    q->setErrorText( i18nc( "@info:status", "Login for user %1 on %2 failed: %3",
                            mSession->userName(), mSession->host(), message ) );
    q->emitResult();
}

void SugarJob::Private::loginError( const KDSoapMessage &fault )
{
    q->setError( SugarJob::LoginError );
    q->setErrorText( i18nc( "@info:status", "Login for user %1 on %2 failed: %3",
                            mSession->userName(), mSession->host(), fault.faultAsString() ) );
    q->emitResult();
}

SugarJob::SugarJob( SugarSession* session, QObject* parent )
    : KJob( parent ), d( new Private( this, session ) )
{
    connect( session->soap(), SIGNAL( loginDone( TNS__Set_entry_result ) ),
             this, SLOT( loginDon( TNS__Set_entry_result ) ) );
    connect( session->soap(), SIGNAL( loginError( KDSoapMessage ) ),
             this, SLOT( loginError( KDSoapMessage ) ) );
}

SugarJob::~SugarJob()
{
    delete d;
}

void SugarJob::start()
{
    if ( d->mSession->sessionId().isEmpty() ) {
        QMetaObject::invokeMethod( this, "startLogin", Qt::QueuedConnection );
    } else {
        QMetaObject::invokeMethod( this, "startTask", Qt::QueuedConnection );
    }
}

bool SugarJob::handleLoginError( const KDSoapMessage &fault )
{
    // TODO check for error indicating that new login is required
    if ( true ) {
        Q_UNUSED( fault );
        QMetaObject::invokeMethod( this, "startLogin", Qt::QueuedConnection );
        return true;
    }

    return false;
}

QString SugarJob::sessionId() const
{
    return d->mSession->sessionId();
}

Sugarsoap *SugarJob::soap()
{
    return d->mSession->soap();
}

#include "sugarjob.moc"
