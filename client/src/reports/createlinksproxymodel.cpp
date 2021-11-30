/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2019-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Kevin Funk <kevin.funk@kdab.com>

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

#include "createlinksproxymodel.h"

#include "accountdataextractor.h"
#include "contactdataextractor.h"
#include <Akonadi/EntityTreeModel>
#include "itemstreemodel.h"
#include "opportunitydataextractor.h"

using namespace Akonadi;

CreateLinksProxyModel::CreateLinksProxyModel(const QString &resourceBaseUrl, QObject* parent)
    : QIdentityProxyModel(parent)
    , mResourceBaseUrl(resourceBaseUrl)
{
}

QVariant CreateLinksProxyModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        auto linkedName = [this, role](const ItemDataExtractor &itemDataExtractor, const QModelIndex &index) {
            const Item item = index.data(EntityTreeModel::ItemRole).value<Item>();
            const QString name = sourceModel()->data(mapToSource(index), role).toString();
            const QUrl url = itemDataExtractor.itemUrl(mResourceBaseUrl, item);
            if (url.isValid())
                return QStringLiteral("<html><a href=\"%1\">%2</a></html>").arg(url.toString(), name );

            return name;
        };

        switch (index.data(ItemsTreeModel::ColumnTypeRole).value<ItemsTreeModel::ColumnType>()) {
        case ItemsTreeModel::Name:
            return linkedName(AccountDataExtractor(), index);
        case ItemsTreeModel::OpportunityName:
            return linkedName(OpportunityDataExtractor(), index);
        case ItemsTreeModel::FullName:
            return linkedName(ContactDataExtractor(), index);
        default:
            break;
        }
    }

    return QIdentityProxyModel::data(index, role);
}
