/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sugarsession.h"

#include "wsdl_sugar41.h"
#include "passwordhandler.h"
#include "sugarprotocolbase.h"
#include "sugarcrmresource_debug.h"

#include <QUrl>

class SugarSession::Private
{
public:
    explicit Private(PasswordHandler *passwordHandler)
        : mPasswordHandler(passwordHandler)
    {
    }

    ~Private()
    {
        delete mProtocol;
    }

public:
    QString mSessionId;
    QString mUserName;
    QString mPassword;
    QString mHost;
    KDSoapGenerated::Sugarsoap *mSoap = nullptr;
    SugarProtocolBase *mProtocol = nullptr;
    PasswordHandler *mPasswordHandler;
};

SugarSession::SugarSession(PasswordHandler *passwordHandler, QObject *parent)
    : QObject(parent), d(new Private(passwordHandler))
{
}

SugarSession::~SugarSession()
{
    delete d;
}

SugarSession::RequiredAction SugarSession::setSessionParameters(const QString &username, const QString &password, const QString &host)
{
    RequiredAction result = None;

    if (host != d->mHost || username != d->mUserName) {
        result = NewLogin;
    } else if (password != d->mPassword) {
        result = ReLogin;
    }

    d->mUserName = username;
    d->mPassword = password;
    d->mHost = host;

    return result;
}

PasswordHandler *SugarSession::passwordHandler()
{
    return d->mPasswordHandler;
}

void SugarSession::createSoapInterface()
{
    if (d->mSoap) {
        d->mSoap->disconnect();
        d->mSoap->deleteLater();
    }

    d->mSoap = new KDSoapGenerated::Sugarsoap;
    QUrl url(d->mHost);
    url.setPath("/service/v4_1/soap.php");
    url.setQuery(QString());
    d->mSoap->setEndPoint(url.toString());
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

bool SugarSession::readPassword()
{
    if (d->mPassword.isEmpty() && d->mPasswordHandler) {
        bool userRejected = false;
        d->mPassword = d->mPasswordHandler->password(&userRejected);
        return !userRejected;
    }
    return true;
}

void SugarSession::forgetSession()
{
    d->mSessionId = QString();
}

void SugarSession::setSessionId(const QString &sessionId)
{
    d->mSessionId = sessionId;
}

KDSoapGenerated::Sugarsoap *SugarSession::soap()
{
    return d->mSoap;
}

void SugarSession::setProtocol(SugarProtocolBase *protocol)
{
    d->mProtocol = protocol;
}

SugarProtocolBase *SugarSession::protocol() const
{
    return d->mProtocol;
}

