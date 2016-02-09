#include "itemdataextractor.h"

#include "accountdataextractor.h"
#include "campaigndataextractor.h"
#include "contactdataextractor.h"
#include "leaddataextractor.h"
#include "opportunitydataextractor.h"

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

ItemDataExtractor *ItemDataExtractor::createDataExtractor(DetailsType type, QObject *parent)
{
    switch(type) {
    case Account:
        return new AccountDataExtractor(parent);
    case Opportunity:
        return new OpportunityDataExtractor(parent);
    case Lead:
        return new LeadDataExtractor(parent);
    case Contact:
        return new ContactDataExtractor(parent);
    case Campaign:
        return new CampaignDataExtractor(parent);
    default:
        return nullptr;
    }
}
