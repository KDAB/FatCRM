#include "opportunityfilterwidget.h"
#include "ui_opportunityfilterwidget.h"
#include "opportunityfilterproxymodel.h"
#include <QDate>
#include <QDebug>

OpportunityFilterWidget::OpportunityFilterWidget(OpportunityFilterProxyModel *oppFilterProxyModel,
                                                 QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpportunityFilterWidget),
    m_oppFilterProxyModel(oppFilterProxyModel)
{
    ui->setupUi(this);
    ui->rbAssignedTo->setChecked(true);

    connect(ui->rbAll, SIGNAL(clicked(bool)), m_oppFilterProxyModel, SLOT(showAll()));
    connect(ui->rbAssignedTo, SIGNAL(clicked(bool)), this, SLOT(filterChanged()));
    connect(ui->cbAssignee, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(ui->cbMaxNextStepDate, SIGNAL(activated(QString)), this, SLOT(filterChanged()));

    filterChanged();
}

OpportunityFilterWidget::~OpportunityFilterWidget()
{
    delete ui;
}

void OpportunityFilterWidget::filterChanged()
{
    ui->rbAssignedTo->setChecked(true);
    QStringList assignees;
    if (ui->cbAssignee->currentIndex() == 0) {
        // "me"
        assignees << "Sabine Faure"; // TODO config for groups
    }
    QDate maxDate = QDate::currentDate();
    switch (ui->cbMaxNextStepDate->currentIndex()) {
    case 0: // Today
        break;
    case 1: // End of this week, i.e. next Sunday
        // Ex: dayOfWeek = 3 (Wednesday), must add 4 days.
        maxDate = maxDate.addDays(7 - maxDate.dayOfWeek());
        break;
    case 2: // End of this month
        maxDate = QDate(maxDate.year(), maxDate.month(), maxDate.daysInMonth());
        break;
    }
    qDebug() << "maxDate=" << maxDate;

    m_oppFilterProxyModel->setFilter(assignees, maxDate);
}
