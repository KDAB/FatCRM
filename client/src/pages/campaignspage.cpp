#include "campaignspage.h"

#include "details.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "sugarclient.h"

#include "kdcrmdata/sugarcampaign.h"

#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

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
