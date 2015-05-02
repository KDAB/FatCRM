#include "campaignspage.h"

#include "itemstreemodel.h"
#include "filterproxymodel.h"

#include "kdcrmdata/sugarcampaign.h"

using namespace Akonadi;

CampaignsPage::CampaignsPage(QWidget *parent)
    : Page(parent, QString(SugarCampaign::mimeType()), Campaign)
{
    setFilter(new FilterProxyModel(Campaign, this));
}

CampaignsPage::~CampaignsPage()
{
}

QString CampaignsPage::reportTitle() const
{
    return tr("List of Campaigns");
}
