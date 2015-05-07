#include "opportunityfilterproxymodel.h"

#include <kdcrmdata/sugaropportunity.h>

#include <akonadi/entitytreemodel.h>
#include "itemstreemodel.h"

#include <QDate>
#include <opportunitiespage.h>
#include <referenceddata.h>

using namespace Akonadi;

class OpportunityFilterProxyModel::Private
{
public:
    Private()
        : showOpen(true),
          showClosed(false)
    {}

    QStringList assignees; // no filtering if empty
    QStringList countries; // no filtering if empty
    QDate maxDate;
    bool showOpen;
    bool showClosed;
};

OpportunityFilterProxyModel::OpportunityFilterProxyModel(QObject *parent)
    : FilterProxyModel(Opportunity, parent), d(new Private())
{
}

OpportunityFilterProxyModel::~OpportunityFilterProxyModel()
{
    delete d;
}

void OpportunityFilterProxyModel::setFilter(const QStringList &assignees, const QStringList &countries, const QDate &maxDate,
                                            bool showOpen, bool showClosed)
{
    d->assignees = assignees;
    d->countries = countries;
    d->maxDate = maxDate;
    d->showOpen = showOpen;
    d->showClosed = showClosed;
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

    if (!d->assignees.isEmpty() && !d->assignees.contains(opportunity.assignedUserName()))
        return false;

    if (!d->countries.isEmpty()) {
        const QString country = ReferencedData::instance(AccountCountryRef)->referencedData(opportunity.accountName());
        if (!d->countries.contains(country))
            return false;
    }

    const bool isClosed = opportunity.salesStage().contains("Closed");
    if (!d->showClosed && isClosed)
        return false;
    if (!d->showOpen && !isClosed)
        return false;
    if (d->maxDate.isValid() && (!opportunity.nextCallDate().isValid()
                                 || opportunity.nextCallDate() > d->maxDate))
        return false;

    const QString filterStr = filterString();
    if (filterStr.isEmpty()) {
        return true;
    }

    return opportunityMatchesFilter(opportunity, filterStr);
}

bool OpportunityFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const ItemsTreeModel::ColumnTypes columns = ItemsTreeModel::columnTypes(Opportunity);
    const int nextStepDateColumn = columns.indexOf(ItemsTreeModel::NextStepDate);
    if (sortColumn() == nextStepDateColumn) {
        QVariant l = (left.model() ? left.model()->data(left, sortRole()) : QVariant());
        QVariant r = (right.model() ? right.model()->data(right, sortRole()) : QVariant());
        if (l.userType() == QVariant::Date) {
            QDate leftDt = l.toDate();
            QDate rightDt = r.toDate();
            if (leftDt == rightDt) {
                // compare last modified dates
                const int lastModifiedDateColumn = columns.indexOf(ItemsTreeModel::LastModifiedDate);
                leftDt = left.sibling(left.row(), lastModifiedDateColumn).data(sortRole()).toDate();
                rightDt = right.sibling(right.row(), lastModifiedDateColumn).data(sortRole()).toDate();
            }
            return leftDt < rightDt;
        }
    }
    return FilterProxyModel::lessThan(left, right);
}

#include "opportunityfilterproxymodel.moc"

