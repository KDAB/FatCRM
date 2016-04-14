/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QDate>

#include <Akonadi/EntityTreeModel>
#include <Akonadi/Item>

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

static QDate firstDayOfMonth(const QDate &date)
{
    return QDate(date.year(), date.month(), 1);
}

static QDate lastDayOfMonth(const QDate &date)
{
    return QDate(date.year(), date.month(), date.daysInMonth());
}

ReportPage::ReportPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReportPage),
    mOppModel(Q_NULLPTR)
{
    ui->setupUi(this);

    // TODO: use OpportunityFilterWidget, to be able to filter on country / assignee / etc. ?

    // 12 months before the beginning of this month
    const QDate today = QDate::currentDate();
    ui->from->setDate(QDate(today.year() - 1, today.month(), 1));
    // last day of last month
    ui->to->setDate(firstDayOfMonth(today).addDays(-1));

    ui->pbMonthlySpreadsheet->setEnabled(false);
}

ReportPage::~ReportPage()
{
    delete ui;
}

void ReportPage::setOppModel(ItemsTreeModel *model)
{
    mOppModel = model;
}

// If @p from is month 0, return how many months @p date is after @p from.
// Example: relativeMonthNumber( feb 2016, nov 2015 ) = 3
//          (0=nov, 1=dec, 2=jan, 3=feb)
//          12 + 2 - 11 = 3
static int relativeMonthNumber(const QDate &date, const QDate &from)
{
    return ( date.year() - from.year() ) * 12 + date.month() - from.month();
}

QStringList ReportPage::headerLabels() const
{
    return QStringList() << i18n("Created") << i18n("Won") << i18n("Lost");
}

void ReportPage::on_calculateOppCount_clicked()
{
    // The date in this map is always day==1, so the key is the month+year.
    const QDate monthFrom = firstDayOfMonth(ui->from->date());
    const QDate monthTo = lastDayOfMonth(ui->to->date());
    const int numMonths = ( monthTo.year() - monthFrom.year() ) * 12 + monthTo.month() - monthFrom.month() + 1;
    numCreated.fill(0, numMonths);
    numWon.fill(0, numMonths);
    numLost.fill(0, numMonths);
    for (int i = 0; i < mOppModel->rowCount(); ++i) {
        const QModelIndex index = mOppModel->index(i, 0);
        const Akonadi::Item item = mOppModel->data(index, Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();
        if (item.hasPayload<SugarOpportunity>()) {
            const SugarOpportunity opportunity = item.payload<SugarOpportunity>();

            const QDate created = KDCRMUtils::dateTimeFromString(opportunity.dateEntered()).date();
            if (created >= ui->from->date() && created <= ui->to->date()) {
                ++numCreated[relativeMonthNumber(created, monthFrom)];
            }

            const QString salesStage = opportunity.salesStage();
            if (salesStage.contains("Closed")) {
                const QDate dateModified = opportunity.dateModified().date();
                // ### This is not exactly correct, if an opp is modified again after being closed
                // (which happens when adding a custom field.....)
                if (dateModified >= ui->from->date() && dateModified <= ui->to->date()) {
                    if (salesStage.contains("Closed Won") ) {
                        ++numWon[relativeMonthNumber(dateModified, monthFrom)];
                    } else if (salesStage.contains("Closed Lost")) {
                        ++numLost[relativeMonthNumber(dateModified, monthFrom)];
                    }
                }
            }

        }
    }
    ui->table->setColumnCount(numMonths);
    const QStringList labels = headerLabels();
    ui->table->setRowCount(labels.count());
    ui->table->setVerticalHeaderLabels(labels);
    enum { ROW_CREATED, ROW_WON, ROW_LOST };
    QDate monthStart = monthFrom;
    for (int month = 0; month < numMonths; ++month) {
        const QString monthName = monthStart.toString("MMM yyyy");
        monthStart = monthStart.addMonths(1);
        ui->table->setHorizontalHeaderItem(month, new QTableWidgetItem(monthName));
        const int created = numCreated.at(month);
        ui->table->setItem(ROW_CREATED, month, new QTableWidgetItem(QString::number(created)));
        const int won = numWon.at(month);
        ui->table->setItem(ROW_WON, month, new QTableWidgetItem(QString::number(won)));
        const int lost = numLost.at(month);
        ui->table->setItem(ROW_LOST, month, new QTableWidgetItem(QString::number(lost)));
    }
    ui->pbMonthlySpreadsheet->setEnabled(true);
}

void ReportPage::on_pbMonthlySpreadsheet_clicked()
{
    const QString csvFile = QFileDialog::getSaveFileName(this, i18n("Save to CSV file"), QString(), "*.csv");
    if (!csvFile.isEmpty()) {
        QFile file(csvFile);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, i18n("Error while saving"), i18n("Could not open file %1 for writing, check permissions", csvFile));
        } else {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            const QChar sep(',');
            const int numMonths = numCreated.size();
            stream << i18n("Month") << sep << headerLabels().join(sep) << "\n";
            const QDate monthFrom = firstDayOfMonth(ui->from->date());
            QDate monthStart = monthFrom;
            for (int month = 0; month < numMonths; ++month) {
                const QString monthName = monthStart.toString("MMM yyyy");
                monthStart = monthStart.addMonths(1);
                stream << monthName << sep
                       << numCreated.at(month) << sep
                       << numWon.at(month) << sep
                       << numLost.at(month) << "\n";
            }
        }
    }
}
