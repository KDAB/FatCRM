/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef OPPORTUNITYFILTERWIDGET_H
#define OPPORTUNITYFILTERWIDGET_H

#include <QDate>
#include <QWidget>
#include <QSettings>

#include "opportunityfiltersettings.h"
#include "clientsettings.h"

namespace Ui {
class OpportunityFilterWidget;
}
class OpportunityFilterProxyModel;

class OpportunityFilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OpportunityFilterWidget(OpportunityFilterProxyModel *oppFilterProxyModel, QWidget *parent = nullptr);
    ~OpportunityFilterWidget() override;

    void setSearchPrefix(const QString &searchPrefix);
    void setSearchName(const QString &searchName);
    QString searchName() const { return mSearchName; }
    void setSearchText(const QString &searchText);
    QString searchText() const { return mSearchText; }

    void saveSearch();
    void loadSearch(const QString &searchPrefix);

public Q_SLOTS:
signals:
    void filterUpdated(const OpportunityFilterSettings &settings);


private Q_SLOTS:
    void setupFromConfig(const OpportunityFilterSettings &settings = ClientSettings::self()->filterSettings());
    void filterChanged();
    void slotAssigneeSelected();
    void slotCountrySelected();
    void slotSetMaxNextStepCustomDate(const QDate &date);
    void slotResetMaxNextStepDateIndex();

private:
    QDate maxNextStepDate() const;
    int indexForOther() const;
    void setFilterSettings(const OpportunityFilterSettings &filterSettings);

    QDate mCustomMaxNextStepDate;
    Ui::OpportunityFilterWidget *ui;
    OpportunityFilterProxyModel *m_oppFilterProxyModel;
    OpportunityFilterSettings m_filterSettings;

    QString mSearchPrefix;
    QString mSearchName;
    QString mSearchText;
};

#endif // OPPORTUNITYFILTERWIDGET_H
