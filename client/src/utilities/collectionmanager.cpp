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

#include "collectionmanager.h"

#include <AkonadiCore/CollectionFetchJob>
#include <AkonadiCore/CollectionFetchScope>

#include <QStringList>

using namespace Akonadi;

CollectionManager::CollectionManager(QObject *parent) :
    QObject(parent)
{
}

void CollectionManager::setResource(const QByteArray &identifier)
{
    /*
     * Look for the wanted collection explicitly by listing all collections
     * of the currently selected resource, filtering by MIME type.
     * include statistics to get the number of items in each collection
     */
    CollectionFetchJob *job = new CollectionFetchJob(Collection::root(), CollectionFetchJob::Recursive);
    job->fetchScope().setResource(identifier);
    job->fetchScope().setIncludeStatistics(true);
    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotCollectionFetchResult(KJob*)));
}

static int orderForCollection(const QString &contentMimeType)
{
    static const char* s_orderedMimeTypes[] = {
        "application/x-vnd.kdab.crm.account",
        "application/x-vnd.kdab.crm.opportunity",
        "application/x-vnd.kdab.crm.contacts",
        "application/x-vnd.kdab.crm.note",
        "application/x-vnd.kdab.crm.email",
        "application/x-vnd.akonadi.calendar.todo",
        "inode/directory",
        "text/directory"
    };
    for (uint i = 0 ; i < sizeof(s_orderedMimeTypes) / sizeof(*s_orderedMimeTypes); ++i) {
        if (contentMimeType == s_orderedMimeTypes[i])
            return i;
    }
    qDebug() << "unexpected content mimetype for ordering:" << contentMimeType;
    return 20;
}

static bool collectionLessThan(const Collection &c1, const Collection &c2)
{
    // In my tests contentMimeTypes always had exactly one entry.
    const QString contentMimeType1 = c1.contentMimeTypes().at(0);
    const QString contentMimeType2 = c2.contentMimeTypes().at(0);

    // Now emit them in the right order for fast startup.
    // We want to see Accounts first (because required by Opportunities),
    // then Opportunities (because visible on screen), and load Notes/Emails last.

    const int order1 = orderForCollection(contentMimeType1);
    const int order2 = orderForCollection(contentMimeType2);

    return order1 < order2;
}

void CollectionManager::slotCollectionFetchResult(KJob *job)
{
    CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob *>(job);

    QVector<Collection> collections;
    Q_FOREACH (const Collection &collection, fetchJob->collections()) {
        collections.append(collection);
    }

    qSort(collections.begin(), collections.end(), collectionLessThan);

    Q_FOREACH (const Collection &collection, collections) {
        //qDebug() << collection.contentMimeTypes() << "name" << collection.name();
        emit collectionResult(collection.contentMimeTypes().at(0), collection);
    }
}
