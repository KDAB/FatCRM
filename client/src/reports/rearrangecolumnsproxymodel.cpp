/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "rearrangecolumnsproxymodel.h"

RearrangeColumnsProxyModel::RearrangeColumnsProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

void RearrangeColumnsProxyModel::setSourceColumns(const QVector<int> &columns)
{
    m_sourceColumns = columns;
}

int RearrangeColumnsProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_sourceColumns.count();
}

int RearrangeColumnsProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    // The parent in the source model is on column 0, whatever swapping we are doing
    const QModelIndex sourceParent = mapToSource(parent).sibling(parent.row(), 0);
    return sourceModel()->rowCount(sourceParent);
}

// We derive from QIdentityProxyModel simply to be able to use
// its mapFromSource method which has friend access to createIndex() in the source model.

QModelIndex RearrangeColumnsProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    Q_ASSERT(row >= 0);
    Q_ASSERT(column >= 0);
    // The parent in the source model is on column 0, whatever swapping we are doing
    const QModelIndex sourceParent = mapToSource(parent).sibling(parent.row(), 0);
    // Find the child in the source model, we need its internal pointer
    const QModelIndex sourceIndex = sourceModel()->index(row, sourceColumnForProxyColumn(column), sourceParent);
    Q_ASSERT(sourceIndex.isValid());
    return createIndex(row, column, sourceIndex.internalPointer());
}

QModelIndex RearrangeColumnsProxyModel::parent(const QModelIndex &child) const
{
    Q_ASSERT(child.isValid() ? child.model() == this : true);
    const QModelIndex sourceIndex = mapToSource(child);
    const QModelIndex sourceParent = sourceIndex.parent();
    if (!sourceParent.isValid())
        return QModelIndex();
    return createIndex(sourceParent.row(), 0, sourceParent.internalPointer());
}

QModelIndex RearrangeColumnsProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex();
    Q_ASSERT(sourceIndex.model() == sourceModel());
    const int proxyColumn = proxyColumnForSourceColumn(sourceIndex.column());
    return createIndex(sourceIndex.row(), proxyColumn, sourceIndex.internalPointer());
}

QModelIndex RearrangeColumnsProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid())
        return QModelIndex();
    // This is just an indirect way to call sourceModel->createIndex(row, sourceColumn, pointer)
    const QModelIndex fakeIndex = createIndex(proxyIndex.row(), sourceColumnForProxyColumn(proxyIndex.column()), proxyIndex.internalPointer());
    return QIdentityProxyModel::mapToSource(fakeIndex);
}

QVariant RearrangeColumnsProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        const int sourceSection = sourceColumnForProxyColumn(section);
        return QIdentityProxyModel::headerData(sourceSection, orientation, role);
    }
    return QIdentityProxyModel::headerData(section, orientation, role);
}

int RearrangeColumnsProxyModel::proxyColumnForSourceColumn(int sourceColumn) const
{
    // ### for performance we could add a second QVector with index=logical_source_column value=desired_pos_in_proxy.
    return m_sourceColumns.indexOf(sourceColumn);
}

int RearrangeColumnsProxyModel::sourceColumnForProxyColumn(int proxyColumn) const
{
    Q_ASSERT(proxyColumn >= 0);
    Q_ASSERT(proxyColumn < m_sourceColumns.size());
    return m_sourceColumns.at(proxyColumn);
}
