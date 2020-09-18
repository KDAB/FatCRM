/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "wsdl_sugar41.h"
#include "sugarsoapprotocol.h"
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
    void loginDone(const QString &sessionId);
    void loginError(int error, const QString &messageError);

public:
    SugarSession *mSession;
    bool mTryRelogin;

public: // slots
    void startLogin();

    void startTask()
    {
        q->startSugarTask();
    }

    void slotPasswordAvailable();

};

void SugarJob::Private::startLogin()
{
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << q;
    mTryRelogin = false;

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

    mSession->setSessionId(QString());

    if(mSession->protocol() == nullptr) {
        auto *protocol = new SugarSoapProtocol;
        protocol->setSession(mSession);
        mSession->setProtocol(protocol);
    }

    QString sessionId;
    QString errorMessage;
    const int result = mSession->protocol()->login(username, password, sessionId, errorMessage);
    if (result == KJob::NoError) {
        Private::loginDone(sessionId);
    } else {
        Private::loginError(result, errorMessage);
    }
}

void SugarJob::Private::loginDone(const QString &sessionId)
{
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

void SugarJob::Private::loginError(int error, const QString &messageError)
{
    mSession->setSessionId(QString());
    //kDebug() << q << "error=" << error;
    q->setError(error);
    q->setErrorText(messageError);
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
        if (d->mSession->passwordHandler() == nullptr || d->mSession->passwordHandler()->isPasswordAvailable()) {
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

bool SugarJob::handleConnectError(int error, const QString &errorMessage)
{
    Q_UNUSED(errorMessage);
    if (error == SugarJob::CouldNotConnectError) {
        if (d->mTryRelogin) {
            QMetaObject::invokeMethod(this, "startLogin", Qt::QueuedConnection);
            return true;
        }
    }
    return false;
}

SugarSession *SugarJob::session() const
{
    return d->mSession;
}

QString SugarJob::sessionId() const
{
    return d->mSession->sessionId();
}

bool SugarJob::shouldTryRelogin() const
{
    return d->mTryRelogin;
}

#include "moc_sugarjob.cpp"
