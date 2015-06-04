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

    connect(ui->rbAll, SIGNAL(clicked(bool)), this, SLOT(filterChanged()));
    connect(ui->rbAssignedTo, SIGNAL(clicked(bool)), this, SLOT(filterChanged()));
    connect(ui->rbCountry, SIGNAL(clicked(bool)), this, SLOT(filterChanged()));
    connect(ui->cbAssignee, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(ui->cbMaxNextStepDate, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(ui->cbOpen, SIGNAL(clicked(bool)), this, SLOT(filterChanged()));
    connect(ui->cbClosed, SIGNAL(clicked(bool)), this, SLOT(filterChanged()));
    connect(ui->cbCountry, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(ui->modifiedAfter, SIGNAL(dateChanged(QDate)), this, SLOT(filterChanged()));
    connect(ui->modifiedBefore, SIGNAL(dateChanged(QDate)), this, SLOT(filterChanged()));

    ui->modifiedAfter->setNullable(true);
    ui->modifiedAfter->clear();
    ui->modifiedBefore->setNullable(true);
    ui->modifiedBefore->clear();
    ui->cbMaxNextStepDate->setCurrentIndex(0);

    connect(ClientSettings::self(), SIGNAL(assigneeFiltersChanged()), this, SLOT(setupFromConfig()));
    connect(ClientSettings::self(), SIGNAL(countryFiltersChanged()), this, SLOT(setupFromConfig()));

    ui->rbAssignedTo->setChecked(true);
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
    ui->cbAssignee->addItems(ClientSettings::self()->assigneeFilters().groupNames());
    ui->cbCountry->clear();
    ui->cbCountry->addItems(ClientSettings::self()->countryFilters().groupNames());
    filterChanged();
}

QDate OpportunityFilterWidget::maxNextStepDate() const
{
    QDate date = QDate::currentDate();
    switch (ui->cbMaxNextStepDate->currentIndex()) {
    case 0: // Any
        date = QDate();
        break;
    case 1: // Today
        break;
    case 2: // End of this week, i.e. next Sunday
        // Ex: dayOfWeek = 3 (Wednesday), must add 4 days.
        date = date.addDays(7 - date.dayOfWeek());
        break;
    case 3: // End of this month
        date = QDate(date.year(), date.month(), date.daysInMonth());
        break;
    case 4: // End of this year
        date = QDate(date.year(), 12, 31);
        break;
    }
    return date;
}

void OpportunityFilterWidget::filterChanged()
{
    QStringList assignees;
    QStringList countries;
    if (ui->rbAssignedTo->isChecked()) {
        const int idx = ui->cbAssignee->currentIndex();
        if (idx == 0) {
            // "me"
            assignees << ClientSettings::self()->fullUserName();
        } else {
            assignees = ClientSettings::self()->assigneeFilters().groups().at(idx-1).entries;
        }
    } else if (ui->rbCountry->isChecked()) {
        const int idx = ui->cbCountry->currentIndex();
        countries = ClientSettings::self()->countryFilters().groups().at(idx).entries;
    }
    const bool showOpen = ui->cbOpen->isChecked();
    const bool showClosed = ui->cbClosed->isChecked();
    const QDate modifiedAfter = ui->modifiedAfter->date();
    const QDate modifiedBefore = ui->modifiedBefore->date();
    m_oppFilterProxyModel->setFilter(assignees, countries, maxNextStepDate(), modifiedAfter, modifiedBefore, showOpen, showClosed);
    m_oppFilterProxyModel->setFilterDescriptionData(ui->cbAssignee->currentText(), ui->cbCountry->currentText());
}
