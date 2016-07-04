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

#include "clientsettings.h"
#include "opportunityfiltersettings.h"

#include <KLocalizedString>
#include <KDebug>

#include <QWidget>
#include <QSettings>

Q_GLOBAL_STATIC(ClientSettings, s_self)

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
    return value.isValid() ? value.toBool() : false;
}

void ClientSettings::setShowToolTips(bool on)
{
    m_settings->setValue("showToolTips", on);
}

bool ClientSettings::showToolTips() const
{
    return m_settings->value("showToolTips", true).toBool();
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

    // Clean up after a previous bug
    const QStringList groups = ret.groupNames();
    const int idx = groups.indexOf(i18n("No country set"));
    if (idx >= 0)
        ret.removeGroup(idx);

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
    const QString str = m_settings->value("countryFilters").toString();

    if (str.isEmpty()) {
        // Initial set
        ClientSettings::GroupFilters::Group item;
        item.group = i18n("No country set");
        item.entries = QStringList() << QString();
        ret.addGroup(item);
    }
    ret.loadFromString(str);

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
    Q_FOREACH(const QString &itemStr, str.split('|')) {
        const QStringList lst = itemStr.split(';');
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

void ClientSettings::GroupFilters::addGroup(const ClientSettings::GroupFilters::Group &item)
{
    m_filters.append(item);
}

void ClientSettings::saveHeaderView(const QString &viewId, const QByteArray &headerViewData)
{
    //kDebug() << "writing" << ("header/"+viewId);
    m_settings->setValue("header/" + viewId, headerViewData);
}

QByteArray ClientSettings::restoreHeaderView(const QString &viewId) const
{
    //kDebug() << "reading" << ("header/"+viewId);
    return m_settings->value("header/" + viewId).toByteArray();
}

void ClientSettings::setFilterSettings(const OpportunityFilterSettings &settings)
{
    settings.save(*m_settings, "defaultOppFilter");
}

OpportunityFilterSettings ClientSettings::filterSettings() const
{
    OpportunityFilterSettings settings;
    settings.load(*m_settings, "defaultOppFilter");
    return settings;
}
