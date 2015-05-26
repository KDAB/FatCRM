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

#include "clientsettings.h"

#include <QWidget>
#include <QSettings>

Q_GLOBAL_STATIC(ClientSettings, s_self);

ClientSettings *ClientSettings::self()
{
    return s_self();
}

ClientSettings::ClientSettings()
    : m_settings(new QSettings("KDAB", "FatCRM"))
{
}

ClientSettings::~ClientSettings()
{
    delete m_settings;
}

void ClientSettings::sync()
{
    m_settings->sync();
}

void ClientSettings::setShowDetails(const QString &typeString, bool on)
{
    m_settings->setValue("showDetails/" + typeString, on);
}

bool ClientSettings::showDetails(const QString &typeString) const
{
    const QVariant value = m_settings->value("showDetails/" + typeString);
    return value.isValid() ? value.toBool() : true;
}

void ClientSettings::setFullUserName(const QString &name)
{
    m_settings->setValue("fullUserName", name);
}

QString ClientSettings::fullUserName() const
{
    return m_settings->value("fullUserName").toString();
}

void ClientSettings::saveWindowSize(const QString &windowId, QWidget *window)
{
    m_settings->setValue("windowSize/" + windowId, window->size());
}

void ClientSettings::restoreWindowSize(const QString &windowId, QWidget *window) const
{
    const QSize sz = m_settings->value("windowSize/" + windowId).toSize();
    if (sz.isValid()) {
        window->resize(sz);
    }
}

void ClientSettings::setAssigneeFilters(const ClientSettings::GroupFilters &filters)
{
    m_settings->setValue("assigneeFilters", filters.toString());
    emit assigneeFiltersChanged();
}

ClientSettings::GroupFilters ClientSettings::assigneeFilters() const
{
    ClientSettings::GroupFilters ret;
    ret.loadFromString(m_settings->value("assigneeFilters").toString());
    return ret;
}

void ClientSettings::setCountryFilters(const ClientSettings::GroupFilters &filters)
{
    m_settings->setValue("countryFilters", filters.toString());
    emit countryFiltersChanged();
}

ClientSettings::GroupFilters ClientSettings::countryFilters() const
{
    ClientSettings::GroupFilters ret;
    ret.loadFromString(m_settings->value("countryFilters").toString());
    return ret;
}

QString ClientSettings::GroupFilters::toString() const
{
    QString ret;
    for (int i = 0 ; i < m_filters.count() ; ++i) {
        const Group& item = m_filters.at(i);
        ret += item.group + ';';
        ret += item.entries.join(QString(QLatin1Char(';')));
        if (i + 1 < m_filters.count())
            ret += '|';
    }
    return ret;
}

void ClientSettings::GroupFilters::loadFromString(const QString &str)
{
    Q_FOREACH(const QString &itemStr, str.split('|', QString::SkipEmptyParts)) {
        const QStringList lst = itemStr.split(';', QString::SkipEmptyParts);
        if (lst.isEmpty()) {
            continue;
        }
        Group item;
        item.group = lst.first();
        item.entries = lst.mid(1);
        m_filters.append(item);
    }
}

void ClientSettings::GroupFilters::removeGroup(int row)
{
    m_filters.remove(row);
}

void ClientSettings::GroupFilters::prependGroup(const QString &group)
{
    Group item;
    item.group = group;
    m_filters.prepend(item);
}

void ClientSettings::GroupFilters::updateGroup(int row, const QStringList &users)
{
    m_filters[row].entries = users;
}

QStringList ClientSettings::GroupFilters::groupNames() const
{
    QStringList ret;
    Q_FOREACH(const Group &item, m_filters) {
        ret.append(item.group);
    }
    return ret;
}


void ClientSettings::setVisibleColumns(const QString &viewId, const QStringList &names)
{
    m_settings->setValue("columns/" + viewId, names);
}

QStringList ClientSettings::visibleColumns(const QString &viewId, const QStringList &defaultColumns) const
{
    return m_settings->value("columns/" + viewId, defaultColumns).toStringList();
}
