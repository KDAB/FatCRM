/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

namespace Ui {
class ConfigurationDialog;
}

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDialog(QWidget *parent = 0);
    ~ConfigurationDialog();

    void setFullUserName(const QString &fullUserName);
    QString fullUserName() const;

    void setAssigneeFilters(const ClientSettings::GroupFilters &assigneeFilters);
    ClientSettings::GroupFilters assigneeFilters() const;

    void setCountryFilters(const ClientSettings::GroupFilters &countryFilters);
    ClientSettings::GroupFilters countryFilters() const;

private Q_SLOTS:
    void slotGroupListClicked(const QModelIndex &idx);
    void slotGroupRemoved(const QString &group);
    void slotGroupAdded(const QString &group);
    void slotCountryListClicked(const QModelIndex &idx);
    void slotCountryRemoved(const QString &country);
    void slotCountryAdded(const QString &country);
    void slotUsersChanged();
    void slotEditCountryGroup();

private:
    Ui::ConfigurationDialog *ui;
    ClientSettings::GroupFilters m_assigneeFilters;
    ClientSettings::GroupFilters m_countryFilters;
    int m_currentFilterRow;
    int m_currentCountryGroupRow;
};

#endif // CONFIGURATIONDIALOG_H
