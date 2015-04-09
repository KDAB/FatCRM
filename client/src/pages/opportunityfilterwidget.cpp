#include "opportunityfilterwidget.h"
#include "ui_opportunityfilterwidget.h"
#include "opportunityfilterproxymodel.h"
#include "clientsettings.h"
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
    ui->cbMaxNextStepDate->setCurrentIndex(0);

    connect(ClientSettings::self(), SIGNAL(assigneeFiltersChanged()), this, SLOT(setupFromConfig()));

    setupFromConfig();
}

OpportunityFilterWidget::~OpportunityFilterWidget()
{
    delete ui;
}

void OpportunityFilterWidget::setupFromConfig()
{
    ui->cbAssignee->clear();
    ui->cbAssignee->addItem(tr("me"));
    ui->cbAssignee->addItems(ClientSettings::self()->assigneeFilters().groups());
    filterChanged();
}

void OpportunityFilterWidget::filterChanged()
{
    ui->rbAssignedTo->setChecked(true);
    QStringList assignees;
    const int idx = ui->cbAssignee->currentIndex();
    if (idx == 0) {
        // "me"
        assignees << ClientSettings::self()->fullUserName();
    } else {
        assignees << ClientSettings::self()->assigneeFilters().items().at(idx-1).users;
    }
    QDate maxDate = QDate::currentDate();
    switch (ui->cbMaxNextStepDate->currentIndex()) {
    case 0: // Any
        maxDate = QDate();
        break;
    case 1: // Today
        break;
    case 2: // End of this week, i.e. next Sunday
        // Ex: dayOfWeek = 3 (Wednesday), must add 4 days.
        maxDate = maxDate.addDays(7 - maxDate.dayOfWeek());
        break;
    case 3: // End of this month
        maxDate = QDate(maxDate.year(), maxDate.month(), maxDate.daysInMonth());
        break;
    case 4: // End of this year
        maxDate = QDate(maxDate.year(), 12, 31);
        break;
    }

    m_oppFilterProxyModel->setFilter(assignees, maxDate);
}
