/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Tobias Koenig <tobias.koenig@kdab.com>

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

#include "dbusinvokerinterface.h"

#include <KDebug>

#include <QDBusConnection>

DBusInvokerInterface::DBusInvokerInterface(QObject *parent)
    : QObject(parent)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    const bool registered = bus.registerObject(QLatin1String("/InvokerInterface"), this,
                                               QDBusConnection::ExportScriptableSlots);
    if (!registered) {
        kWarning() << "Failed to register provider object /InvokerInterface";
    }
}

void DBusInvokerInterface::importCsvFile(const QString &filePath)
{
    emit importCsvFileRequested(filePath);
}
