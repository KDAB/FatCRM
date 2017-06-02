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

#include "listmodulesjob.h"

#include "sugarsoap.h"
#include "sugarsession.h"
#include "sugarprotocolbase.h"
using namespace KDSoapGenerated;
#include <KDSoapClient/KDSoapMessage.h>

#include <KDebug>

#include <QStringList>

class ListModulesJob::Private
{
    ListModulesJob *const q;

public:
    explicit Private(ListModulesJob *parent) : q(parent)
    {
    }

public:
    QStringList mModules;

public:
    void listModulesDone(const TNS__Select_fields &moduleNames);
    void listModulesError(int error, const QString &errorMessage);
};

void ListModulesJob::Private::listModulesDone(const KDSoapGenerated::TNS__Select_fields &moduleNames)
{
    mModules = moduleNames.items();

    kDebug() << "Got" << mModules.count() << "available modules";
    Q_ASSERT(!mModules.isEmpty()); // abort before deleting everything locally...

    q->emitResult();
}

void ListModulesJob::Private::listModulesError(int error, const QString &errorMessage)
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

ListModulesJob::ListModulesJob(SugarSession *session, QObject *parent)
    : SugarJob(session, parent), d(new Private(this))
{
    connect(soap(), SIGNAL(get_available_modulesDone(KDSoapGenerated::TNS__Module_list)),
            this,  SLOT(listModulesDone(KDSoapGenerated::TNS__Module_list)));
    connect(soap(), SIGNAL(get_available_modulesError(KDSoapMessage)),
            this,  SLOT(listModulesError(KDSoapMessage)));
}

ListModulesJob::~ListModulesJob()
{
    delete d;
}

QStringList ListModulesJob::modules() const
{
    return d->mModules;
}

void ListModulesJob::startSugarTask()
{
    KDSoapGenerated::TNS__Select_fields selectFields;
    QString errorMessage;
    int result = session()->protocol()->listModules(selectFields, errorMessage);
    if (result == KJob::NoError) {
        d->listModulesDone(selectFields);
    } else {
        d->listModulesError(result, errorMessage);
    }
}

#include "listmodulesjob.moc"
