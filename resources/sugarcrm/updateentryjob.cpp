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

#include "updateentryjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"

using namespace KDSoapGenerated;
#include <KDSoapClient/KDSoapMessage.h>

#include <Akonadi/Item>

#include <KDebug>
#include <KLocale>

#include <QStringList>

using namespace Akonadi;

class UpdateEntryJob::Private
{
    UpdateEntryJob *const q;

public:
    enum Stage {
        Init,
        GetEntry,
        UpdateEntry,
        GetRevision
    };

    explicit Private(UpdateEntryJob *parent, const Item &item)
        : q(parent), mItem(item), mHandler(nullptr), mStage(Init)
    {
    }

public:
    Item mItem;
    ModuleHandler *mHandler;

    Item mConflictItem;

    Stage mStage;

    void setEntryError(int error, const QString &errorMessage);
    void setEntryDone(const QString &id);

public: // slots
    void getEntryDone(const TNS__Entry_value entryValue);
    void getEntryError(int error, const QString &errorMessage);
    void getRevisionDone(const TNS__Entry_value &entryValue);
    void getRevisionError(const KDSoapMessage &fault);
};

void UpdateEntryJob::Private::getEntryDone(const KDSoapGenerated::TNS__Entry_value entryValue)
{
    // check if this is our signal
    if (mStage != GetEntry) {
        return;
    }

    const Akonadi::Item remoteItem = mHandler->itemFromEntry(entryValue, mItem.parentCollection());

    kDebug() << "remote=" << remoteItem.remoteRevision()
             << "local="  << mItem.remoteRevision();
    bool hasConflict = false;
    if (mItem.remoteRevision().isEmpty()) {
        kWarning() << "local item (id=" << mItem.id()
                   << ", remoteId=" << mItem.remoteId()
                   << ") in collection=" << mHandler->moduleName()
                   << "does not have remoteRevision";
        hasConflict = !remoteItem.remoteRevision().isEmpty();
    } else if (remoteItem.remoteRevision().isEmpty()) {
        kWarning() << "remote item (id=" << remoteItem.id()
                   << ", remoteId=" << remoteItem.remoteId()
                   << ") in collection=" << mHandler->moduleName()
                   << "does not have remoteRevision";
    } else {
        // remoteRevision is an ISO date, so string comparisons are accurate for < or >
        hasConflict = (remoteItem.remoteRevision() > mItem.remoteRevision());
    }

    if (hasConflict) {
        mConflictItem = remoteItem;
        q->setError(UpdateEntryJob::ConflictError);
        q->setErrorText(i18nc("info:status parameter is module name",
                              "%1 entry on server is newer than the one the local update was based on",
                              mHandler->moduleName()));
        q->emitResult();
    } else {
        mStage = UpdateEntry;

        QString id, errorMessage;
        id = entryValue.id();
        int result = mHandler->setEntry(mItem, id, errorMessage);
        if(result == KJob::NoError) {
            setEntryDone(remoteItem.remoteId());
        } else {
            setEntryError(result, errorMessage);
        }
    }
}

void UpdateEntryJob::Private::getEntryError(int error, const QString &errorMessage)
{
    // check if this is our signal
    if (mStage != GetEntry) {
        return;
    }

    if (error == SugarJob::CouldNotConnectError) {
        // Invalid login error, meaning we need to log in again
        if (q->shouldTryRelogin()) {
            kDebug() << "Got error 10, probably a session timeout, let's login again";
            QMetaObject::invokeMethod(q, "startLogin", Qt::QueuedConnection);
            return;
        }
    }
    kWarning() << q << error << errorMessage;

    q->setError(SugarJob::SoapError);
    q->setErrorText(errorMessage);
    q->emitResult();
}

void UpdateEntryJob::Private::setEntryDone(const QString &id)
{
    kDebug() << "Updated entry" << id << "in module" << mHandler->moduleName();
    mItem.setRemoteId(id);

    mStage = Private::GetRevision;

    KDSoapGenerated::TNS__Select_fields selectedFields;
    selectedFields.setItems(QStringList() << QLatin1String("date_modified"));

    KDSoapGenerated::TNS__Entry_value entryValue;
    QString errorMessage;
    int result = mHandler->getEntry(mItem, entryValue, errorMessage);
    if (result == KJob::NoError) {
        getRevisionDone(entryValue);
    } else if (result == -1) {
        q->setError(SugarJob::InvalidContextError);
        q->setErrorText(i18nc("@info:status", "Attempting to modify a malformed item in folder %1",
                           mHandler->moduleName()));
        q->emitResult();
    } else {
        getEntryError(result, errorMessage);
    }
}

void UpdateEntryJob::Private::setEntryError(int error, const QString &errorMessage)
{
    if (error == SugarJob::CouldNotConnectError) {
        // Invalid login error, meaning we need to log in again
        if (q->shouldTryRelogin()) {
            kDebug() << "Got error 10, probably a session timeout, let's login again";
            QMetaObject::invokeMethod(q, "startLogin", Qt::QueuedConnection);
            return;
        }
    }
    kWarning() << q << error << errorMessage;

    q->setError(SugarJob::SoapError);
    q->setErrorText(errorMessage);
    q->emitResult();
}

void UpdateEntryJob::Private::getRevisionDone(const KDSoapGenerated::TNS__Entry_value &entryValue)
{
    // check if this is our signal
    if (mStage != GetRevision) {
        return;
    }

    const Akonadi::Item remoteItem = mHandler->itemFromEntry(entryValue, mItem.parentCollection());

    mItem.setRemoteRevision(remoteItem.remoteRevision());
    kDebug() << "Got remote revision" << mItem.remoteRevision();

    q->emitResult();
}

void UpdateEntryJob::Private::getRevisionError(const KDSoapMessage &fault)
{
    // check if this is our signal
    if (mStage != GetRevision) {
        return;
    }

    kWarning() << "Error when getting remote revision:" << fault.faultAsString();

    // the item has been added we just don't have a server side datetime
    q->emitResult();
}

UpdateEntryJob::UpdateEntryJob(const Akonadi::Item &item, SugarSession *session, QObject *parent)
    : SugarJob(session, parent), d(new Private(this, item))
{

}

UpdateEntryJob::~UpdateEntryJob()
{
    delete d;
}

void UpdateEntryJob::setModule(ModuleHandler *handler)
{
    d->mHandler = handler;
}

ModuleHandler *UpdateEntryJob::module() const
{
    return d->mHandler;
}

Item UpdateEntryJob::item() const
{
    return d->mItem;
}

Item UpdateEntryJob::conflictItem() const
{
    return d->mConflictItem;
}

void UpdateEntryJob::startSugarTask()
{
    Q_ASSERT(d->mItem.isValid());
    Q_ASSERT(d->mHandler != nullptr);

    d->mStage = Private::GetEntry;

    KDSoapGenerated::TNS__Entry_value entryValue;
    QString errorMessage;
    int result = d->mHandler->getEntry(d->mItem, entryValue, errorMessage);
    if (result == KJob::NoError) {
        d->getEntryDone(entryValue);
    } else if (result == -1) {
        setError(SugarJob::InvalidContextError);
        setErrorText(i18nc("@info:status", "Attempting to modify a malformed item in folder %1",
                           d->mHandler->moduleName()));
        emitResult();
    } else {
        d->getEntryError(result, errorMessage);
    }
}

#include "updateentryjob.moc"
