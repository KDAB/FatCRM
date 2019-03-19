/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "clientsettings.h"
#include "opportunityfilterproxymodel.h"
#include "opportunityfiltersettings.h"
#include "fatcrm_client_debug.h"

#include <KLocalizedString>

#include <QCalendarWidget>
#include <QDate>

OpportunityFilterWidget::OpportunityFilterWidget(OpportunityFilterProxyModel *oppFilterProxyModel,
                                                 QWidget *parent) :
    QWidget(parent),
    mCustomMaxNextStepDate(QDate()),
    ui(new Ui::OpportunityFilterWidget),
    m_oppFilterProxyModel(oppFilterProxyModel)
{
    ui->setupUi(this);
    ui->rbAssignedTo->setChecked(true);

    ui->modifiedAfter->setNullable(true);
    ui->modifiedAfter->clear();
    ui->modifiedBefore->setNullable(true);
    ui->modifiedBefore->clear();
    ui->cbMaxNextStepDate->setCurrentIndex(0);

    setupFromConfig();

    connect(ui->rbAll, SIGNAL(toggled(bool)), this, SLOT(filterChanged()));
    connect(ui->rbAssignedTo, SIGNAL(toggled(bool)), this, SLOT(filterChanged()));
    connect(ui->rbCountry, SIGNAL(toggled(bool)), this, SLOT(filterChanged()));
    connect(ui->cbAssignee, SIGNAL(activated(QString)), this, SLOT(slotAssigneeSelected()));
    connect(ui->cbAssignee, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(ui->cbMaxNextStepDate, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(ui->cbOpen, SIGNAL(toggled(bool)), this, SLOT(filterChanged()));
    connect(ui->cbClosed, SIGNAL(toggled(bool)), this, SLOT(filterChanged()));
    connect(ui->cbCountry, SIGNAL(activated(QString)), this, SLOT(slotCountrySelected()));
    connect(ui->cbCountry, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(ui->modifiedAfter, SIGNAL(dateChanged(QDate)), this, SLOT(filterChanged()));
    connect(ui->modifiedBefore, SIGNAL(dateChanged(QDate)), this, SLOT(filterChanged()));
    connect(ui->cbPriority, SIGNAL(currentIndexChanged(QString)), this, SLOT(filterChanged()));


    connect(ClientSettings::self(), SIGNAL(assigneeFiltersChanged()), this, SLOT(setupFromConfig()));
    connect(ClientSettings::self(), SIGNAL(countryFiltersChanged()), this, SLOT(setupFromConfig()));
}

OpportunityFilterWidget::~OpportunityFilterWidget()
{
    delete ui;
}

void OpportunityFilterWidget::setupFromConfig(const OpportunityFilterSettings &settings)
{
    ui->cbAssignee->clear();
    ui->cbAssignee->addItem(i18n("me"));
    ui->cbAssignee->addItems(ClientSettings::self()->assigneeFilters().groupNames());
    ui->cbCountry->clear();
    ui->cbCountry->addItems(ClientSettings::self()->countryFilters().groupNames());
    ui->cbCountry->addItem(i18n("Other"));
    ui->cbAssignee->setCurrentIndex(qMax(0, ui->cbAssignee->findText(settings.assigneeGroup())));
    ui->cbCountry->setCurrentIndex(qMax(0, ui->cbCountry->findText(settings.countryGroup())));
    ui->cbOpen->setChecked(settings.showOpen());
    ui->cbClosed->setChecked(settings.showClosed());
    if (settings.customMaxDate().isValid()) {
        ui->cbMaxNextStepDate->insertItem(CustomDate, settings.customMaxDate().toString(QStringLiteral("MM/d/yyyy")));
        mCustomMaxNextStepDate = settings.customMaxDate();
    }
    ui->cbMaxNextStepDate->setCurrentIndex(settings.maxDateIndex());
    ui->modifiedBefore->setDate(settings.modifiedBefore());
    ui->modifiedAfter->setDate(settings.modifiedAfter());
    ui->rbAll->setChecked(true); // unless one of the two below gets checked
    ui->rbAssignedTo->setChecked(!settings.assignees().isEmpty());
    ui->rbCountry->setChecked(!settings.countries().isEmpty());
    ui->cbPriority->setCurrentIndex(qMax(0, ui->cbPriority->findText(settings.shownPriority())));

    filterChanged();
}

void OpportunityFilterWidget::slotAssigneeSelected()
{
    ui->rbAssignedTo->setChecked(true);
}

void OpportunityFilterWidget::slotCountrySelected()
{
    ui->rbCountry->setChecked(true);
}

QDate OpportunityFilterWidget::maxNextStepDate() const
{
    QDate date = QDate::currentDate();
    switch (ui->cbMaxNextStepDate->currentIndex()) {
    case NoDate: // Any
        date = QDate();
        break;
    case OneMonthAgo:
        date = date.addDays(-31);
        break;
    case Today:
        break;
    case EndOfThisWeek: // i.e. next Sunday
        // Ex: dayOfWeek = 3 (Wednesday), must add 4 days.
        date = date.addDays(7 - date.dayOfWeek());
        break;
    case EndOfThisMonth:
        date = QDate(date.year(), date.month(), date.daysInMonth());
        break;
    case EndOfThisYear:
        date = QDate(date.year(), 12, 31);
        break;
    case CustomDate: // User selection
        date = QDate::fromString(ui->cbMaxNextStepDate->currentText(), QStringLiteral("MM/d/yyyy"));
        break;
    }
    return date;
}

int OpportunityFilterWidget::indexForOther() const
{
    return mCustomMaxNextStepDate.isValid() ? CustomDate + 1 : CustomDate;
}

void OpportunityFilterWidget::setFilterSettings(const OpportunityFilterSettings &filterSettings)
{
    m_filterSettings = filterSettings;
}


void OpportunityFilterWidget::filterChanged()
{
    OpportunityFilterSettings filterSettings;
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
        if (idx == ui->cbCountry->count() - 1) // "other"
            countries << OpportunityFilterSettings::otherCountriesSpecialValue();
        else if (idx != -1) {
            countries = ClientSettings::self()->countryFilters().groups().at(idx).entries;
        }
    }
    filterSettings.setAssignees(assignees, ui->cbAssignee->currentText());
    filterSettings.setCountries(countries, ui->cbCountry->currentText());
    filterSettings.setShowOpenClosed(ui->cbOpen->isChecked(), ui->cbClosed->isChecked());
    filterSettings.setModifiedAfter(ui->modifiedAfter->date());
    filterSettings.setModifiedBefore(ui->modifiedBefore->date());
    filterSettings.setCustomMaxDate(mCustomMaxNextStepDate);
    filterSettings.setShownPriority(ui->cbPriority->currentText());

    if (ui->cbMaxNextStepDate->currentIndex() == indexForOther()) {
        auto *calendar = new QCalendarWidget();
        calendar->setWindowModality(Qt::ApplicationModal);
        calendar->setWindowFlags(Qt::Popup);
        calendar->setAttribute(Qt::WA_DeleteOnClose);
        calendar->setWindowTitle(i18n("Custom Date"));
        calendar->show();
        calendar->move(ui->cbMaxNextStepDate->pos().x(), (ui->cbMaxNextStepDate->pos().y() + calendar->rect().height()));
        connect(calendar, SIGNAL(clicked(QDate)), this, SLOT(slotSetMaxNextStepCustomDate(QDate)));
        connect(calendar, SIGNAL(destroyed()), this, SLOT(slotResetMaxNextStepDateIndex()));
    }
    filterSettings.setMaxDate(maxNextStepDate(), ui->cbMaxNextStepDate->currentIndex());
    m_oppFilterProxyModel->setFilter(filterSettings);

    emit filterUpdated(filterSettings);
    setFilterSettings(filterSettings);
}

void OpportunityFilterWidget::slotResetMaxNextStepDateIndex()
{
    // In case the calendar was closed without any date being selected
    ui->cbMaxNextStepDate->setCurrentIndex(NoDate);
}

void OpportunityFilterWidget::slotSetMaxNextStepCustomDate(const QDate &date)
{
    auto *calendar = qobject_cast<QCalendarWidget*>(sender());
    if (!calendar)
        return;
    disconnect(calendar, SIGNAL(destroyed()), this, SLOT(slotResetMaxNextStepDateIndex()));
    if (mCustomMaxNextStepDate.isValid())
        ui->cbMaxNextStepDate->removeItem(CustomDate); // existing custom date will be replaced
    mCustomMaxNextStepDate = date;
    ui->cbMaxNextStepDate->insertItem(CustomDate, mCustomMaxNextStepDate.toString(QStringLiteral("MM/d/yyyy")));
    ui->cbMaxNextStepDate->setCurrentIndex(CustomDate);
    calendar->close(); // Qt::WA_DeleteOnClose
    filterChanged();
}

void OpportunityFilterWidget::setSearchPrefix(const QString &searchPrefix)
{
    mSearchPrefix = searchPrefix;
}

void OpportunityFilterWidget::setSearchName(const QString &searchName)
{
    mSearchName = searchName;
}

void OpportunityFilterWidget::setSearchText(const QString &searchText)
{
    mSearchText = searchText;
}

void OpportunityFilterWidget::saveSearch()
{
    filterChanged();

    // Add values from filterWidget
    m_filterSettings.setSearchName(mSearchName);
    m_filterSettings.setSearchText(mSearchText);
    // Save as a saved search
    ClientSettings::self()->saveSearch(m_filterSettings, mSearchPrefix);
}

void OpportunityFilterWidget::loadSearch(const QString &searchPrefix)
{
    OpportunityFilterSettings settings;
    ClientSettings::self()->loadSavedSearch(settings, searchPrefix);
    setupFromConfig(settings);

    setSearchName(settings.searchName());
    setSearchText(settings.searchText());
}
