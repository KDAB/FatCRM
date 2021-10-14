/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>

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

#ifndef PASSWORDHANDLER_H
#define PASSWORDHANDLER_H

#include <QObject>
#include <QWidget>

class PasswordHandler : public QObject
{
    Q_OBJECT
public:
    explicit PasswordHandler(const QString &resourceId, QObject *parent = nullptr);

    bool isPasswordAvailable();
    QString password( bool *userRejected = nullptr );
    void setPassword( const QString &password );

Q_SIGNALS:
    void passwordAvailable();

private:
    bool savePassword();
    QString mPassword;
    const QString mResourceId;
    bool mKeychainOpened = false;
    bool mDeniedByUser = false;
};

#endif // PASSWORDHANDLER_H
