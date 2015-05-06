#include "itemstreeview.h"
#include "itemstreemodel.h"

#include <QHeaderView>

ItemsTreeView::ItemsTreeView(QWidget *parent) :
    Akonadi::EntityTreeView(parent),
    mItemsTreeModel(0)
{
    header()->setResizeMode(QHeaderView::ResizeToContents);

    header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header(), SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotHeaderContextMenu(QPoint)));
}

void ItemsTreeView::setModels(QAbstractItemModel *model, ItemsTreeModel *sourceModel)
{
    setModel(model);
    mItemsTreeModel = sourceModel;
}

void ItemsTreeView::slotHeaderContextMenu(const QPoint &point)
{
    const int section = header()->logicalIndexAt(point);
    const ItemsTreeModel::Column columnType = mItemsTreeModel->columns().at(section);
    switch (columnType) {
    case ItemsTreeModel::NextStepDate:
    case ItemsTreeModel::CreationDate:
    case ItemsTreeModel::LastModifiedDate:
        qDebug() << "Clicked on a date!";
    default:
        break;
    }
}

