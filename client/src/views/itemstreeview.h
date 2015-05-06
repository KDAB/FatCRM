#ifndef ITEMSTREEVIEW_H
#define ITEMSTREEVIEW_H

#include <akonadi/entitytreeview.h>
class ItemsTreeModel;

class ItemsTreeView : public Akonadi::EntityTreeView
{
    Q_OBJECT
public:
    explicit ItemsTreeView(QWidget *parent = 0);

    void setModels(QAbstractItemModel *model, ItemsTreeModel *sourceModel);
signals:

public slots:

private slots:
    void slotHeaderContextMenu(const QPoint &point);

private:
    ItemsTreeModel* mItemsTreeModel;

};

#endif // ITEMSTREEVIEW_H
