/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sugarjob.h"

#include "sugarsession.h"
#include "sugarsoap.h"
#include "passwordhandler.h"

using namespace KDSoapGenerated;
#include <KDSoapClient/KDSoapMessage.h>
//#include <QCryptographicHash>

#include "sugarcrmresource_debug.h"
#include <KLocalizedString>

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
    void slotPasswordAvailable();
};

void SugarJob::Private::startLogin()
{
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << q;
    mTryRelogin = false;

    Sugarsoap *soap = mSession->soap();
    Q_ASSERT(soap != 0);

    if (!mSession->readPassword()) {
        // this can only happen when the user forcibly closes KWallet.
        q->setError(SugarJob::LoginError);
        q->emitResult();
        return;
    }

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
    soap->asyncLogin(userAuth, QLatin1String("FatCRM"));
}

void SugarJob::Private::loginDone(const KDSoapGenerated::TNS__Set_entry_result &callResult)
{
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << q << "error=" << callResult.error().number();
    const QString sessionId = callResult.id();

    QString message;
    if (sessionId.isEmpty()) {
        message = i18nc("@info:status", "server returned an empty session identifier");
    } else if (sessionId == QLatin1String("-1")) {
        message = i18nc("@info:status", "server returned an invalid session identifier");
    } else {
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << q << "Login (for" << q->metaObject()->className() << ") succeeded: sessionId=" << sessionId;
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
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << q << "faultcode=" << faultcode;
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

void SugarJob::Private::slotPasswordAvailable()
{
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG);
    startLogin();
}

SugarJob::SugarJob(SugarSession *session, QObject *parent)
    : KJob(parent), d(new Private(this, session))
{
    connect(session->soap(), SIGNAL(loginDone(KDSoapGenerated::TNS__Set_entry_result)),
            this, SLOT(loginDone(KDSoapGenerated::TNS__Set_entry_result)));
    connect(session->soap(), SIGNAL(loginError(KDSoapMessage)),
            this, SLOT(loginError(KDSoapMessage)));
    //qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << this;
}

SugarJob::~SugarJob()
{
    //qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << this;
    delete d;
}

void SugarJob::start()
{
    d->mTryRelogin = true;

    if (d->mSession->sessionId().isEmpty()) {
        if (d->mSession->passwordHandler()->isPasswordAvailable()) {
            QMetaObject::invokeMethod(this, "startLogin", Qt::QueuedConnection);
        } else {
            connect(d->mSession->passwordHandler(), SIGNAL(passwordAvailable()),
                    this, SLOT(slotPasswordAvailable()));
        }
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

bool SugarJob::doKill()
{
    return true;
}

bool SugarJob::handleError(const KDSoapGenerated::TNS__Error_value &errorValue)
{
    if (errorValue.number() == QLatin1String("0"))
        return false; // no error

    if (errorValue.number() == QLatin1String("10")) {
        // Invalid login error, meaning we need to log in again
        if (d->mTryRelogin) {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Got error 10, probably a session timeout, let's login again";
            QMetaObject::invokeMethod(this, "startLogin", Qt::QueuedConnection);
            // We'll retry the operation in loginDone()
            return true;
        }
    }
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << errorValue.number() << errorValue.description();
    setError(SugarJob::SoapError);
    setErrorText(errorValue.description());
    emitResult();
    return true;
}

// This is called when KDSoap sends a fault, e.g. "Fault code 99: Host unreachable"
bool SugarJob::handleLoginError(const KDSoapMessage &fault)
{
    // TODO check for error indicating that new login is required, e.g. network error.
    // No point in re-login on a real fault sent by the server.
    qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "fault" << fault.name() << fault.faultAsString() << "d->mTryRelogin=" << d->mTryRelogin;
    Q_UNUSED(fault);
    if (d->mTryRelogin) {
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
#include "moc_sugarjob.cpp"
