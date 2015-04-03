#include "opportunitiespage.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "sugarclient.h"
#include "opportunityfilterwidget.h"
#include "opportunityfilterproxymodel.h"

#include "kdcrmdata/sugaropportunity.h"

#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

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
    treeView()->sortByColumn(4 /*NextStepDate*/, Qt::DescendingOrder);
}

void OpportunitiesPage::addItem(const QMap<QString, QString> &data)
{
    Item item;
    details()->updateItem(item, data);

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob(item, collection());
    Q_UNUSED(job);
    clientWindow()->setEnabled(false);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    emit statusMessage(tr("Be patient the data is being saved remotely!..."));
}

void OpportunitiesPage::modifyItem(Item &item, const QMap<QString, QString> &data)
{
    details()->updateItem(item, data);

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemModifyJob *job = new ItemModifyJob(item);
    if (!job->exec()) {
        return;    //qDebug() << "Error:" << job->errorString();
    }

    clientWindow()->setEnabled(false);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    emit statusMessage(tr("Saving opportunity..."));
}
