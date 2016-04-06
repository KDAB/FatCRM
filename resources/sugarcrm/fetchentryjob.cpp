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

#include "fetchentryjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"
using namespace KDSoapGenerated;

#include <KDSoapClient/KDSoapMessage.h>

#include <AkonadiCore/Item>

#include "sugarcrmresource_debug.h"
#include <KLocalizedString>

#include <QStringList>

using namespace Akonadi;

class FetchEntryJob::Private
{
    FetchEntryJob *const q;

public:
    explicit Private(FetchEntryJob *parent, const Item &item)
        : q(parent), mItem(item), mHandler(0)
    {
    }

public:
    Item mItem;
    ModuleHandler *mHandler;

public: // slots
    void getEntryDone(const KDSoapGenerated::TNS__Get_entry_result &callResult);
    void getEntryError(const KDSoapMessage &fault);
};

void FetchEntryJob::Private::getEntryDone(const KDSoapGenerated::TNS__Get_entry_result &callResult)
{
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
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Fetched" << mHandler->moduleName()
             << "Entry" << mItem.remoteId()
             << "with revision" << mItem.remoteRevision();

    q->emitResult();
}

void FetchEntryJob::Private::getEntryError(const KDSoapMessage &fault)
{
    if (!q->handleLoginError(fault)) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "Fetch Entry Error:" << fault.faultAsString();

        q->setError(SugarJob::SoapError);
        q->setErrorText(fault.faultAsString());
        q->emitResult();
    }
}

FetchEntryJob::FetchEntryJob(const Akonadi::Item &item, SugarSession *session, QObject *parent)
    : SugarJob(session, parent), d(new Private(this, item))
{
    connect(soap(), SIGNAL(get_entryDone(KDSoapGenerated::TNS__Get_entry_result)),
            this,  SLOT(getEntryDone(KDSoapGenerated::TNS__Get_entry_result)));
    connect(soap(), SIGNAL(get_entryError(KDSoapMessage)),
            this,  SLOT(getEntryError(KDSoapMessage)));
}

FetchEntryJob::~FetchEntryJob()
{
    delete d;
}

void FetchEntryJob::setModule(ModuleHandler *handler)
{
    d->mHandler = handler;
}

Item FetchEntryJob::item() const
{
    return d->mItem;
}

void FetchEntryJob::startSugarTask()
{
    Q_ASSERT(d->mItem.isValid());
    Q_ASSERT(d->mHandler != 0);

    if (!d->mHandler->getEntry(d->mItem)) {
        setError(SugarJob::InvalidContextError);
        setErrorText(i18nc("@info:status", "Attempting to fetch a malformed item from folder %1",
                           d->mHandler->moduleName()));
        emitResult();
    }
}
#include "moc_fetchentryjob.cpp"
