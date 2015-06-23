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

#ifndef LISTENTRIESJOB_H
#define LISTENTRIESJOB_H

#include "sugarjob.h"

#include <Akonadi/Item>

namespace Akonadi
{
class Collection;
}

class ModuleHandler;
namespace KDSoapGenerated
{
class TNS__Get_entries_count_result;
class TNS__Get_entry_list_result;
}

class ListEntriesJob : public SugarJob
{
    Q_OBJECT

public:
    ListEntriesJob(const Akonadi::Collection &collection, SugarSession *session, QObject *parent = 0);

    ~ListEntriesJob();

    Akonadi::Collection collection() const;
    void setModule(ModuleHandler *handler);

    static int currentContentsVersion(const Akonadi::Collection &collection);
    QString latestTimestamp() const;

Q_SIGNALS:
    void totalItems(int count);
    void itemsReceived(const Akonadi::Item::List &items);
    void deletedReceived(const Akonadi::Item::List &items);

protected:
    void startSugarTask();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void getEntriesCountDone(const KDSoapGenerated::TNS__Get_entries_count_result &callResult))
    Q_PRIVATE_SLOT(d, void getEntriesCountError(const KDSoapMessage &fault))
    Q_PRIVATE_SLOT(d, void listEntriesDone(const KDSoapGenerated::TNS__Get_entry_list_result &callResult))
    Q_PRIVATE_SLOT(d, void listEntriesError(const KDSoapMessage &fault))
};

#endif
