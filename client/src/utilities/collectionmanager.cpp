#include "collectionmanager.h"
#include <akonadi/collectionfetchjob.h>
#include <akonadi/collectionfetchscope.h>

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

static bool collectionLessThan(const Collection &c1, const Collection &c2)
{
    // In my tests contentMimeTypes always had exactly one entry.
    const QString contentMimeType1 = c1.contentMimeTypes().at(0);
    const QString contentMimeType2 = c2.contentMimeTypes().at(0);

    // Now emit them in the right order for fast startup.
    // We want to see Opportunities first, and load Notes last.

    static const char s_opp[] = "application/x-vnd.kdab.crm.opportunity";
    if (contentMimeType1 == s_opp)
        return true;
    if (contentMimeType2 == s_opp)
        return false;

    static const char s_note[] = "application/x-vnd.kdab.crm.note";
    if (contentMimeType1 == s_note)
        return false;
    if (contentMimeType2 == s_note)
        return true;

    return contentMimeType1 < contentMimeType2;
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
