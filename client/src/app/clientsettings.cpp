/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <algorithm>

#include <KLocalizedString>

#include <QWidget>
#include <QSettings>

Q_GLOBAL_STATIC(ClientSettings, s_self)

ClientSettings *ClientSettings::self()
{
    return s_self();
}

ClientSettings::ClientSettings()
    : m_settings(new QSettings(QStringLiteral("KDAB"), QStringLiteral("FatCRM")))
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
    m_settings->setValue(QStringLiteral("fullUserName"), name);
}

QString ClientSettings::fullUserName() const
{
    return m_settings->value(QStringLiteral("fullUserName")).toString();
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
    m_settings->setValue(QStringLiteral("assigneeFilters"), filters.toString());
    emit assigneeFiltersChanged();
}

ClientSettings::GroupFilters ClientSettings::assigneeFilters() const
{
    ClientSettings::GroupFilters ret;
    ret.loadFromString(m_settings->value(QStringLiteral("assigneeFilters")).toString());

    // Clean up after a previous bug
    const QStringList groups = ret.groupNames();
    const int idx = groups.indexOf(i18n("No country set"));
    if (idx >= 0)
        ret.removeGroup(idx);

    return ret;
}

void ClientSettings::setCountryFilters(const ClientSettings::GroupFilters &filters)
{
    m_settings->setValue(QStringLiteral("countryFilters"), filters.toString());
    emit countryFiltersChanged();
}

ClientSettings::GroupFilters ClientSettings::countryFilters() const
{
    ClientSettings::GroupFilters ret;
    const QString str = m_settings->value(QStringLiteral("countryFilters")).toString();

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

QString ClientSettings::defaultResourceId() const
{
    return m_settings->value(QStringLiteral("defaultResourceId"), QString()).toString();
}

void ClientSettings::setDefaultResourceId(const QString &id)
{
    m_settings->setValue(QStringLiteral("defaultResourceId"), id);
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
    if (str.isEmpty())
        return;

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

void ClientSettings::GroupFilters::addGroup(const QString &group)
{
    Group item;
    item.group = group;
    m_filters.append(item);
}

void ClientSettings::GroupFilters::renameGroup(int row, const QString &group)
{
    m_filters[row].group = group;
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
    //qCDebug(FATCRM_CLIENT_LOG) << "writing" << ("header/"+viewId);
    m_settings->setValue("header/" + viewId, headerViewData);
}

QByteArray ClientSettings::restoreHeaderView(const QString &viewId) const
{
    //qCDebug(FATCRM_CLIENT_LOG) << "reading" << ("header/"+viewId);
    return m_settings->value("header/" + viewId).toByteArray();
}

void ClientSettings::setFilterSettings(const OpportunityFilterSettings &settings)
{
    settings.save(*m_settings, QStringLiteral("defaultOppFilter"));
}

OpportunityFilterSettings ClientSettings::filterSettings() const
{
    OpportunityFilterSettings settings;
    settings.load(*m_settings, QStringLiteral("defaultOppFilter"));
    return settings;
}

void ClientSettings::saveSearch(const OpportunityFilterSettings &settings, QString prefix)
{
    const QVector<QString> savedSearches = self()->savedSearches();
    if (std::none_of(savedSearches.begin(),
                     savedSearches.end(),
                     [this, prefix](const QString &selectedSearchName){ return (searchPrefixFromName(selectedSearchName) == prefix); })) {
        // Prefix does not exist so create a new search
        incrementSearchIndex();
        prefix = "savedSearch-" + QString::number(searchIndex());
    }
    settings.save(*m_settings, prefix);

    ClientSettings::self()->addRecentlyUsedSearch(settings.searchName());
}

void ClientSettings::loadSavedSearch(OpportunityFilterSettings &settings, const QString &prefix)
{
    settings.load(*m_settings, prefix);
}

const QSettings &ClientSettings::settings() const
{
    return *m_settings;
}

QSettings &ClientSettings::settings()
{
    return *m_settings;
}

QVector<QString> ClientSettings::savedSearches() const
{
    return OpportunityFilterSettings::savedSearches(*m_settings);
}

QString ClientSettings::searchPrefixFromName(const QString &name) const
{
    return OpportunityFilterSettings::searchPrefixFromName(*m_settings, name);
}

QString ClientSettings::searchNameFromPrefix(const QString &prefix) const
{
    return m_settings->value(prefix + "/searchName").toString();
}

void ClientSettings::incrementSearchIndex()
{
    m_settings->setValue("savedSearches/searchesIndex", QString::number(searchIndex() + 1));
}

int ClientSettings::searchIndex() const
{
    return m_settings->value("savedSearches/searchesIndex").toInt();
}

void ClientSettings::removeSearch(const QString &searchName)
{
    m_settings->remove(searchPrefixFromName(searchName));

    QStringList recentlyUsedSearches = this->recentlyUsedSearches();
    auto it = std::find(recentlyUsedSearches.begin(),
                        recentlyUsedSearches.end(),
                        searchName);
    if (it != recentlyUsedSearches.end()) {
        recentlyUsedSearches.erase(it);
    }
    setRecentlyUsedSearches(recentlyUsedSearches);
}

void ClientSettings::setRecentlyUsedSearches(const QStringList &useOrder)
{
    m_settings->setValue("savedSearches/useOrder", useOrder);
    emit recentSearchesUpdated();
}

void ClientSettings::addRecentlyUsedSearch(const QString &searchName)
{
    QStringList recentlyUsedSearches = this->recentlyUsedSearches();
    auto it = std::find(recentlyUsedSearches.begin(),
                        recentlyUsedSearches.end(),
                        searchName);
    // Remove if it exists
    if (it != recentlyUsedSearches.end()) {
        recentlyUsedSearches.erase(it);
    }
    // And then prepend
    recentlyUsedSearches.prepend(searchName);
    setRecentlyUsedSearches(recentlyUsedSearches);
}

QString ClientSettings::searchText(const QString &searchName) const
{
    const QString prefix = searchPrefixFromName(searchName);
    return m_settings->value(prefix + "/searchText").toString();
}

QStringList ClientSettings::recentlyUsedSearches() const
{
    return m_settings->value("savedSearches/useOrder").toStringList();
}
