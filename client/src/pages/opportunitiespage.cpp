/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "opportunitiespage.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "opportunityfilterwidget.h"
#include "opportunityfilterproxymodel.h"

#include "kdcrmdata/kdcrmfields.h"
#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/sugaropportunity.h"

#include <QDebug>
#include <QStyledItemDelegate>

using namespace Akonadi;

class OpportunityTreeViewItemDelegate: public QStyledItemDelegate
{
public:
    OpportunityTreeViewItemDelegate(QObject *parent)
        : QStyledItemDelegate(parent)
    {
    }

    // Return a null size for the description column:
    // (if hidden, because of QTBUG-8376, and if visible, because we prefer truncating over making rows higher)
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        if (index.column() == ItemsTreeModel::columnTypes(Opportunity).indexOf(ItemsTreeModel::Description)) {
            return QSize(0,0);
        }
        return QStyledItemDelegate::sizeHint(option, index);
    }
};

OpportunitiesPage::OpportunitiesPage(QWidget *parent)
    : Page(parent, SugarOpportunity::mimeType(), Opportunity)
{
    treeView()->setItemDelegate(new OpportunityTreeViewItemDelegate(this));
    mOppFilterProxyModel = new OpportunityFilterProxyModel(this);
    setFilter(mOppFilterProxyModel);

    mFilterUiWidget = new OpportunityFilterWidget(mOppFilterProxyModel, this);
    insertFilterWidget(mFilterUiWidget);
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
    return i18n("List of Opportunities");
}

QMap<QString, QString> OpportunitiesPage::dataForNewObject()
{
    QMap<QString, QString> initialData;
    initialData.insert(KDCRMFields::salesStage(), "Prospecting");
    initialData.insert(KDCRMFields::probability(), "10");
    initialData.insert(KDCRMFields::dateClosed(), KDCRMUtils::dateToString(QDate::currentDate().addMonths(12)));
    initialData.insert(KDCRMFields::nextCallDate(), KDCRMUtils::dateToString(QDate::currentDate().addDays(14)));
    return initialData;
}

QString OpportunitiesPage::idForItem(const Akonadi::Item &item) const
{
    if (item.hasPayload<SugarOpportunity>()) {
        const SugarOpportunity opportunity = item.payload<SugarOpportunity>();
        return opportunity.id();
    }
    return QString();
}

QString OpportunitiesPage::itemAddress() const
{
    return QString("?action=DetailView&module=Opportunities&record=");
}
