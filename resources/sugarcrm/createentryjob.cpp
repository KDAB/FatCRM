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

#include "createentryjob.h"
#include "sugarcrmresource_debug.h"
#include "modulehandler.h"
#include "sugarsoap.h"

using namespace KDSoapGenerated;
#include <KDSoapClient/KDSoapMessage.h>

#include <AkonadiCore/Item>

#include <KLocalizedString>

#include <QStringList>

using namespace Akonadi;

class CreateEntryJob::Private
{
    CreateEntryJob *const q;

public:
    enum Stage {
        Init,
        CreateEntry,
        GetEntry
    };

    explicit Private(CreateEntryJob *parent, const Item &item)
        : q(parent), mItem(item), mHandler(nullptr)
    {
    }

public:
    Item mItem;
    ModuleHandler *mHandler;

public: // slots
    void setEntryHandle(int result, const QString &id, const QString &errorMessage);
    void setEntryDone(const QString &id);
    void setEntryError(int error, const QString &errorMessage);
    void getEntryDone(const TNS__Entry_value entryValue);
    void getEntryError(int error, const QString &errorMessage);
};

void CreateEntryJob::Private::setEntryDone(const QString &id)
{
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Created entry" << id << "in module" << mHandler->moduleName();
    mItem.setRemoteId(id);

    KDSoapGenerated::TNS__Entry_value entryValue;
    QString errorMessage;
    int result = mHandler->getEntry(mItem, entryValue, errorMessage);

    if (result == KJob::NoError) {
        getEntryDone(entryValue);
    } else if (result == SugarJob::InvalidContextError) {
        // the item has been added we just don't have a server side datetime
        q->emitResult();
    } else {
        getEntryError(result, errorMessage);
    }
}

void CreateEntryJob::Private::setEntryError(int error, const QString &errorMessage)
{
    if (error == SugarJob::CouldNotConnectError) {
        // Invalid login error, meaning we need to log in again
        if (q->shouldTryRelogin()) {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Got error 10, probably a session timeout, let's login again";
            QMetaObject::invokeMethod(q, "startLogin", Qt::QueuedConnection);
            return;
        }
    }
    qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << q << error << errorMessage;

    q->setError(SugarJob::SoapError);
    q->setErrorText(errorMessage);
    q->emitResult();
}

void CreateEntryJob::Private::getEntryDone(const KDSoapGenerated::TNS__Entry_value entryValue)
{
    const Akonadi::Item remoteItem = mHandler->itemFromEntry(entryValue, mItem.parentCollection());

    Item item = remoteItem;
    item.setId(mItem.id());
    item.setRevision(mItem.revision());
    mItem = item;
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Got entry with revision" << mItem.remoteRevision();

    q->emitResult();
}

void CreateEntryJob::Private::getEntryError(int error, const QString &errorMessage)
{
    if (error == SugarJob::CouldNotConnectError) {
        // Invalid login error, meaning we need to log in again
        if (q->shouldTryRelogin()) {
            qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Got error 10, probably a session timeout, let's login again";
            QMetaObject::invokeMethod(q, "startLogin", Qt::QueuedConnection);
            return;
        }
    }
    qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << q << error << errorMessage;

    q->setError(SugarJob::SoapError);
    q->setErrorText(errorMessage);
    q->emitResult();
}

CreateEntryJob::CreateEntryJob(const Akonadi::Item &item, SugarSession *session, QObject *parent)
    : SugarJob(session, parent), d(new Private(this, item))
{
}

CreateEntryJob::~CreateEntryJob()
{
    delete d;
}

void CreateEntryJob::setModule(ModuleHandler *handler)
{
    d->mHandler = handler;
}

Item CreateEntryJob::item() const
{
    return d->mItem;
}

void CreateEntryJob::startSugarTask()
{
    Q_ASSERT(d->mItem.isValid());
    Q_ASSERT(d->mHandler != nullptr);

    QString newId, errorMessage;
    int result = d->mHandler->setEntry(d->mItem, newId, errorMessage);
    if (result == KJob::NoError) {
        d->setEntryDone(newId);
    } else if (result == SugarJob::InvalidContextError) {
        setError(result);
        setErrorText(i18nc("@info:status", "Attempting to add malformed item to folder %1",
                           d->mHandler->moduleName()));
        emitResult();
    } else {
        d->setEntryError(result, errorMessage);
    }
}
#include "moc_createentryjob.cpp"
