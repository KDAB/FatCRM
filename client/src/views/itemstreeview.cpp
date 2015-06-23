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

#include "itemstreeview.h"
#include "itemstreemodel.h"
#include "clientsettings.h"

#include <QAction>
#include <QHeaderView>
#include <QtGui>

ItemsTreeView::ItemsTreeView(QWidget *parent) :
    Akonadi::EntityTreeView(parent),
    mItemsTreeModel(0)
{
    setRootIsDecorated(false);
    header()->setResizeMode(QHeaderView::ResizeToContents);

    header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header(), SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotHeaderContextMenu(QPoint)));
}

void ItemsTreeView::setViewName(const QString &name)
{
    setObjectName(name);
}

void ItemsTreeView::setModels(QAbstractItemModel *model, ItemsTreeModel *sourceModel, const ItemsTreeModel::ColumnTypes &defaultColumns)
{
    setModel(model);
    mItemsTreeModel = sourceModel;

    QStringList defaultColumnNames;
    foreach(ItemsTreeModel::ColumnType ct, defaultColumns)
        defaultColumnNames.append(ItemsTreeModel::columnNameFromType(ct));

    const QStringList columns = ClientSettings::self()->visibleColumns(objectName(), defaultColumnNames);
    //kDebug() << "wanted columns:" << columns;
    for (int i = 0; i < header()->count(); ++i) {
        const QString name = mItemsTreeModel->columnName(i);
        header()->setSectionHidden(i, !columns.contains(name));
    }
}

void ItemsTreeView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        const QModelIndex idx = currentIndex();
        if (idx.isValid()) {
            const Akonadi::Item item = idx.data(Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();
            if (item.isValid()) {
                emit returnPressed(item);
            }
        }
    }
    Akonadi::EntityTreeView::keyPressEvent(event);
}

void ItemsTreeView::slotHeaderContextMenu(const QPoint &point)
{
    const int section = header()->logicalIndexAt(point);

    QMenu menu;
    for (int i = 0; i < header()->count(); ++i) {
        const QString title = model()->headerData(i, Qt::Horizontal).toString();
        QAction *showHideAction = new QAction(title, &menu);
        showHideAction->setCheckable(true);
        showHideAction->setChecked(!header()->isSectionHidden(i));
        showHideAction->setData(i);
        menu.addAction(showHideAction);
    }

    const ItemsTreeModel::ColumnType columnType = mItemsTreeModel->columnTypes().at(section);
    switch (columnType) {
    case ItemsTreeModel::NextStepDate:
    case ItemsTreeModel::CreationDate:
    case ItemsTreeModel::LastModifiedDate:
        qDebug() << "Clicked on a date!";
    default:
        break;
    }

    QAction *selectedAction = menu.exec(header()->mapToGlobal(point));
    if (selectedAction) {
        const QVariant var = selectedAction->data();
        if (var.type() == QVariant::Int) {
            const int toggleSection = var.toInt();
            const bool hide = !selectedAction->isChecked();
            header()->setSectionHidden(toggleSection, hide);
            saveVisibleColumns();
        }
    }

}

void ItemsTreeView::saveVisibleColumns()
{
    QStringList columns;
    for (int i = 0; i < header()->count(); ++i) {
        if (!header()->isSectionHidden(i)) {
            const QString name = mItemsTreeModel->columnName(i);
            columns.append(name);
        }
    }
    ClientSettings::self()->setVisibleColumns(objectName(), columns);
}
