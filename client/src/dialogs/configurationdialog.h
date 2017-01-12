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

#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>
#include <QMap>
#include "clientsettings.h"
class QModelIndex;
class QItemSelection;
class QListWidgetItem;

namespace Ui {
class ConfigurationDialog;
}

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDialog(QWidget *parent = nullptr);
    ~ConfigurationDialog() override;

public Q_SLOTS:
    void accept() override;

private Q_SLOTS:
    void slotCurrentAssigneeChanged(const QModelIndex &idx);
    void slotAssigneeGroupChanged(QListWidgetItem *);
    void slotRemoveAssignee();
    void slotAddAssignee();
    void slotEditAssigneeGroup();

    void slotCurrentCountryChanged(const QModelIndex &idx);
    void slotCountryGroupChanged(QListWidgetItem *);
    void slotRemoveCountry();
    void slotAddCountry();
    void slotEditCountryGroup();

private:
    QString fullUserName() const;
    ClientSettings::GroupFilters assigneeFilters() const;
    ClientSettings::GroupFilters countryFilters() const;
    void addCountryItem(const QString &name);
    void addAssigneeItem(const QString &name);

    Ui::ConfigurationDialog *ui;
    ClientSettings::GroupFilters m_assigneeFilters;
    ClientSettings::GroupFilters m_countryFilters;
};

#endif // CONFIGURATIONDIALOG_H
