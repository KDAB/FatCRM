/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "linkeditemsrepository.h"
#include "opportunitydataextractor.h"
#include "opportunityfilterwidget.h"
#include "opportunityfilterproxymodel.h"
#include "referenceddata.h"
#include "fatcrm_client_debug.h"
#include "clientsettings.h"

#include "kdcrmdata/kdcrmfields.h"
#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/sugaropportunity.h"

#include <KLocalizedString>

#include <QPainter>
#include <QStyledItemDelegate>
#include <QSettings>

using namespace Akonadi;

class OpportunityTreeViewItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    OpportunityTreeViewItemDelegate(QObject *parent)
        : QStyledItemDelegate(parent)
    {
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        if (index.column() == ItemsTreeModel::columnTypes(DetailsType::Opportunity).indexOf(ItemsTreeModel::OpportunityName)) {
            QStyleOptionViewItem opt = option;
            initStyleOption(&opt, index);

            const QStyle *style = option.widget ? option.widget->style() : QApplication::style();
            style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, option.widget);

            const QPalette palette = option.widget ? option.widget->palette() : QApplication::palette();

            // use Qt::EditRole so we get the integer representation
            const int probabilityColumn = ItemsTreeModel::columnTypes(DetailsType::Opportunity).indexOf(ItemsTreeModel::Probability);
            const double fillPercent = index.sibling(index.row(), probabilityColumn).data(Qt::EditRole).toInt() / 100.0;

            painter->save();
            painter->setBrush(palette.text());
            painter->setOpacity(0.4);
            painter->setPen(Qt::NoPen);
            painter->drawRect(QRect(opt.rect.topLeft(), QSize(static_cast<int>(opt.rect.width() * fillPercent), 2)));
            painter->restore();
        } else {
            QStyledItemDelegate::paint(painter, option, index);
        }
    }

    // Return a null size for the description column:
    // (if hidden, because of QTBUG-8376, and if visible, because we prefer truncating over making rows higher)
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        if (index.column() == ItemsTreeModel::columnTypes(DetailsType::Opportunity).indexOf(ItemsTreeModel::Description)) {
            return QSize(0,0);
        }
        return QStyledItemDelegate::sizeHint(option, index);
    }
};

OpportunitiesPage::OpportunitiesPage(QWidget *parent)
    : Page(parent, SugarOpportunity::mimeType(), DetailsType::Opportunity), mDataExtractor(new OpportunityDataExtractor)
{
    treeView()->setItemDelegate(new OpportunityTreeViewItemDelegate(this));
    mOppFilterProxyModel = new OpportunityFilterProxyModel(this);
    setFilter(mOppFilterProxyModel);

    mFilterUiWidget = new OpportunityFilterWidget(mOppFilterProxyModel, this);
    insertFilterWidget(mFilterUiWidget);

    connect(mFilterUiWidget, &OpportunityFilterWidget::filterUpdated, this, &OpportunitiesPage::slotDefaultOppFilterUpdated);
}

OpportunitiesPage::~OpportunitiesPage()
{
}

void OpportunitiesPage::createOpportunity(const QString &accountId)
{
    QMap<QString, QString> opportunityData;
    opportunityData.insert(KDCRMFields::accountId(), accountId);
    createNewItem(opportunityData);
}

QMap<QString, QString> OpportunitiesPage::dataForNewObject()
{
    QMap<QString, QString> initialData;
    initialData.insert(KDCRMFields::salesStage(), QStringLiteral("Prospecting"));
    initialData.insert(KDCRMFields::probability(), QStringLiteral("10"));
    initialData.insert(KDCRMFields::dateClosed(), KDCRMUtils::dateToString(QDate::currentDate().addMonths(12)));
    initialData.insert(KDCRMFields::nextCallDate(), KDCRMUtils::dateToString(QDate::currentDate().addDays(14)));
    return initialData;
}

QString OpportunitiesPage::reportTitle() const
{
    return i18n("List of Opportunities");
}

void OpportunitiesPage::handleNewRows(int start, int end, bool emitChanges)
{
    //qDebug();
    ItemsTreeModel *treeModel = itemsTreeModel();
    QMap<QString, QString> assignedToRefMap;
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = treeModel->index(row, 0);
        const Item item = treeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarOpportunity>()) {
            const SugarOpportunity opportunity = item.payload<SugarOpportunity>();
            assignedToRefMap.insert(opportunity.assignedUserId(), opportunity.assignedUserName());
            linkedItemsRepository()->addOpportunity(opportunity);
        }
    }
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap, emitChanges);
}

void OpportunitiesPage::handleRemovedRows(int start, int end, bool initialLoadingDone)
{
    Q_UNUSED(initialLoadingDone);
    ItemsTreeModel *treeModel = itemsTreeModel();
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = treeModel->index(row, 0);
        const Item item = treeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarOpportunity>()) {
            const SugarOpportunity opportunity = item.payload<SugarOpportunity>();
            linkedItemsRepository()->removeOpportunity(opportunity);
        }
    }
}

void OpportunitiesPage::handleItemChanged(const Item &item)
{
    if (!item.hasPayload<SugarOpportunity>()) {
        qWarning() << "Broken item, missing payload (or not an opportunity)" << item.id() << item.remoteId();
        return;
    }
    const SugarOpportunity opp = item.payload<SugarOpportunity>();
    const QString id = opp.id();
    if (id.isEmpty()) {
        return;
    }
    linkedItemsRepository()->updateOpportunity(opp);
}

void OpportunitiesPage::setSearchPrefix(const QString &searchPrefix)
{
    mFilterUiWidget->setSearchPrefix(searchPrefix);
}

void OpportunitiesPage::setSearchName(const QString &searchName)
{
    mFilterUiWidget->setSearchName(searchName);
}

void OpportunitiesPage::setSearchText(const QString &searchText)
{
    mFilterUiWidget->setSearchText(searchText);
}

void OpportunitiesPage::saveSearch()
{
    mFilterUiWidget->saveSearch();
}

void OpportunitiesPage::loadSearch(const QString &searchPrefix)
{
    mFilterUiWidget->loadSearch(searchPrefix);
}

ItemDataExtractor *OpportunitiesPage::itemDataExtractor() const
{
    return mDataExtractor.get();
}

void OpportunitiesPage::slotDefaultOppFilterUpdated(const OpportunityFilterSettings &settings)
{
    ClientSettings::self()->setFilterSettings(settings);
}

#include "opportunitiespage.moc"
