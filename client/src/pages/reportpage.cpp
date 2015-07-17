/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "reportpage.h"
#include "ui_reportpage.h"
#include "itemstreemodel.h"
#include "sugaropportunity.h"
#include "kdcrmutils.h"

#include <AkonadiCore/EntityTreeModel>
#include <AkonadiCore/Item>

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
                const QDate dateModified = opportunity.dateModified().date();
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
