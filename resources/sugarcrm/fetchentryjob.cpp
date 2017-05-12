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

#include "fetchentryjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"
using namespace KDSoapGenerated;

#include <KDSoapClient/KDSoapMessage.h>

#include <Akonadi/Item>

#include <KDebug>
#include <KLocale>

#include <QStringList>

using namespace Akonadi;

class FetchEntryJob::Private
{
    FetchEntryJob *const q;

public:
    explicit Private(FetchEntryJob *parent, const Item &item)
        : q(parent), mItem(item), mHandler(nullptr)
    {
    }

public:
    Item mItem;
    ModuleHandler *mHandler;

    void getEntryDone(const TNS__Entry_value &entryValue);
    void getEntryError(int error, QString &errorMessage);
};

void FetchEntryJob::Private::getEntryDone(const KDSoapGenerated::TNS__Entry_value &entryValue)
{
    const Akonadi::Item remoteItem = mHandler->itemFromEntry(entryValue, mItem.parentCollection());

    Item item = remoteItem;
    item.setId(mItem.id());
    item.setRevision(mItem.revision());
    mItem = item;
    kDebug() << "Fetched" << mHandler->module()
             << "Entry" << mItem.remoteId()
             << "with revision" << mItem.remoteRevision();

    q->emitResult();
}

void FetchEntryJob::Private::getEntryError(int error, QString &errorMessage)
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

FetchEntryJob::FetchEntryJob(const Akonadi::Item &item, SugarSession *session, QObject *parent)
    : SugarJob(session, parent), d(new Private(this, item))
{

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
    Q_ASSERT(d->mHandler != nullptr);

    KDSoapGenerated::TNS__Entry_value entryValue;
    QString errorMessage;
    int result = d->mHandler->getEntry(d->mItem, entryValue, errorMessage);
    if (result == KJob::NoError) {
        d->getEntryDone(entryValue);
    } else if (result == SugarJob::InvalidContextError) {
        setError(result);
        setErrorText(i18nc("@info:status", "Attempting to fetch a malformed item from folder %1",
                           moduleToName(d->mHandler->module())));
        emitResult();
    } else {
        d->getEntryError(result, errorMessage);
    }
}

#include "fetchentryjob.moc"
