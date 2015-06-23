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

#include "deleteentryjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"

using namespace KDSoapGenerated;
#include <KDSoapClient/KDSoapMessage.h>

#include <Akonadi/Collection>
#include <Akonadi/Item>

#include <KDebug>

#include <QStringList>

using namespace Akonadi;

class DeleteEntryJob::Private
{
    DeleteEntryJob *const q;

public:
    explicit Private(DeleteEntryJob *parent, const Item &item)
        : q(parent), mItem(item)
    {
    }

public:
    const Item mItem;

public: // slots
    void setEntryDone(const KDSoapGenerated::TNS__Set_entry_result &callResult);
    void setEntryError(const KDSoapMessage &fault);
};

void DeleteEntryJob::Private::setEntryDone(const KDSoapGenerated::TNS__Set_entry_result &callResult)
{
    Q_UNUSED(callResult);
    if (q->handleError(callResult.error())) {
        return;
    }

    kDebug() << "Entry" << mItem.remoteId() << "deleted from module"
             << mItem.parentCollection().remoteId();
    q->emitResult();
}

void DeleteEntryJob::Private::setEntryError(const KDSoapMessage &fault)
{
    if (!q->handleLoginError(fault)) {
        kWarning() << "Delete Entry Error:" << fault.faultAsString();

        q->setError(SugarJob::SoapError);
        q->setErrorText(fault.faultAsString());
        q->emitResult();
    }
}

DeleteEntryJob::DeleteEntryJob(const Akonadi::Item &item, SugarSession *session, QObject *parent)
    : SugarJob(session, parent), d(new Private(this, item))
{
    connect(soap(), SIGNAL(set_entryDone(KDSoapGenerated::TNS__Set_entry_result)),
            this,  SLOT(setEntryDone(KDSoapGenerated::TNS__Set_entry_result)));
    connect(soap(), SIGNAL(set_entryError(KDSoapMessage)),
            this,  SLOT(setEntryError(KDSoapMessage)));
}

DeleteEntryJob::~DeleteEntryJob()
{
    delete d;
}

Item DeleteEntryJob::item() const
{
    return d->mItem;
}

void DeleteEntryJob::startSugarTask()
{
    Q_ASSERT(d->mItem.isValid());
    Q_ASSERT(d->mItem.parentCollection().isValid());

    // delete just required identifier and "deleted" field
    // no need for type specific code
    KDSoapGenerated::TNS__Name_value idField;
    idField.setName(QLatin1String("id"));
    idField.setValue(d->mItem.remoteId());

    KDSoapGenerated::TNS__Name_value deletedField;
    deletedField.setName(QLatin1String("deleted"));
    deletedField.setValue(QLatin1String("1"));

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(QList<KDSoapGenerated::TNS__Name_value>() << idField << deletedField);

    soap()->asyncSet_entry(sessionId(), d->mItem.parentCollection().remoteId(), valueList);
}

#include "deleteentryjob.moc"
