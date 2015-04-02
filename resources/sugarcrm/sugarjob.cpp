#include "sugarjob.h"

#include "sugarsession.h"
#include "sugarsoap.h"

using namespace KDSoapGenerated;
#include <KDSoapClient/KDSoapMessage.h>

#include <KDebug>
#include <KLocale>

#include <QNetworkReply>

class SugarJob::Private
{
    SugarJob *const q;
public:
    Private(SugarJob *parent, SugarSession *session)
        : q(parent), mSession(session), mTryRelogin(true)
    {
    }

public:
    SugarSession *mSession;
    bool mTryRelogin;

public: // slots
    void startLogin();

    void startTask()
    {
        q->startSugarTask();
    }

    void loginDone(const KDSoapGenerated::TNS__Set_entry_result &callResult);
    void loginError(const KDSoapMessage &fault);
};

void SugarJob::Private::startLogin()
{
    mTryRelogin = false;

    Sugarsoap *soap = mSession->soap();
    Q_ASSERT(soap != 0);

    const QString username = mSession->userName();
    const QString password = mSession->password();

    // TODO krake: SugarCRM docs say that login wants an MD5 hash but it only works with clear text
    // might depend on SugarCRM configuration
    // would have the additional advantage of not having to save the password in clear text

    //const QByteArray passwordHash = QCryptographicHash::hash( password.toUtf8(), QCryptographicHash::Md5 );
    const QByteArray passwordHash = password.toUtf8();

    KDSoapGenerated::TNS__User_auth userAuth;
    userAuth.setUser_name(username);
    userAuth.setPassword(QString::fromAscii(passwordHash));
    userAuth.setVersion(QLatin1String(".01"));

    mSession->setSessionId(QString());

    // results handled by slots loginDone() and loginError()
    soap->asyncLogin(userAuth, QLatin1String("SugarClient"));
}

void SugarJob::Private::loginDone(const KDSoapGenerated::TNS__Set_entry_result &callResult)
{
    const QString sessionId = callResult.id();

    QString message;
    if (sessionId.isEmpty()) {
        message = i18nc("@info:status", "server returned an empty session identifier");
    } else if (sessionId == QLatin1String("-1")) {
        message = i18nc("@info:status", "server returned an invalid session identifier");
    } else {
        kDebug() << "Login (for" << q->metaObject()->className() << ") succeeded: sessionId=" << sessionId;
        mSession->setSessionId(sessionId);
        q->setError(0);
        q->setErrorText(QString());
        q->startSugarTask();
        return;
    }

    q->setError(SugarJob::LoginError);
    q->setErrorText(i18nc("@info:status", "Login for user %1 on %2 failed: %3",
                          mSession->userName(), mSession->host(), message));
    q->emitResult();
}

void SugarJob::Private::loginError(const KDSoapMessage &fault)
{
    mSession->setSessionId(QString());

    const int faultcode = fault.childValues().child(QLatin1String("faultcode")).value().toInt();
    if (faultcode == QNetworkReply::UnknownNetworkError ||
            faultcode == QNetworkReply::HostNotFoundError) {
        q->setError(SugarJob::CouldNotConnectError);
    } else {
        q->setError(SugarJob::LoginError);
    }
    q->setErrorText(i18nc("@info:status", "Login for user %1 on %2 failed: %3",
                          mSession->userName(), mSession->host(), fault.faultAsString()));
    q->emitResult();
}

SugarJob::SugarJob(SugarSession *session, QObject *parent)
    : KJob(parent), d(new Private(this, session))
{
    connect(session->soap(), SIGNAL(loginDone(KDSoapGenerated::TNS__Set_entry_result)),
            this, SLOT(loginDone(KDSoapGenerated::TNS__Set_entry_result)));
    connect(session->soap(), SIGNAL(loginError(KDSoapMessage)),
            this, SLOT(loginError(KDSoapMessage)));
}

SugarJob::~SugarJob()
{
    delete d;
}

void SugarJob::start()
{
    d->mTryRelogin = true;

    if (d->mSession->sessionId().isEmpty()) {
        QMetaObject::invokeMethod(this, "startLogin", Qt::QueuedConnection);
    } else {
        QMetaObject::invokeMethod(this, "startTask", Qt::QueuedConnection);
    }
}

void SugarJob::restart()
{
    setAutoDelete(true);
    setError(0);
    setErrorText(QString());
    start();
}

bool SugarJob::handleLoginError(const KDSoapMessage &fault)
{
    // TODO check for error indicating that new login is required
    if (d->mTryRelogin) {
        Q_UNUSED(fault);
        QMetaObject::invokeMethod(this, "startLogin", Qt::QueuedConnection);
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
