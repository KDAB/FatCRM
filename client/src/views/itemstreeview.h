#ifndef ITEMSTREEVIEW_H
#define ITEMSTREEVIEW_H

#include <akonadi/entitytreeview.h>
#include "itemstreemodel.h"

class ItemsTreeView : public Akonadi::EntityTreeView
{
    Q_OBJECT
public:
    explicit ItemsTreeView(QWidget *parent = 0);

    void setViewName(const QString &name);

    void setModels(QAbstractItemModel *model, ItemsTreeModel *sourceModel, const ItemsTreeModel::ColumnTypes &defaultColumns);
signals:

public slots:

private slots:
    void slotHeaderContextMenu(const QPoint &point);

private:
    void saveVisibleColumns();

    ItemsTreeModel* mItemsTreeModel;
};

#endif // ITEMSTREEVIEW_H
