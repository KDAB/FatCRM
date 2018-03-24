/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef OPPORTUNITYDETAILS_H
#define OPPORTUNITYDETAILS_H

#include "details.h"

#include <QDate>

namespace Ui
{
class OpportunityDetails;
}

class LinkedItemsRepository;
class OpportunityDataExtractor;

class OpportunityDetails : public Details
{
    Q_OBJECT
public:
    explicit OpportunityDetails(QWidget *parent = nullptr);

    ~OpportunityDetails() override;

    void setLinkedItemsRepository(LinkedItemsRepository *repo) override;
    ItemDataExtractor *itemDataExtractor() const override;
    void setItemsTreeModel(ItemsTreeModel *model) override;

private Q_SLOTS:
    void slotAutoNextStepDate();
    void slotSalesStageActivated(const QString &stage);
    void slotSelectAccount();
    void slotAccountSelected(const QString &accountId);
    void slotCloseDateChanged(const QDate &date);
    void slotLinkedItemsModified(const QString &oppId);

    void on_viewNotesButton_clicked();
    void on_viewDocumentsButton_clicked();
    void on_buttonOpenAccount_clicked();

private:
    void initialize();
    QMap<QString, QString> data(const Akonadi::Item &item) const override;
    void updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const override;
    void setDataInternal(const QMap<QString, QString> &data) override;
    void updateCloseDateLabel(bool closed);

private:
    Ui::OpportunityDetails *mUi;
    LinkedItemsRepository *mLinkedItemsRepository;
    OpportunityDataExtractor *mDataExtractor;
    QDate mOriginalCloseDate;
    bool mCloseDateChangedByUser = false;
    void updateLinkedItemsButtons();
};

#endif /* OPPORTUNITYDETAILS_H */

