#include "opportunitiespage.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "opportunityfilterwidget.h"
#include "opportunityfilterproxymodel.h"

#include "kdcrmdata/sugaropportunity.h"

#include <akonadi/entitymimetypefiltermodel.h>

#include <QDebug>

using namespace Akonadi;

OpportunitiesPage::OpportunitiesPage(QWidget *parent)
    : Page(parent, SugarOpportunity::mimeType(), Opportunity)
{
    OpportunityFilterProxyModel *oppFilterProxyModel = new OpportunityFilterProxyModel(this);
    setFilter(oppFilterProxyModel);

    OpportunityFilterWidget* filterUiWidget = new OpportunityFilterWidget(oppFilterProxyModel, this);
    insertFilterWidget(filterUiWidget);
}

OpportunitiesPage::~OpportunitiesPage()
{
}

void OpportunitiesPage::setupModel()
{
    Page::setupModel();

    const ItemsTreeModel::ColumnTypes columns = ItemsTreeModel::columnTypes(Opportunity);
    const int nextStepDateColumn = columns.indexOf(ItemsTreeModel::NextStepDate);
    treeView()->sortByColumn(nextStepDateColumn, Qt::DescendingOrder);
}

QString OpportunitiesPage::reportTitle() const
{
    return tr("List of Opportunities"); // TODO extend title with proxy filter settings
}
