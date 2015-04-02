#ifndef OPPORTUNITYFILTERWIDGET_H
#define OPPORTUNITYFILTERWIDGET_H

#include <QWidget>

namespace Ui {
class OpportunityFilterWidget;
}
class OpportunityFilterProxyModel;

class OpportunityFilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OpportunityFilterWidget(OpportunityFilterProxyModel *oppFilterProxyModel, QWidget *parent = 0);
    ~OpportunityFilterWidget();

private Q_SLOTS:
    void filterChanged();

private:
    Ui::OpportunityFilterWidget *ui;
    OpportunityFilterProxyModel *m_oppFilterProxyModel;
};

#endif // OPPORTUNITYFILTERWIDGET_H
