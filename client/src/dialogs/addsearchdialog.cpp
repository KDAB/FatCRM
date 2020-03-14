/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author(s): Robin Jonsson <robin.jonsson@kdab.com>

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

#include "addsearchdialog.h"
#include "ui_addsearchdialog.h"

#include "searchesdialog.h"
#include "clientsettings.h"
#include "opportunitiespage.h"
#include "opportunityfiltersettings.h"
#include "opportunityfilterwidget.h"
#include "opportunityfilterproxymodel.h"

#include <QFile>
#include <QPushButton>
#include <QMessageBox>

#include <KLocalizedString>

AddSearchDialog::AddSearchDialog(QWidget *parent, const bool searchBeingEdited, const QString searchName, const QString filterString) :
    QDialog(parent),
    ui(new Ui::AddSearchDialog)
{
    ui->setupUi(this);

    // load UI from opportunityfilterwidget
    mOppFilterProxyModel = new OpportunityFilterProxyModel(this);
    mFilterUiWidget = new OpportunityFilterWidget(mOppFilterProxyModel, this);

    ui->verticalLayout->insertWidget(1, mFilterUiWidget);

    initialize(searchBeingEdited, searchName, filterString);

    connect(ui->searchName, &QLineEdit::textChanged, this, &AddSearchDialog::slotUpdateSearchName);
}

AddSearchDialog::~AddSearchDialog()
{
    delete ui;
}

void AddSearchDialog::initialize(const bool searchBeingEdited, const QString &searchName, const QString &filterString)
{
    if (searchBeingEdited) {
        const QString prefixBeingEdited = ClientSettings::self()->searchPrefixFromName(searchName);
        mFilterUiWidget->setSearchPrefix(prefixBeingEdited);
        mFilterUiWidget->loadSearch(prefixBeingEdited);

        ui->searchName->setText(mFilterUiWidget->searchName());
        ui->searchText->setText(mFilterUiWidget->searchText());
    } else {
        // No search being edited so prepare new prefix
        mFilterUiWidget->setSearchPrefix(searchPrefix());
        ui->searchText->setText(filterString);
    }

    // Needed when the edit button is pressed in SearchesDialog
    mSearchName = ui->searchName->text();
    mEditingSearch = searchBeingEdited;

    updateSaveButton();
}

QString AddSearchDialog::searchPrefix() const
{
    const QString newSearchIndex = QString::number(ClientSettings::self()->searchIndex() + 1);
    return "savedSearch-" + newSearchIndex;
}

void AddSearchDialog::slotUpdateSearchName(const QString &searchName)
{
    mSearchName = searchName;
    updateSaveButton();
}

void AddSearchDialog::updateSaveButton()
{
    ui->buttonBox->button(QDialogButtonBox::Save)->setDisabled(mSearchName.isEmpty());
}

void AddSearchDialog::saveSearch()
{
    // Send values to opportunityFilterWidget
    mFilterUiWidget->setSearchName(ui->searchName->text());
    mFilterUiWidget->setSearchText(ui->searchText->text());
    // Save search with added values
    mFilterUiWidget->saveSearch();
}

void AddSearchDialog::accept()
{
    // Check for name clash
    QVector<QString> savedSearches = ClientSettings::self()->savedSearches();
    if (savedSearches.contains(mSearchName) && !mEditingSearch) {
        int msgBox = QMessageBox::question(this,
                                           i18n("Search exists!"),
                                           i18n("Do you want to overwrite \"%1\"?").arg(mSearchName),
                                           QMessageBox::Yes, QMessageBox::No);

        if (msgBox != QMessageBox::Yes) {
            return;
        }
        // Overwriting search, so get searchPrefix and replace that search with the new one.
        mFilterUiWidget->setSearchPrefix(ClientSettings::self()->searchPrefixFromName(mSearchName));
    }
    saveSearch();
    QDialog::accept();
}
