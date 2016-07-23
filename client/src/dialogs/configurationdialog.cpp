/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "configurationdialog.h"
#include "editlistdialog.h"
#include "clientsettings.h"
#include "ui_configurationdialog.h"

#include <QInputDialog>
#include <QListView>
#include <QModelIndex>

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);

    connect(ui->countryListWidget->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotCurrentCountryChanged(QModelIndex)));
    connect(ui->removeCountryGroup, SIGNAL(clicked()),
            this, SLOT(slotRemoveCountry()));
    connect(ui->addCountryGroup, SIGNAL(clicked()),
            this, SLOT(slotAddCountry()));
    connect(ui->editSelectedCountryFilter, SIGNAL(clicked()),
            this, SLOT(slotEditCountryGroup()));
    connect(ui->countryListWidget, SIGNAL(itemChanged(QListWidgetItem*)),
            this, SLOT(slotCountryGroupChanged(QListWidgetItem*)));
    ui->editSelectedCountryFilter->setEnabled(false);
    ui->removeCountryGroup->setEnabled(false);

    connect(ui->assigneeListWidget->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotCurrentAssigneeChanged(QModelIndex)));
    connect(ui->removeAssigneeGroup, SIGNAL(clicked()),
            this, SLOT(slotRemoveAssignee()));
    connect(ui->addAssigneeGroup, SIGNAL(clicked()),
            this, SLOT(slotAddAssignee()));
    connect(ui->editSelectedAssigneeFilter, SIGNAL(clicked()),
            this, SLOT(slotEditAssigneeGroup()));
    connect(ui->assigneeListWidget, SIGNAL(itemChanged(QListWidgetItem*)),
            this, SLOT(slotAssigneeGroupChanged(QListWidgetItem*)));
    ui->editSelectedAssigneeFilter->setEnabled(false);
    ui->removeAssigneeGroup->setEnabled(false);

    ClientSettings *settings = ClientSettings::self();
    ui->fullName->setText(settings->fullUserName());
    m_assigneeFilters = settings->assigneeFilters();
    foreach (const QString &groupName, m_assigneeFilters.groupNames()) {
        addAssigneeItem(groupName);
    }
    m_countryFilters = settings->countryFilters();
    foreach (const QString &groupName, m_countryFilters.groupNames()) {
        addCountryItem(groupName);
    }
    ui->cbShowToolTips->setChecked(settings->showToolTips());

    ClientSettings::self()->restoreWindowSize("configurationdialog", this);
}

ConfigurationDialog::~ConfigurationDialog()
{
    ClientSettings::self()->saveWindowSize("configurationdialog", this);
    delete ui;
}

QString ConfigurationDialog::fullUserName() const
{
    return ui->fullName->text();
}

ClientSettings::GroupFilters ConfigurationDialog::assigneeFilters() const
{
    return m_assigneeFilters;
}

ClientSettings::GroupFilters ConfigurationDialog::countryFilters() const
{
    return m_countryFilters;
}

void ConfigurationDialog::addAssigneeItem(const QString &name)
{
    QListWidgetItem *item = new QListWidgetItem(name, ui->assigneeListWidget);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
}

void ConfigurationDialog::addCountryItem(const QString &name)
{
    QListWidgetItem *item = new QListWidgetItem(name, ui->countryListWidget);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
}

void ConfigurationDialog::accept()
{
    ClientSettings *settings = ClientSettings::self();
    settings->setFullUserName(fullUserName());
    settings->setAssigneeFilters(assigneeFilters());
    settings->setCountryFilters(countryFilters());
    settings->setShowToolTips(ui->cbShowToolTips->isChecked());
    settings->sync();
    QDialog::accept();
}

void ConfigurationDialog::slotCurrentAssigneeChanged(const QModelIndex &idx)
{
    ui->editSelectedAssigneeFilter->setEnabled(idx.isValid());
    ui->removeAssigneeGroup->setEnabled(idx.isValid());
}

void ConfigurationDialog::slotAssigneeGroupChanged(QListWidgetItem *item)
{
    const int row = ui->assigneeListWidget->row(item);
    m_assigneeFilters.renameGroup(row, item->text());
}

void ConfigurationDialog::slotRemoveAssignee()
{
    const int row = ui->assigneeListWidget->currentRow();
    delete ui->assigneeListWidget->currentItem();
    m_assigneeFilters.removeGroup(row);
}

void ConfigurationDialog::slotAddAssignee()
{
    const QString assignee = QInputDialog::getText(this, i18n("Add assignee group"), i18n("Assignee group name"));
    m_assigneeFilters.addGroup(assignee);
    addAssigneeItem(assignee);
}

void ConfigurationDialog::slotCurrentCountryChanged(const QModelIndex &idx)
{
    ui->editSelectedCountryFilter->setEnabled(idx.isValid());
    ui->removeCountryGroup->setEnabled(idx.isValid());
}

void ConfigurationDialog::slotCountryGroupChanged(QListWidgetItem *item)
{
    const int row = ui->countryListWidget->row(item);
    m_countryFilters.renameGroup(row, item->text());
}

void ConfigurationDialog::slotRemoveCountry()
{
    const int row = ui->countryListWidget->currentRow();
    delete ui->countryListWidget->currentItem();
    m_countryFilters.removeGroup(row);
}

void ConfigurationDialog::slotAddCountry()
{
    const QString country = QInputDialog::getText(this, i18n("Add country group"), i18n("Country group name"));
    m_countryFilters.addGroup(country);
    addCountryItem(country);
}

void ConfigurationDialog::slotEditAssigneeGroup()
{
    const int row = ui->assigneeListWidget->currentRow();
    EditListDialog dialog(i18n("Enter the full name of the users for the group, one name per line:"), this);
    ClientSettings::GroupFilters::Group group = m_assigneeFilters.groups().at(row);
    dialog.setWindowTitle(i18n("Editing assignee group %1", group.group));
    dialog.setItems(group.entries);
    if (dialog.exec()) {
        m_assigneeFilters.updateGroup(row, dialog.items());
    }
}

void ConfigurationDialog::slotEditCountryGroup()
{
    const int row = ui->countryListWidget->currentRow();
    EditListDialog dialog(i18n("Enter the names of the countries for the group, one country per line:"), this);
    ClientSettings::GroupFilters::Group group = m_countryFilters.groups().at(row);
    dialog.setWindowTitle(i18n("Editing country group %1", group.group));
    dialog.setItems(group.entries);
    if (dialog.exec()) {
        m_countryFilters.updateGroup(row, dialog.items());
    }
}
