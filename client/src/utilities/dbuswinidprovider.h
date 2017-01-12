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

#ifndef DBUSWINIDPROVIDER_H
#define DBUSWINIDPROVIDER_H

#include <QObject>

class QWidget;

class DBusWinIdProvider : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.akonaditray")

public:
    explicit DBusWinIdProvider(QWidget *referenceWindow);

    ~DBusWinIdProvider() override;

public Q_SLOTS:
    Q_SCRIPTABLE qlonglong getWinId();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void tryRegister())
};

#endif
