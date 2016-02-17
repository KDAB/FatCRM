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

#include "leadspage.h"

#include "filterproxymodel.h"
#include "itemstreemodel.h"
#include "leaddataextractor.h"

#include "kdcrmdata/sugarlead.h"

#include <QDebug>
#include <referenceddata.h>
#include <sugaropportunity.h>

using namespace Akonadi;

LeadsPage::LeadsPage(QWidget *parent)
    : Page(parent, QString(SugarLead::mimeType()), Lead), mDataExtractor(new LeadDataExtractor(this))
{
    setFilter(new FilterProxyModel(Lead, this));
}

LeadsPage::~LeadsPage()
{
}

QString LeadsPage::reportTitle() const
{
    return i18n("List of Leads");
}

void LeadsPage::handleNewRows(int start, int end, bool emitChanges)
{
    //kDebug();
    ItemsTreeModel *treeModel = itemsTreeModel();
    QMap<QString, QString> assignedToRefMap;
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = treeModel->index(row, 0);
        const Item item = treeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarOpportunity>()) {
            const SugarOpportunity opportunity = item.payload<SugarOpportunity>();
            assignedToRefMap.insert(opportunity.assignedUserId(), opportunity.assignedUserName());
        }
    }
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap, emitChanges);
}

ItemDataExtractor *LeadsPage::itemDataExtractor() const
{
    return mDataExtractor;
}
