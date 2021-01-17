/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef REARRANGECOLUMNSPROXYMODEL_H
#define REARRANGECOLUMNSPROXYMODEL_H

#include <QIdentityProxyModel>
#include "fatcrm_client_debug.h"

/**
 * Selects columns from the source model, in any order.
 *
 * Supported: not selecting all columns (i.e. this is not just rearranging,
 *   but also filtering out some columns).
 * Supported: source model being a tree, dataChanged.
 * Not supported: selecting the same column more than once.
 *
 */
class RearrangeColumnsProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit RearrangeColumnsProxyModel(QObject *parent = nullptr);

    // API

    // Set the chosen source columns, in the desired order for the proxy columns
    // columns[proxyColumn=0] is the source column to show in the first proxy column, etc.
    void setSourceColumns(const QVector<int> &columns);

    // Implementation

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QModelIndex mapFromSource(const QModelIndex & sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex & proxyIndex) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    int proxyColumnForSourceColumn(int sourceColumn) const;
    int sourceColumnForProxyColumn(int proxyColumn) const;

private:
    // Configuration
    QVector<int> m_sourceColumns;
};

#endif
