/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef SUGARSESSION_H
#define SUGARSESSION_H

#include <QObject>
class SugarProtocolBase;

namespace KDSoapGenerated
{
class Sugarsoap;
}
class PasswordHandler;

class SugarSession : public QObject
{
    Q_OBJECT

public:
    enum RequiredAction {
        None,
        ReLogin,
        NewLogin
    };

    explicit SugarSession(PasswordHandler *passwordHandler, QObject *parent = 0);

    ~SugarSession() override;

    RequiredAction setSessionParameters(const QString &username, const QString &password,
                                        const QString &host);

    QString userName() const;
    QString password() const;
    QString host() const;

    void setSessionId(const QString &sessionId);
    QString sessionId() const;
    void forgetSession();

    PasswordHandler *passwordHandler();
    void createSoapInterface();
    // read password from wallet (and store it in session), return true on success
    bool readPassword();
    void logout();

    void setProtocol(SugarProtocolBase *protocol);
    SugarProtocolBase *protocol() const;

    KDSoapGenerated::Sugarsoap *soap();

private:
    class Private;
    Private *const d;
};

#endif
