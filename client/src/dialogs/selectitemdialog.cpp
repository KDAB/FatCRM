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

#include "selectitemdialog.h"

#include "itemdataextractor.h"
#include "ui_selectitemdialog.h"

#include <QSortFilterProxyModel>

using namespace Akonadi;

SelectItemDialog::SelectItemDialog(DetailsType type, QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::SelectItemDialog),
    mDataExtractor(ItemDataExtractor::createDataExtractor(type))
{
   mUi->setupUi(this);
   mUi->treeView->setSortingEnabled(true);
   connect(mUi->treeView, SIGNAL(clicked(Akonadi::Item)), this, SLOT(slotItemSelected(Akonadi::Item)));
   connect(mUi->treeView, SIGNAL(activated(QModelIndex)), this, SLOT(accept()));
}

SelectItemDialog::~SelectItemDialog()
{
    delete mUi;
}

void SelectItemDialog::setModel(QAbstractItemModel *model)
{
    mUi->treeView->header()->setContextMenuPolicy(Qt::NoContextMenu);
    auto *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1); // all columns
    connect(mUi->searchLE, SIGNAL(textChanged(QString)), proxyModel, SLOT(setFilterFixedString(QString)));
    mUi->treeView->setModel(proxyModel);
}

void SelectItemDialog::slotItemSelected(const Akonadi::Item &item)
{
    mSelectedItem = item;
}

void SelectItemDialog::accept()
{
    Q_ASSERT(mDataExtractor);
    const QString id = mDataExtractor->idForItem(mSelectedItem);
    emit selectedItem(id);
    QDialog::accept();
}
