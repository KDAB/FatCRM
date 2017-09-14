/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "accountrepository.h"
#include "clientsettings.h"
#include "itemstreemodel.h"
#include "opportunitiespage.h"
#include "opportunityfiltersettings.h"
#include "referenceddata.h"

#include "kdcrmdata/sugaropportunity.h"

#include <AkonadiCore/EntityTreeModel>

#include <KLocalizedString>

#include <QDate>

using namespace Akonadi;

class OpportunityFilterProxyModel::Private
{
public:
    Private()
    {}

    OpportunityFilterSettings settings;

};

OpportunityFilterProxyModel::OpportunityFilterProxyModel(QObject *parent)
    : FilterProxyModel(Opportunity, parent), d(new Private())
{
}

OpportunityFilterProxyModel::~OpportunityFilterProxyModel()
{
    delete d;
}

void OpportunityFilterProxyModel::setFilter(const OpportunityFilterSettings &settings)
{
    d->settings = settings;
    invalidate();
}

QString OpportunityFilterProxyModel::filterDescription() const
{
    QString txt = d->settings.filterDescription();

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
    const QString accountName = ReferencedData::instance(AccountRef)->referencedData(opportunity.accountId());
    if (accountName.contains(filter, Qt::CaseInsensitive)) {
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
    if (opportunity.opportunitySize().contains(filter, Qt::CaseInsensitive)) {
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

    const QStringList assignees = d->settings.assignees();
    if (!assignees.isEmpty() && !assignees.contains(opportunity.assignedUserName()))
        return false;

    const QStringList countries = d->settings.countries();
    if (!countries.isEmpty()) {
        const QString country = AccountRepository::instance()->accountById(opportunity.accountId()).countryForGui();
        if (countries.contains(OpportunityFilterSettings::otherCountriesSpecialValue())) {
            // Special case: filtering for a country not in any of the defined groups
            QVector<ClientSettings::GroupFilters::Group> groups = ClientSettings::self()->countryFilters().groups();
            const auto it = std::find_if(groups.constBegin(), groups.constEnd(),
                                         [&country](const ClientSettings::GroupFilters::Group &group){
                return group.entries.contains(country, Qt::CaseInsensitive); });
            if (it != groups.constEnd())
                return false;
        } else {
            // Standard filtering using a country group
            if (!countries.contains(country, Qt::CaseInsensitive))
                return false;
        }
    }

    const bool isClosed = opportunity.salesStage().contains(QLatin1String("Closed"));
    if (!d->settings.showClosed() && isClosed)
        return false;
    if (!d->settings.showOpen() && !isClosed)
        return false;
    if (d->settings.maxDate().isValid() && (!opportunity.nextCallDate().isValid()
                                 || opportunity.nextCallDate() > d->settings.maxDate()))
        return false;
    if (d->settings.modifiedAfter().isValid() && opportunity.dateModified().date() < d->settings.modifiedAfter())
        return false;
    if (d->settings.modifiedBefore().isValid() && opportunity.dateModified().date() > d->settings.modifiedBefore())
        return false;

    QString shownPriority = d->settings.shownPriority();
    if (shownPriority == "Not set") // "Not set" is much clearer to me than just a blank space (like the WebUI has)
        shownPriority = "";
    if (shownPriority != "-" && opportunity.opportunityPriority().toUpper() != shownPriority)
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
