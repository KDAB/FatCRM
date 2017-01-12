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
#include "clientsettings.h"
#include "itemstreemodel.h"
#include "sugaropportunity.h"
#include "kdcrmutils.h"
#include "referenceddata.h"

#include <AkonadiCore/EntityTreeModel>
#include <AkonadiCore/Item>

#include <QDate>
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
    mOppModel(nullptr)
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

void ReportPage::on_calculateCreatedWonLostReport_clicked()
{
    // The date in this map is always day==1, so the key is the month+year.
    const QDate monthFrom = firstDayOfMonth(ui->from->date());
    const QDate monthTo = lastDayOfMonth(ui->to->date());
    const int numMonths = ( monthTo.year() - monthFrom.year() ) * 12 + monthTo.month() - monthFrom.month() + 1;

    QVector<int> numCreated, numWon, numLost, avgAgeWon, avgAgeLost;
    numCreated.fill(0, numMonths);
    numWon.fill(0, numMonths);
    numLost.fill(0, numMonths);
    avgAgeWon.fill(0, numMonths);
    avgAgeLost.fill(0, numMonths);

    QVector<QStringList> detailsCreated, detailsWon, detailsLost;
    detailsCreated.resize(numMonths);
    detailsWon.resize(numMonths);
    detailsLost.resize(numMonths);

    for (int i = 0; i < mOppModel->rowCount(); ++i) {
        const QModelIndex index = mOppModel->index(i, 0);
        const Akonadi::Item item = mOppModel->data(index, Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();
        if (item.hasPayload<SugarOpportunity>()) {
            const SugarOpportunity opportunity = item.payload<SugarOpportunity>();
            const QString accountName = ReferencedData::instance(AccountRef)->referencedData(opportunity.accountId());
            const QString name = accountName + QLatin1String(" -- ") + opportunity.name();

            const QDate createdDate = KDCRMUtils::dateTimeFromString(opportunity.dateEntered()).date();
            if (createdDate >= ui->from->date() && createdDate <= ui->to->date()) {
                const int month = relativeMonthNumber(createdDate, monthFrom);
                ++numCreated[month];
                detailsCreated[month].append(name);
            }

            const QString salesStage = opportunity.salesStage();
            if (salesStage.contains(QLatin1String("Closed"))) {
                // FatCRM now sets dateClosed when closing an opp, but older versions didn't do it,
                // and Sugar Web doesn't do it, so we use dateModified as fallback, when it's clearly
                // more correct (earlier than dateClosed).
                const QDate closedDate = qMin(KDCRMUtils::dateFromString(opportunity.dateClosed()),
                                              opportunity.dateModified().date());

                if (closedDate >= ui->from->date() && closedDate <= ui->to->date()) {
                    const int month = relativeMonthNumber(closedDate, monthFrom);
                    if (salesStage.contains(QLatin1String("Closed Won")) ) {
                        ++numWon[month];
                        detailsWon[month].append(name);
                        avgAgeWon[month] += createdDate.daysTo(closedDate);
                    } else if (salesStage.contains(QLatin1String("Closed Lost"))) {
                        ++numLost[month];
                        detailsLost[month].append(name);
                        avgAgeLost[month] += createdDate.daysTo(closedDate);
                    }
                }
            }
        }
    }

    for (int month = 0; month < numMonths; ++month) {
        if (numWon[month] != 0)
            avgAgeWon[month] = avgAgeWon[month] / numWon[month];

        if (numLost[month] != 0)
            avgAgeLost[month] = avgAgeLost[month] / numLost[month];
    }

    ui->table->clear();
    ui->table->setColumnCount(numMonths);
    const QStringList labels = QStringList() << i18n("Created") << i18n("Won") << i18n("Lost") << i18n("Avg. Age Won") << i18n("Avg. Age Lost");

    ui->table->setRowCount(labels.count());
    ui->table->setVerticalHeaderLabels(labels);
    enum { ROW_CREATED, ROW_WON, ROW_LOST, ROW_AVG_AGE_WON, ROW_AVG_AGE_LOST };
    QDate monthStart = monthFrom;

    const QChar newline('\n');

    QTableWidgetItem *item = nullptr;
    for (int month = 0; month < numMonths; ++month) {
        const QString monthName = monthStart.toString(QStringLiteral("MMM yyyy"));
        monthStart = monthStart.addMonths(1);

        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, monthName);
        item->setData(Qt::UserRole, monthName);
        ui->table->setHorizontalHeaderItem(month, item);

        const int created = numCreated.at(month);
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, QString::number(created));
        item->setData(Qt::UserRole, created);
        item->setToolTip(detailsCreated.at(month).join(newline));
        ui->table->setItem(ROW_CREATED, month, item);

        const int won = numWon.at(month);
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, QString::number(won));
        item->setData(Qt::UserRole, won);
        item->setToolTip(detailsWon.at(month).join(newline));
        ui->table->setItem(ROW_WON, month, item);

        const int lost = numLost.at(month);
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, QString::number(lost));
        item->setData(Qt::UserRole, lost);
        item->setToolTip(detailsLost.at(month).join(newline));
        ui->table->setItem(ROW_LOST, month, item);

        const int ageWon = avgAgeWon.at(month);
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, ki18ncp("number of days", "%1 day", "%1 days").subs(ageWon).toString());
        item->setData(Qt::UserRole, ageWon);
        ui->table->setItem(ROW_AVG_AGE_WON, month, item);

        const int ageLost = avgAgeLost.at(month);
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, ki18ncp("number of days", "%1 day", "%1 days").subs(ageLost).toString());
        item->setData(Qt::UserRole, ageLost);
        ui->table->setItem(ROW_AVG_AGE_LOST, month, item);
    }

    ui->pbMonthlySpreadsheet->setEnabled(true);
}

void ReportPage::on_calculateOpenPerCountryReport_clicked()
{
    // The date in this map is always day==1, so the key is the month+year.
    const QDate monthFrom = firstDayOfMonth(ui->from->date());
    const QDate monthTo = lastDayOfMonth(ui->to->date());
    const int numMonths = ( monthTo.year() - monthFrom.year() ) * 12 + monthTo.month() - monthFrom.month() + 1;

    // maps groupIndex to amount of open opportunities per month
    QVector<QVector<int> > numPerCountry;

    QVector<int> numTotal(numMonths, 0);

    const ClientSettings::GroupFilters groups = ClientSettings::self()->countryFilters();

    QStringList labels;

    // maps country name to group indexes
    QHash<QString, QSet<int> > groupIndexLookupHash;

    const int numGroups = groups.groups().count();
    numPerCountry.reserve(numGroups);

    int groupIndex = 0;
    foreach (const ClientSettings::GroupFilters::Group &group, groups.groups()) {
        labels << group.group;

        foreach (const QString &country, group.entries) {
            groupIndexLookupHash[country.toLower()].insert(groupIndex);
        }

        numPerCountry.append(QVector<int>(numMonths, 0));

        ++groupIndex;
    }
    labels << i18n("Total");

    for (int i = 0; i < mOppModel->rowCount(); ++i) {
        const QModelIndex index = mOppModel->index(i, 0);
        const Akonadi::Item item = mOppModel->data(index, Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();
        if (item.hasPayload<SugarOpportunity>()) {
            const SugarOpportunity opportunity = item.payload<SugarOpportunity>();

            const QDate createdDate = KDCRMUtils::dateTimeFromString(opportunity.dateEntered()).date();

            // FatCRM now sets dateClosed when closing an opp, but older versions didn't do it,
            // and Sugar Web doesn't do it, so we use dateModified as fallback, when it's clearly
            // more correct (earlier than dateClosed).
            const QDate closedDate = qMin(KDCRMUtils::dateFromString(opportunity.dateClosed()),
                                          opportunity.dateModified().date());

            const bool isClosed = opportunity.salesStage().contains(QLatin1String("Closed"));

            if (isClosed && closedDate < monthFrom) {
                // Opp too old to appear anywhere
                continue;
            }


            const QString country = AccountRepository::instance()->accountById(opportunity.accountId()).countryForGui().toLower();
            const QSet<int> groupIndexes = groupIndexLookupHash.value(country);

            if (groupIndexes.isEmpty()) { // not part of configured country groups -> count it in "Total"
                const QString accountName = ReferencedData::instance(AccountRef)->referencedData(opportunity.accountId());
                const QString country = AccountRepository::instance()->accountById(opportunity.accountId()).countryForGui();
                qDebug() << "Opp in no country group:" << accountName << opportunity.name() << "country" << country << "closed" << isClosed << closedDate;
            }

            for (QDate month = monthFrom; month <= monthTo; month = month.addMonths(1)) {
                const QDate lastDay = lastDayOfMonth(month);
                if (createdDate <= lastDay && (!isClosed || closedDate > lastDay)) {
                    const int monthIndex = relativeMonthNumber(month, monthFrom);

                    foreach (int groupIndex, groupIndexes) {
                        numPerCountry[groupIndex][monthIndex] += 1;
                    }

                    numTotal[monthIndex] += 1;
                }
            }
        }
    }

    ui->table->clear();
    ui->table->setColumnCount(numMonths);
    ui->table->setRowCount(labels.count());
    ui->table->setVerticalHeaderLabels(labels);

    QDate monthStart = monthFrom;
    QTableWidgetItem *item = nullptr;

    for (int month = 0; month < numMonths; ++month) {
        const QString monthName = monthStart.toString(QStringLiteral("MMM yyyy"));
        monthStart = monthStart.addMonths(1);

        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, monthName);
        item->setData(Qt::UserRole, monthName);
        ui->table->setHorizontalHeaderItem(month, item);

        for (int groupIndex = 0; groupIndex < numGroups; ++groupIndex) {
            const int count = numPerCountry[groupIndex][month];
            item = new QTableWidgetItem();
            item->setData(Qt::DisplayRole, QString::number(count));
            item->setData(Qt::UserRole, count);
            ui->table->setItem(groupIndex, month, item);
        }

        const int total = numTotal.value(month);
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, QString::number(total));
        item->setData(Qt::UserRole, total);
        ui->table->setItem(numGroups, month, item);
    }

    ui->pbMonthlySpreadsheet->setEnabled(true);
}

void ReportPage::on_pbMonthlySpreadsheet_clicked()
{
    const QString csvFile = QFileDialog::getSaveFileName(this, i18n("Save to CSV file"), QString(), QStringLiteral("*.csv"));
    if (!csvFile.isEmpty()) {
        QFile file(csvFile);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, i18n("Error while saving"), i18n("Could not open file %1 for writing, check permissions", csvFile));
        } else {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            const QChar sep(',');

            stream << i18n("Month") << sep;
            for (int row = 0; row < ui->table->rowCount(); ++row) {
                stream << ui->table->verticalHeaderItem(row)->text();
                if (row < (ui->table->rowCount() - 1))
                    stream << sep;
                else
                    stream << "\n";
            }

            for (int column = 0; column < ui->table->columnCount(); ++column) {
                stream << ui->table->horizontalHeaderItem(column)->text() << sep;
                for (int row = 0; row < ui->table->rowCount(); ++row) {
                    stream << ui->table->item(row, column)->data(Qt::UserRole).toString();
                    if (row < (ui->table->rowCount() - 1))
                        stream << sep;
                    else
                        stream << "\n";
                }
            }
        }
    }
}
