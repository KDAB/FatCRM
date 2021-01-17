/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "wsdl_sugar41.h"
#include "sugarsession.h"
#include "sugarprotocolbase.h"
using namespace KDSoapGenerated;
#include <KDSoapClient/KDSoapMessage.h>

#include "sugarcrmresource_debug.h"

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
    void listModulesDone(const QStringList &moduleNames);
    void listModulesError(int error, const QString &errorMessage);
};

void ListModulesJob::Private::listModulesDone(const QStringList &moduleNames)
{
    mModules = moduleNames;

    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Got" << mModules.count() << "available modules";
    Q_ASSERT(!mModules.isEmpty()); // abort before deleting everything locally...

    q->emitResult();
}

void ListModulesJob::Private::listModulesError(int error, const QString &errorMessage)
{
    qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << q << error << errorMessage;
    if (q->handleConnectError(error, errorMessage)) {
        return;
    }

    q->setError(SugarJob::SoapError);
    q->setErrorText(errorMessage);
    q->emitResult();
}

ListModulesJob::ListModulesJob(SugarSession *session, QObject *parent)
    : SugarJob(session, parent), d(new Private(this))
{

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
    QStringList modules;
    QString errorMessage;
    int result = session()->protocol()->listModules(modules, errorMessage);
    if (result == KJob::NoError) {
        d->listModulesDone(modules);
    } else {
        d->listModulesError(result, errorMessage);
    }
}

#include "moc_listmodulesjob.cpp"
