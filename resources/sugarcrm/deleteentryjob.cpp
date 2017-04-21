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

#include "deleteentryjob.h"

#include "modulehandler.h"
#include "sugarsoap.h"
#include "sugarsession.h"

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
    explicit Private(DeleteEntryJob *parent, const Item &item, const QString &moduleName)
        : q(parent), mItem(item), mModuleName(moduleName)
    {
    }

public:
    const Item mItem;
    const QString mModuleName;
    void setEntryDone();
    void setEntryError(int error, const QString &errorMessage);
};

void DeleteEntryJob::Private::setEntryDone()
{
    kDebug() << "Entry" << mItem.remoteId() << "deleted from module"
             << mItem.parentCollection().remoteId();
    q->emitResult();
}

void DeleteEntryJob::Private::setEntryError(int error, const QString &errorMessage)
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

DeleteEntryJob::DeleteEntryJob(const Akonadi::Item &item, SugarSession *session, const QString &moduleName, QObject *parent)
    : SugarJob(session, parent), d(new Private(this, item, moduleName))
{

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
    //Q_ASSERT(d->mItem.parentCollection().isValid());

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

    QString id, errorMessage;
    id = d->mItem.remoteId();
    int result = session()->protocol()->setEntry(d->mModuleName, valueList, id, errorMessage);
    if (result == KJob::NoError) {
        d->setEntryDone();
    } else {
        d->setEntryError(result, errorMessage);
    }
}

#include "deleteentryjob.moc"
