#include "sugarsession.h"

#include "sugarsoap.h"

using namespace KDSoapGenerated;
#include <KUrl>

static QString endPointFromHostString( const QString &host )
{
    KUrl url( host );
    url.setFileName( QLatin1String( "soap.php" ) );
    url.setQuery( QString() );

    return url.url();
}

class SugarSession::Private
{
    SugarSession *const q;
public:
    explicit Private( SugarSession *parent )
        : q( parent ), mSoap( 0 )
    {
    }

public:
    QString mSessionId;
    QString mUserName;
    QString mPassword;
    QString mHost;
    Sugarsoap *mSoap;
};

SugarSession::SugarSession( QObject* parent )
    : QObject( parent ), d( new Private( this ) )
{
}

SugarSession::~SugarSession()
{
    delete d;
}

SugarSession::RequiredAction SugarSession::setSessionParameters( const QString &username, const QString &password, const QString &host )
{
    RequiredAction result = None;
    if ( d->mSoap != 0 ) {
        if ( host != d->mHost || username != d->mUserName ) {
            result = NewLogin;
        } else if ( password != d->mPassword ) {
            result = ReLogin;
        }
    } else {
        result = NewLogin;
    }

    d->mUserName = username;
    d->mPassword = password;
    d->mHost = host;

    return result;
}

void SugarSession::createSoapInterface()
{
    if (d->mSoap) {
        d->mSoap->disconnect();
        d->mSoap->deleteLater();
    }

    d->mSoap = new Sugarsoap;
    d->mSoap->setEndPoint( endPointFromHostString( d->mHost ) );
}

QString SugarSession::sessionId() const
{
    return d->mSessionId;
}

QString SugarSession::userName() const
{
    return d->mUserName;
}

QString SugarSession::password() const
{
    return d->mPassword;
}

QString SugarSession::host() const
{
    return d->mHost;
}

void SugarSession::logout()
{
    if ( !d->mSessionId.isEmpty() && d->mSoap != 0 ) {
        d->mSoap->logout( d->mSessionId );
    }
    d->mSessionId = QString();
}

void SugarSession::setSessionId( const QString &sessionId )
{
    d->mSessionId = sessionId;
}

Sugarsoap *SugarSession::soap()
{
    return d->mSoap;
}

#include "sugarsession.moc"
