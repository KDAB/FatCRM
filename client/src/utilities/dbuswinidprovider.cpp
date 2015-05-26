/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "dbuswinidprovider.h"

#include <KDebug>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QWidget>

class DBusWinIdProvider::Private
{
    DBusWinIdProvider *const q;
public:
    Private(DBusWinIdProvider *parent, QWidget *window)
        : q(parent), mWindow(window), mObjectRegistered(false)
    {
    }

public:
    QWidget *mWindow;
    bool mObjectRegistered;

public: // slots
    void tryRegister();
};

void DBusWinIdProvider::Private::tryRegister()
{
    // we don't care if we can't register or if we are replaced by the real tray
    // this is for fallback in case the tray app is not running
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusConnectionInterface *busInterface = bus.interface();
    busInterface->registerService(QLatin1String("org.freedesktop.akonaditray"),
                                  QDBusConnectionInterface::DontQueueService,
                                  QDBusConnectionInterface::AllowReplacement);

    if (!mObjectRegistered) {
        mObjectRegistered = bus.registerObject(QLatin1String("/Actions"), q,
                                               QDBusConnection::ExportScriptableSlots);
        if (!mObjectRegistered) {
            kWarning() << "Failed to register provider object /Actions";
        }
    }

    kDebug() << "currentOwner="
             << busInterface->serviceOwner(QLatin1String("org.freedesktop.akonaditray"));
}

DBusWinIdProvider::DBusWinIdProvider(QWidget *referenceWindow)
    : QObject(referenceWindow), d(new Private(this, referenceWindow))
{
    Q_ASSERT(referenceWindow != 0);

    QDBusServiceWatcher *watcher =
        new QDBusServiceWatcher(QLatin1String("org.freedesktop.akonaditray"),
                                QDBusConnection::sessionBus(),
                                QDBusServiceWatcher::WatchForUnregistration,
                                this);
    connect(watcher, SIGNAL(serviceUnregistered(QString)),
            this, SLOT(tryRegister()));

    QMetaObject::invokeMethod(this, "tryRegister", Qt::QueuedConnection);
}

DBusWinIdProvider::~DBusWinIdProvider()
{
    delete d;
}

qlonglong DBusWinIdProvider::getWinId()
{
    return (qlonglong) d->mWindow->winId();
}

#include "dbuswinidprovider.moc"
