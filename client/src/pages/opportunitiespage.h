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

#ifndef OPPORTUNITIESPAGE_H
#define OPPORTUNITIESPAGE_H

#include "page.h"

class OpportunityFilterWidget;
class OpportunityFilterProxyModel;
class OpportunityFilterSettings;
class OpportunityDataExtractor;

class OpportunitiesPage : public Page
{
    Q_OBJECT
public:
    explicit OpportunitiesPage(QWidget *parent = nullptr);

    ~OpportunitiesPage() override;

    void setSearchPrefix(const QString &searchPrefix);
    void setSearchName(const QString &searchName);
    void setSearchText(const QString &searchText);

    void saveSearch();
    void loadSearch(const QString &searchPrefix);

public slots:
    void createOpportunity(const QString &accountId);

protected:
    QMap<QString, QString> dataForNewObject() override;
    ItemDataExtractor *itemDataExtractor() const override;
    QString reportTitle() const override;
    void handleNewRows(int start, int end, bool emitChanges) override;
    void handleRemovedRows(int start, int end, bool initialLoadingDone) override;
    void handleItemChanged(const Akonadi::Item &item) override;

private:
    OpportunityFilterWidget *mFilterUiWidget;
    OpportunityFilterProxyModel *mOppFilterProxyModel;
    std::unique_ptr<OpportunityDataExtractor> mDataExtractor;

private slots:
    void slotDefaultOppFilterUpdated(const OpportunityFilterSettings &settings);
};

#endif /* OPPORTUNITIESPAGE_H */

