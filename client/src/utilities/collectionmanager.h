#ifndef COLLECTIONMANAGER_H
#define COLLECTIONMANAGER_H

#include <QObject>

namespace Akonadi
{
class Collection;
}
class KJob;

class CollectionManager : public QObject
{
    Q_OBJECT
public:
    explicit CollectionManager(QObject *parent = 0);

    // This is what triggers the collection finding
    void setResource(const QByteArray &identifier);

signals:
    void collectionResult(const QString &mimeType, const Akonadi::Collection &collection);

private slots:
    void slotCollectionFetchResult(KJob *job);
};

#endif // COLLECTIONMANAGER_H
