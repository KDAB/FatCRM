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

#include "listmodulesjob.h"

#include "sugarsoap.h"

using namespace KDSoapGenerated;
#include <KDSoapClient/KDSoapMessage.h>

#include <QDebug>

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

public: // slots
    void listModulesDone(const KDSoapGenerated::TNS__Module_list &callResult);
    void listModulesError(const KDSoapMessage &fault);
};

void ListModulesJob::Private::listModulesDone(const KDSoapGenerated::TNS__Module_list &callResult)
{
    if (q->handleError(callResult.error())) {
        return;
    }
    const KDSoapGenerated::TNS__Select_fields moduleNames = callResult.modules();
    mModules = moduleNames.items();

    qDebug() << "Got" << mModules.count() << "available modules";
    Q_ASSERT(!mModules.isEmpty()); // abort before deleting everything locally...

    q->emitResult();
}

void ListModulesJob::Private::listModulesError(const KDSoapMessage &fault)
{
    if (!q->handleLoginError(fault)) {
        qWarning() << "List Modules Error:" << fault.faultAsString();

        q->setError(SugarJob::SoapError);
        q->setErrorText(fault.faultAsString());
        q->emitResult();
    }
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
    soap()->asyncGet_available_modules(sessionId());
}

#include "moc_listmodulesjob.cpp"
