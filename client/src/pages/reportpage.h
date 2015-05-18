#ifndef REPORTPAGE_H
#define REPORTPAGE_H

#include <QWidget>

namespace Ui {
class ReportPage;
}
class ItemsTreeModel;

class ReportPage : public QWidget
{
    Q_OBJECT

public:
    explicit ReportPage(QWidget *parent = 0);
    ~ReportPage();

    void setOppModel(ItemsTreeModel *model);

private slots:
    void on_calculateOppCount_clicked();

private:
    Ui::ReportPage *ui;
    ItemsTreeModel *mOppModel;
};

#endif // REPORTPAGE_H
