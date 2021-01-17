/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "invoker.h"

#include <QDebug>
#include <KToolInvocation>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QRegExp>

Invoker::Invoker(const QString &fileName, QObject *parent)
    : QObject(parent)
    , mFileName(fileName)
{
}

void Invoker::run()
{
    QDBusConnectionInterface *iface = QDBusConnection::sessionBus().interface();
    if (!iface) {
        qWarning() << "Unable to connect to dbus";
        return;
    }

    const QStringList services = iface->registeredServiceNames();
    const QStringList fatCrmInstances = services.filter(QRegExp("org.kde.fatcrm-\\d+"));
    if (fatCrmInstances.isEmpty())
        invokeNewInstance();
    else
        invokeRunningInstance(fatCrmInstances.at(0));
}

void Invoker::invokeRunningInstance(const QString &serviceName)
{
    QDBusInterface iface(serviceName, QStringLiteral("/InvokerInterface"));
    if (!iface.isValid()) {
        qWarning() << "Unable to access invoker interface of" << serviceName << ":" << iface.lastError().message();
        return;
    }

    iface.call(QStringLiteral("importCsvFile"), mFileName);
}

void Invoker::invokeNewInstance()
{
    QString errorMessage;
    QString serviceName;

    const int result = KToolInvocation::startServiceByDesktopName(QStringLiteral("org.kde.fatcrm"), QStringList(), &errorMessage, &serviceName);
    if (result != 0) {
        qWarning() << "Unable to start fatcrm:" << errorMessage;
        return;
    }

    invokeRunningInstance(serviceName);
}
