#include "reportpage.h"
#include "ui_reportpage.h"

#include "itemstreemodel.h"
#include <sugaropportunity.h>
#include <kdcrmutils.h>

#include <akonadi/entitytreemodel.h>
#include <akonadi/item.h>

ReportPage::ReportPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReportPage),
    mOppModel(Q_NULLPTR)
{
    ui->setupUi(this);

    // beginning of this month
    const QDate today = QDate::currentDate();
    ui->from->setDate(QDate(today.year(), today.month(), 1));
    // end of this month
    ui->to->setDate(QDate(today.year(), today.month(), today.daysInMonth()));
}

ReportPage::~ReportPage()
{
    delete ui;
}

void ReportPage::setOppModel(ItemsTreeModel *model)
{
    mOppModel = model;
}

void ReportPage::on_calculateOppCount_clicked()
{
    int createdCount = 0;
    int wonCount = 0;
    int lostCount = 0;
    for (int i = 0; i < mOppModel->rowCount(); ++i) {
        const QModelIndex index = mOppModel->index(i, 0);
        const Akonadi::Item item = mOppModel->data(index, Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();
        if (item.hasPayload<SugarOpportunity>()) {
            const SugarOpportunity opportunity = item.payload<SugarOpportunity>();

            const QDate created = KDCRMUtils::dateTimeFromString(opportunity.dateEntered()).date();
            if (created >= ui->from->date() && created <= ui->to->date()) {
                ++createdCount;
            }

            const QString salesStage = opportunity.salesStage();
            if (salesStage.contains("Closed")) {
                const QDate dateModified = KDCRMUtils::dateTimeFromString(opportunity.dateModified()).date();
                if (dateModified >= ui->from->date() && dateModified <= ui->to->date()) {
                    if (salesStage.contains("Closed Won") ) {
                        ++wonCount;
                    } else if (salesStage.contains("Closed Lost")) {
                        ++lostCount;
                    }
                }
            }

        }
    }
    QString reportText = QString("%1 opportunities created, %2 opportunities won, %3 opportunities lost");
    reportText = reportText.arg(createdCount).arg(wonCount).arg(lostCount);
    ui->result->setText(reportText);
}
