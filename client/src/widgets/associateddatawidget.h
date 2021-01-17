/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Michel Boyer de la Giroday <michel.giroday@kdab.com>

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

#ifndef ASSOCIATEDDATAWIDGET_H
#define ASSOCIATEDDATAWIDGET_H

#include <QWidget>

namespace Ui {
class AssociatedDataWidget;
}

class QModelIndex;
class QStringListModel;

class AssociatedDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AssociatedDataWidget(QWidget *parent = nullptr);
    ~AssociatedDataWidget() override;

    void hideOpportunityGui();
    void setContactsModel(QStringListModel *model);
    void setOpportunitiesModel(QStringListModel *model);

Q_SIGNALS:
    void openItem(const QString &item);

public Q_SLOTS:
    void editItem(const QModelIndex &index);

private:
    Ui::AssociatedDataWidget *mUi;
};

#endif // ASSOCIATEDDATAWIDGET_H
