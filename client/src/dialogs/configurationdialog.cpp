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
#include "ui_configurationdialog.h"
#include <QListView>
#include <QModelIndex>
#include "fatcrm_client_debug.h"

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog),
    m_currentAssigneeFilterRow(-1),
    m_currentCountryGroupRow(-1)
{
    ui->setupUi(this);

    connect(ui->groupListWidget->listView(), SIGNAL(clicked(QModelIndex)),
            this, SLOT(slotGroupListClicked(QModelIndex)));
    connect(ui->groupListWidget, SIGNAL(removed(QString)),
            this, SLOT(slotGroupRemoved(QString)));
    connect(ui->groupListWidget, SIGNAL(added(QString)),
            this, SLOT(slotGroupAdded(QString)));
    connect(ui->editSelectedAssigneeFilter, SIGNAL(clicked()),
            this, SLOT(slotEditAssigneeGroup()));
    ui->editSelectedAssigneeFilter->setEnabled(false);

    connect(ui->countryListWidget->listView(), SIGNAL(clicked(QModelIndex)),
            this, SLOT(slotCountryListClicked(QModelIndex)));
    connect(ui->countryListWidget, SIGNAL(removed(QString)),
            this, SLOT(slotCountryRemoved(QString)));
    connect(ui->countryListWidget, SIGNAL(added(QString)),
            this, SLOT(slotCountryAdded(QString)));
    connect(ui->editSelectedCountryFilter, SIGNAL(clicked()),
            this, SLOT(slotEditCountryGroup()));
    ui->editSelectedCountryFilter->setEnabled(false);
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}

void ConfigurationDialog::setFullUserName(const QString &fullUserName)
{
    ui->fullName->setText(fullUserName);
}

QString ConfigurationDialog::fullUserName() const
{
    return ui->fullName->text();
}

void ConfigurationDialog::setAssigneeFilters(const ClientSettings::GroupFilters &assigneeFilters)
{
    m_assigneeFilters = assigneeFilters;
    ui->groupListWidget->setItems(m_assigneeFilters.groupNames());
}

ClientSettings::GroupFilters ConfigurationDialog::assigneeFilters() const
{
    return m_assigneeFilters;
}

void ConfigurationDialog::setCountryFilters(const ClientSettings::GroupFilters &countryFilters)
{
    m_countryFilters = countryFilters;
    ui->countryListWidget->setItems(m_countryFilters.groupNames());
}

ClientSettings::GroupFilters ConfigurationDialog::countryFilters() const
{
    return m_countryFilters;
}

void ConfigurationDialog::slotGroupListClicked(const QModelIndex &idx)
{
    m_currentAssigneeFilterRow = idx.row();
    ui->editSelectedAssigneeFilter->setEnabled(true);
}

void ConfigurationDialog::slotGroupRemoved(const QString &group)
{
    Q_UNUSED(group);
    m_assigneeFilters.removeGroup(m_currentAssigneeFilterRow);
}

void ConfigurationDialog::slotGroupAdded(const QString &group)
{
    m_assigneeFilters.prependGroup(group);
}

void ConfigurationDialog::slotCountryListClicked(const QModelIndex &idx)
{
    m_currentCountryGroupRow = idx.row();
    ui->editSelectedCountryFilter->setEnabled(true);
}

void ConfigurationDialog::slotCountryRemoved(const QString &country)
{
    Q_UNUSED(country);
    m_countryFilters.removeGroup(m_currentCountryGroupRow);
}

void ConfigurationDialog::slotCountryAdded(const QString &country)
{
    m_countryFilters.prependGroup(country);
}

void ConfigurationDialog::slotEditAssigneeGroup()
{
    EditListDialog dialog(i18n("Type the full name of users to add them to the group:"), this);
    ClientSettings::GroupFilters::Group group = m_assigneeFilters.groups().at(m_currentAssigneeFilterRow);
    dialog.setWindowTitle(i18n("Editing assignee group %1", group.group));
    dialog.setItems(group.entries);
    if (dialog.exec()) {
        m_assigneeFilters.updateGroup(m_currentAssigneeFilterRow, dialog.items());
    }
}

void ConfigurationDialog::slotEditCountryGroup()
{
    EditListDialog dialog(i18n("Type country names to add them to the group:"), this);
    ClientSettings::GroupFilters::Group group = m_countryFilters.groups().at(m_currentCountryGroupRow);
    dialog.setWindowTitle(i18n("Editing country group %1", group.group));
    dialog.setItems(group.entries);
    if (dialog.exec()) {
        m_countryFilters.updateGroup(m_currentCountryGroupRow, dialog.items());
    }
}
