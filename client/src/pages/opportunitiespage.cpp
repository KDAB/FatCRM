#include "opportunitiespage.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "sugarclient.h"
#include "opportunityfilterwidget.h"
#include "opportunityfilterproxymodel.h"
#include "details.h"

#include "kdcrmdata/sugaropportunity.h"

#include <akonadi/entitymimetypefiltermodel.h>

#include <akonadi/itemfetchjob.h>
#include <akonadi/itemfetchscope.h>

#include <QDebug>

using namespace Akonadi;

OpportunitiesPage::OpportunitiesPage(QWidget *parent)
    : Page(parent, QString(SugarOpportunity::mimeType()), Opportunity)
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
    treeView()->sortByColumn(5 /*NextStepDate*/, Qt::DescendingOrder);

    // TODO load notes
    //Akonadi::ItemFetchJob *job = new Akonadi::ItemFetchJob(notesCollection, ...);
}

QString OpportunitiesPage::reportTitle() const
{
    return tr("List of Opportunities"); // TODO extend title with proxy filter settings
}
