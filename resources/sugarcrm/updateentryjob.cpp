/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <AkonadiCore/Item>

#include <QDebug>
#include <KLocalizedString>

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
        : q(parent), mItem(item), mHandler(0), mStage(Init)
    {
    }

public:
    Item mItem;
    ModuleHandler *mHandler;

    Item mConflictItem;

    Stage mStage;

public: // slots
    void getEntryDone(const KDSoapGenerated::TNS__Get_entry_result &callResult);
    void getEntryError(const KDSoapMessage &fault);
    void setEntryDone(const KDSoapGenerated::TNS__Set_entry_result &callResult);
    void setEntryError(const KDSoapMessage &fault);
    void getRevisionDone(const KDSoapGenerated::TNS__Get_entry_result &callResult);
    void getRevisionError(const KDSoapMessage &fault);
};

void UpdateEntryJob::Private::getEntryDone(const KDSoapGenerated::TNS__Get_entry_result &callResult)
{
    // check if this is our signal
    if (mStage != GetEntry) {
        return;
    }

    if (q->handleError(callResult.error())) {
        return;
    }

    const QList<KDSoapGenerated::TNS__Entry_value> entries = callResult.entry_list().items();
    if (entries.count() != 1) {
        qWarning() << "Got" << entries.count() << "entries";
        Q_ASSERT(entries.count() == 1);
    }
    const Akonadi::Item remoteItem = mHandler->itemFromEntry(entries.first(), mItem.parentCollection());

    qDebug() << "remote=" << remoteItem.remoteRevision()
             << "local="  << mItem.remoteRevision();
    bool hasConflict = false;
    if (mItem.remoteRevision().isEmpty()) {
        qWarning() << "local item (id=" << mItem.id()
                   << ", remoteId=" << mItem.remoteId()
                   << ") in collection=" << mHandler->moduleName()
                   << "does not have remoteRevision";
        hasConflict = !remoteItem.remoteRevision().isEmpty();
    } else if (remoteItem.remoteRevision().isEmpty()) {
        qWarning() << "remote item (id=" << remoteItem.id()
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

        mHandler->setEntry(mItem);
    }
}

void UpdateEntryJob::Private::getEntryError(const KDSoapMessage &fault)
{
    // check if this is our signal
    if (mStage != GetEntry) {
        return;
    }

    if (!q->handleLoginError(fault)) {
        qWarning() << "Update Entry Error:" << fault.faultAsString();

        q->setError(SugarJob::SoapError);
        q->setErrorText(fault.faultAsString());
        q->emitResult();
    }
}

void UpdateEntryJob::Private::setEntryDone(const KDSoapGenerated::TNS__Set_entry_result &callResult)
{
    if (q->handleError(callResult.error())) {
        return;
    }

    qDebug() << "Updated entry" << callResult.id() << "in module" << mHandler->moduleName();
    mItem.setRemoteId(callResult.id());

    mStage = Private::GetRevision;

    KDSoapGenerated::TNS__Select_fields selectedFields;
    selectedFields.setItems(QStringList() << QStringLiteral("date_modified"));

    q->soap()->asyncGet_entry(q->sessionId(), mHandler->moduleName(), mItem.remoteId(), selectedFields);
}

void UpdateEntryJob::Private::setEntryError(const KDSoapMessage &fault)
{
    if (!q->handleLoginError(fault)) {
        qWarning() << "Update Entry Error:" << fault.faultAsString();

        q->setError(SugarJob::SoapError);
        q->setErrorText(fault.faultAsString());
        q->emitResult();
    }
}

void UpdateEntryJob::Private::getRevisionDone(const KDSoapGenerated::TNS__Get_entry_result &callResult)
{
    // check if this is our signal
    if (mStage != GetRevision) {
        return;
    }

    const QList<KDSoapGenerated::TNS__Entry_value> entries = callResult.entry_list().items();
    Q_ASSERT(entries.count() == 1);
    const Akonadi::Item remoteItem = mHandler->itemFromEntry(entries.first(), mItem.parentCollection());

    mItem.setRemoteRevision(remoteItem.remoteRevision());
    qDebug() << "Got remote revision" << mItem.remoteRevision();

    q->emitResult();
}

void UpdateEntryJob::Private::getRevisionError(const KDSoapMessage &fault)
{
    // check if this is our signal
    if (mStage != GetRevision) {
        return;
    }

    qWarning() << "Error when getting remote revision:" << fault.faultAsString();

    // the item has been added we just don't have a server side datetime
    q->emitResult();
}

UpdateEntryJob::UpdateEntryJob(const Akonadi::Item &item, SugarSession *session, QObject *parent)
    : SugarJob(session, parent), d(new Private(this, item))
{
    connect(soap(), SIGNAL(get_entryDone(KDSoapGenerated::TNS__Get_entry_result)),
            this,  SLOT(getEntryDone(KDSoapGenerated::TNS__Get_entry_result)));
    connect(soap(), SIGNAL(get_entryError(KDSoapMessage)),
            this,  SLOT(getEntryError(KDSoapMessage)));

    connect(soap(), SIGNAL(set_entryDone(KDSoapGenerated::TNS__Set_entry_result)),
            this,  SLOT(setEntryDone(KDSoapGenerated::TNS__Set_entry_result)));
    connect(soap(), SIGNAL(set_entryError(KDSoapMessage)),
            this,  SLOT(setEntryError(KDSoapMessage)));

    connect(soap(), SIGNAL(get_entryDone(KDSoapGenerated::TNS__Get_entry_result)),
            this,  SLOT(getRevisionDone(KDSoapGenerated::TNS__Get_entry_result)));
    connect(soap(), SIGNAL(get_entryError(KDSoapMessage)),
            this,  SLOT(getRevisionError(KDSoapMessage)));
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
    Q_ASSERT(d->mHandler != 0);

    d->mStage = Private::GetEntry;

    if (!d->mHandler->getEntry(d->mItem)) {
        setError(SugarJob::InvalidContextError);
        setErrorText(i18nc("@info:status", "Attempting to modify a malformed item in folder %1",
                           d->mHandler->moduleName()));
        emitResult();
    }
}
#include "moc_updateentryjob.cpp"
