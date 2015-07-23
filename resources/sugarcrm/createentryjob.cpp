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

#include "createentryjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"

using namespace KDSoapGenerated;
#include <KDSoapClient/KDSoapMessage.h>

#include <AkonadiCore/Item>

#include <QDebug>
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
        : q(parent), mItem(item), mHandler(0), mStage(Init)
    {
    }

public:
    Item mItem;
    ModuleHandler *mHandler;
    Stage mStage;

public: // slots
    void setEntryDone(const KDSoapGenerated::TNS__Set_entry_result &callResult);
    void setEntryError(const KDSoapMessage &fault);
    void getEntryDone(const KDSoapGenerated::TNS__Get_entry_result &callResult);
    void getEntryError(const KDSoapMessage &fault);
};

void CreateEntryJob::Private::setEntryDone(const KDSoapGenerated::TNS__Set_entry_result &callResult)
{
    Q_ASSERT(mStage == CreateEntry);
    if (q->handleError(callResult.error())) {
        return;
    }

    qDebug() << "Created entry" << callResult.id() << "in module" << mHandler->moduleName();
    mItem.setRemoteId(callResult.id());

    mStage = Private::GetEntry;

    if (!mHandler->getEntry(mItem)) {
        // the item has been added we just don't have a server side datetime
        q->emitResult();
    }
}

void CreateEntryJob::Private::setEntryError(const KDSoapMessage &fault)
{
    Q_ASSERT(mStage == CreateEntry);

    if (!q->handleLoginError(fault)) {
        qWarning() << "Create Entry Error:" << fault.faultAsString();

        q->setError(SugarJob::SoapError);
        q->setErrorText(fault.faultAsString());
        q->emitResult();
    }
}

void CreateEntryJob::Private::getEntryDone(const KDSoapGenerated::TNS__Get_entry_result &callResult)
{
    Q_ASSERT(mStage == GetEntry);

    if (q->handleError(callResult.error())) {
        return;
    }

    const QList<KDSoapGenerated::TNS__Entry_value> entries = callResult.entry_list().items();
    Q_ASSERT(entries.count() == 1);
    const Akonadi::Item remoteItem = mHandler->itemFromEntry(entries.first(), mItem.parentCollection());

    Item item = remoteItem;
    item.setId(mItem.id());
    item.setRevision(mItem.revision());
    mItem = item;
    qDebug() << "Got entry with revision" << mItem.remoteRevision();

    q->emitResult();
}

void CreateEntryJob::Private::getEntryError(const KDSoapMessage &fault)
{
    Q_ASSERT(mStage == GetEntry);

    qWarning() << "Error when getting remote version:" << fault.faultAsString();

    // the item has been added we just don't have a server side datetime
    q->emitResult();
}

CreateEntryJob::CreateEntryJob(const Akonadi::Item &item, SugarSession *session, QObject *parent)
    : SugarJob(session, parent), d(new Private(this, item))
{
    connect(soap(), SIGNAL(set_entryDone(KDSoapGenerated::TNS__Set_entry_result)),
            this,  SLOT(setEntryDone(KDSoapGenerated::TNS__Set_entry_result)));
    connect(soap(), SIGNAL(set_entryError(KDSoapMessage)),
            this,  SLOT(setEntryError(KDSoapMessage)));
    connect(soap(), SIGNAL(get_entryDone(KDSoapGenerated::TNS__Get_entry_result)),
            this,  SLOT(getEntryDone(KDSoapGenerated::TNS__Get_entry_result)));
    connect(soap(), SIGNAL(get_entryError(KDSoapMessage)),
            this,  SLOT(getEntryError(KDSoapMessage)));
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
    Q_ASSERT(d->mHandler != 0);

    d->mStage = Private::CreateEntry;

    if (!d->mHandler->setEntry(d->mItem)) {
        setError(SugarJob::InvalidContextError);
        setErrorText(i18nc("@info:status", "Attempting to add malformed item to folder %1",
                           d->mHandler->moduleName()));
        emitResult();
    }
}
#include "moc_createentryjob.cpp"
