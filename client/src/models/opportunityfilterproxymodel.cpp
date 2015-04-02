#include "opportunityfilterproxymodel.h"

#include <kdcrmdata/sugaropportunity.h>

#include <akonadi/entitytreemodel.h>

#include <QDate>

using namespace Akonadi;

class OpportunityFilterProxyModel::Private
{
public:
    Private()
    {}

    QStringList assignees; // no filtering if empty
    QDate maxDate;
};

OpportunityFilterProxyModel::OpportunityFilterProxyModel(QObject *parent)
    : FilterProxyModel(Opportunity, parent), d(new Private())
{
}

OpportunityFilterProxyModel::~OpportunityFilterProxyModel()
{
    delete d;
}

void OpportunityFilterProxyModel::setFilter(const QStringList &assignees, const QDate &maxDate)
{
    d->assignees = assignees;
    d->maxDate = maxDate;
    invalidate();
}

void OpportunityFilterProxyModel::showAll()
{
    d->assignees.clear();
    invalidate();
}

static bool opportunityMatchesFilter(const SugarOpportunity &opportunity, const QString &filter)
{
    if (opportunity.name().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (opportunity.accountName().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (opportunity.salesStage().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (opportunity.amount().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (opportunity.dateClosed().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (opportunity.assignedUserName().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }

    return false;
}

bool OpportunityFilterProxyModel::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    const QModelIndex index = sourceModel()->index(row, 0, parent);
    const Akonadi::Item item =
        index.data(Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();

    Q_ASSERT(item.hasPayload<SugarOpportunity>());
    const SugarOpportunity opportunity = item.payload<SugarOpportunity>();

    if (!d->assignees.isEmpty()) {
        if (!d->assignees.contains(opportunity.assignedUserName()))
            return false;
        if (opportunity.salesStage().contains("Closed"))
            return false;
        if (opportunity.nextCallDate() > d->maxDate)
            return false;
    }

    const QString filterStr = filterString();
    if (!filterStr.isEmpty()) {
        return true;
    }

    return opportunityMatchesFilter(opportunity, filterStr);
}

#include "opportunityfilterproxymodel.moc"

