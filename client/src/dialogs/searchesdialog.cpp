/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Robin Jonsson <robin.jonsson@kdab.com>

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

#include "searchesdialog.h"
#include "ui_searchesdialog.h"

#include "clientsettings.h"
#include "opportunityfiltersettings.h"

#include <KLocalizedString>

SearchesDialog::SearchesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchesDialog)
{
    ui->setupUi(this);
    initialize();

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QWidget::close);
    connect(ui->searchesList, &QListWidget::itemSelectionChanged, this, &SearchesDialog::slotItemChanged);
    connect(ui->searchesList, &QListWidget::doubleClicked, this, &QDialog::accept);
}

SearchesDialog::~SearchesDialog()
{
    delete ui;
}

void SearchesDialog::initialize()
{
    ui->buttonLayout->setAlignment(Qt::AlignTop);
    populateListView();
    updateButtons();
}

void SearchesDialog::updateButtons()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(mSelectedItemName.isEmpty());
    ui->editSearch->setDisabled(mSelectedItemName.isEmpty());
    ui->removeSearch->setDisabled(mSelectedItemName.isEmpty());
}

void SearchesDialog::on_addSearch_clicked()
{
    mDialog = new AddSearchDialog(this, false, "", ClientSettings::self()->searchText(mSelectedItemName));
    mDialog->setWindowTitle(i18n("Add Search"));
    openAddSearchDialog();
}

void SearchesDialog::on_editSearch_clicked()
{
    QString selectedPrefix = ClientSettings::self()->searchPrefixFromName(mSelectedItemName);

    mDialog = new AddSearchDialog(this, true, mSelectedItemName, ClientSettings::self()->searchText(mSelectedItemName));
    mDialog->setWindowTitle(i18n("Edit ") + QString("\"%1\"").arg(mSelectedItemName));

    QList<QVariant> recentlyUsedSearches = ClientSettings::self()->recentlyUsedSearches();
    auto it = std::find_if(recentlyUsedSearches.begin(),
                           recentlyUsedSearches.end(),
                           [this](const QVariant &search){ return search.toString() == mSelectedItemName; });

    openAddSearchDialog();

    if (it != recentlyUsedSearches.end()) {
        recentlyUsedSearches.replace(std::distance(recentlyUsedSearches.begin(), it),
                         ClientSettings::self()->searchNameFromPrefix(selectedPrefix));

        ClientSettings::self()->setRecentlyUsedSearches(recentlyUsedSearches);
    }
}

void SearchesDialog::on_removeSearch_clicked()
{
    ClientSettings::self()->removeSearch(mSelectedItemName);
    populateListView();
}

void SearchesDialog::populateListView()
{
    ui->searchesList->clear();
    QVector<QString> savedSearches = ClientSettings::self()->savedSearches();
    qSort(savedSearches.begin(), savedSearches.end(), [](const QString &s1, const QString &s2){ return s1.toLower() < s2.toLower(); });
    for (const QString &search : savedSearches) {
        ui->searchesList->addItem(search);
    }
}

void SearchesDialog::slotItemChanged()
{
    if (ui->searchesList->selectedItems().isEmpty()) {
        mSelectedItemName = "";
    } else {
        mSelectedItemName = ui->searchesList->currentItem()->text();
    }

    updateButtons();
}

void SearchesDialog::openAddSearchDialog()
{
    mDialog->exec();
    populateListView();
}
