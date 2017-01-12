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

#ifndef UPDATEENTRYJOB_H
#define UPDATEENTRYJOB_H

#include "sugarjob.h"

namespace Akonadi
{
class Item;
}

class ModuleHandler;
namespace KDSoapGenerated
{
class TNS__Get_entry_result;
class TNS__Set_entry_result;
}

class UpdateEntryJob : public SugarJob
{
    Q_OBJECT

public:
    enum UpdateErrors {
        ConflictError = SugarJob::TaskError + 1
    };

    UpdateEntryJob(const Akonadi::Item &item, SugarSession *session, QObject *parent = 0);

    ~UpdateEntryJob() override;

    void setModule(ModuleHandler *handler);
    ModuleHandler *module() const;

    Akonadi::Item item() const;

    Akonadi::Item conflictItem() const;

protected:
    void startSugarTask();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void getEntryDone(const KDSoapGenerated::TNS__Get_entry_result &callResult))
    Q_PRIVATE_SLOT(d, void getEntryError(const KDSoapMessage &fault))
    Q_PRIVATE_SLOT(d, void setEntryDone(const KDSoapGenerated::TNS__Set_entry_result &callResult))
    Q_PRIVATE_SLOT(d, void setEntryError(const KDSoapMessage &fault))
    Q_PRIVATE_SLOT(d, void getRevisionDone(const KDSoapGenerated::TNS__Get_entry_result &callResult))
    Q_PRIVATE_SLOT(d, void getRevisionError(const KDSoapMessage &fault))
};

#endif
