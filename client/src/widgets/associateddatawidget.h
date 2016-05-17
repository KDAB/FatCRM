#ifndef ASSOCIATEDDATAWIDGET_H
#define ASSOCIATEDDATAWIDGET_H

#include <QWidget>

namespace Ui {
class AssociatedDataWidget;
}

class QModelIndex;
class QStringListModel;

class AssociatedDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AssociatedDataWidget(QWidget *parent = 0);
    ~AssociatedDataWidget();

    void hideOpportunityGui();
    void setContactsModel(QStringListModel *model);
    void setOpportunitiesModel(QStringListModel *model);

Q_SIGNALS:
    void openItem(const QString &item);

public Q_SLOTS:
    void itemActivated(const QModelIndex &index);

private:
    Ui::AssociatedDataWidget *mUi;
};

#endif // ASSOCIATEDDATAWIDGET_H
