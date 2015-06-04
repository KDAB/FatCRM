/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "opportunityfilterproxymodel.h"

#include <kdcrmdata/sugaropportunity.h>

#include <akonadi/entitytreemodel.h>
#include "itemstreemodel.h"
#include "kdcrmdata/kdcrmutils.h"

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
    QString assigneeGroup; // user-visible description for <assignee>
    QString countryGroup; // user-visible description for <countries>
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

void OpportunityFilterProxyModel::setFilterDescriptionData(const QString &assigneeGroup, const QString &countryGroup)
{
    d->assigneeGroup = assigneeGroup;
    d->countryGroup = countryGroup;
}

QString OpportunityFilterProxyModel::filterDescription() const
{
    QString openOrClosed;
    if (!d->showOpen && d->showClosed)
        openOrClosed = i18n("closed");
    else if (d->showOpen && !d->showClosed)
        openOrClosed = i18n("open");

    QString txt;
    if (!d->assignees.isEmpty()) {
        txt = i18n("Assigned to %1", d->assigneeGroup);
    } else if (!d->countries.isEmpty()) {
        txt = i18n("In country %1", d->countryGroup);
    }

    if (!openOrClosed.isEmpty()) {
        txt = i18n("%1, %2", txt, openOrClosed);
    }

    if (!d->maxDate.isNull()) {
        txt = i18n("%1, next step before %2", txt, KDCRMUtils::formatDate(d->maxDate));
    }

    if (!filterString().isEmpty()) {
        txt = i18n("%1, containing \"%2\"", txt, filterString());
    }

    return txt;

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
        if (!d->countries.contains(country, Qt::CaseInsensitive))
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

