/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "associateddatawidget.h"
#include "ui_associateddatawidget.h"

#include <QStringListModel>

AssociatedDataWidget::AssociatedDataWidget(QWidget *parent) :
    QWidget(parent),
    mUi(new Ui::AssociatedDataWidget)
{
    mUi->setupUi(this);
    connect(mUi->opportunitiesListView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editItem(QModelIndex)));
    connect(mUi->opportunitiesListView, SIGNAL(returnPressed(QModelIndex)), this, SLOT(editItem(QModelIndex)));
    connect(mUi->contactsListView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editItem(QModelIndex)));
    connect(mUi->contactsListView, SIGNAL(returnPressed(QModelIndex)), this, SLOT(editItem(QModelIndex)));
}

AssociatedDataWidget::~AssociatedDataWidget()
{
    delete mUi;
}

void AssociatedDataWidget::hideOpportunityGui()
{
    mUi->opportunitiesLabel->hide();
    mUi->opportunitiesListView->hide();
}

void AssociatedDataWidget::setContactsModel(QStringListModel *model)
{
    mUi->contactsListView->setModel(model);
}

void AssociatedDataWidget::setOpportunitiesModel(QStringListModel *model)
{
    mUi->opportunitiesListView->setModel(model);
}

void AssociatedDataWidget::editItem(const QModelIndex &index)
{
    emit openItem(index.data(Qt::DisplayRole).toString());

}
