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
        if (!columns.contains(name)) {
            //kDebug() << "Hiding section" << i;
            header()->setSectionHidden(i, true);
        }
    }
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
