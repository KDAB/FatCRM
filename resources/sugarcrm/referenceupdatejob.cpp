/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>

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

#include "referenceupdatejob.h"

#include <AkonadiCore/ItemFetchJob>
#include <AkonadiCore/ItemFetchScope>
#include <AkonadiCore/ItemModifyJob>

ReferenceUpdateJob::ReferenceUpdateJob(const Akonadi::Collection &collection, QObject *parent) :
    KCompositeJob(parent),
    mCollection(collection)
{
}

void ReferenceUpdateJob::start()
{
    Akonadi::ItemFetchJob *job = new Akonadi::ItemFetchJob(mCollection, this);
    job->fetchScope().setCacheOnly(true);
    job->fetchScope().fetchFullPayload(true);
    connect(job, SIGNAL(itemsReceived(Akonadi::Item::List)), this, SLOT(slotItemsReceived(Akonadi::Item::List)));
    addSubjob(job);
}

void ReferenceUpdateJob::slotItemsReceived(const Akonadi::Item::List &items)
{
    Akonadi::Item::List modifiedItems;
    foreach (const Akonadi::Item &item, items) {
        // My kingdom for a C++ std::function here instead of a virtual
        Akonadi::Item copy = item;
        if (updateItem(copy)) {
            modifiedItems.append(copy);
        }
    }
    if (!modifiedItems.isEmpty()) {
        // This method can be called multiple times, so we can create multiple modify jobs
        Akonadi::ItemModifyJob *modifyJob = new Akonadi::ItemModifyJob(modifiedItems, this);
        addSubjob(modifyJob);
    }
}

void ReferenceUpdateJob::slotResult(KJob *job)
{
    KCompositeJob::slotResult(job); // does error handling

    if (!job->error() && qobject_cast<Akonadi::ItemModifyJob *>(job)) {
        if (subjobs().isEmpty()) {
            emitResult();
        }
    }
}
