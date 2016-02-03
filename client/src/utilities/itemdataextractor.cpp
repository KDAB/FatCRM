#include "itemdataextractor.h"

ItemDataExtractor::ItemDataExtractor(QObject *parent) :
    QObject(parent)
{

}

ItemDataExtractor::~ItemDataExtractor()
{

}

QUrl ItemDataExtractor::itemUrl(const QString &resourceBaseUrl, const Akonadi::Item &item) const
{
    const QString itemId = idForItem(item);
    return itemUrl(resourceBaseUrl, itemId);
}

QUrl ItemDataExtractor::itemUrl(const QString &resourceBaseUrl, const QString &itemId) const
{
    if (resourceBaseUrl.isEmpty() || itemId.isEmpty())
        return QUrl();
    return QUrl(resourceBaseUrl + itemAddress() + itemId);
}
