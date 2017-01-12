/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef CLIENTSETTINGS_H
#define CLIENTSETTINGS_H

#include <QObject>
class QSettings;
class OpportunityFilterSettings;
#include <QSize>
#include <QStringList>
#include <QVector>

/**
 * Settings for the FatCRM client application.
 * Saved in ~/.config/KDAB/FatCRM.conf
 */
class ClientSettings : public QObject
{
    Q_OBJECT
public:
    static ClientSettings *self();

    void sync();

    void setShowDetails(const QString &typeString, bool on);
    bool showDetails(const QString &typeString) const;

    void setShowToolTips(bool on);
    bool showToolTips() const;

    void setFullUserName(const QString &name);
    QString fullUserName() const;

    void saveWindowSize(const QString &windowId, QWidget *window);
    void restoreWindowSize(const QString &windowId, QWidget *window) const;

    void saveHeaderView(const QString &viewId, const QByteArray &headerViewData);
    QByteArray restoreHeaderView(const QString &viewId) const;

    void setFilterSettings(const OpportunityFilterSettings &settings);
    OpportunityFilterSettings filterSettings() const;

    class GroupFilters
    {
    public:
        struct Group {
            QString group;
            QStringList entries;
        };

        bool isEmpty() const { return m_filters.isEmpty(); }
        QString toString() const;
        void loadFromString(const QString &str);
        const QVector<Group>& groups() const { return m_filters; }
        void removeGroup(int row);
        void addGroup(const QString &group);
        void renameGroup(int row, const QString &group);
        void updateGroup(int row, const QStringList &users);
        QStringList groupNames() const;
        void addGroup(const Group &item);
    private:
        QVector<Group> m_filters;
    };
    void setAssigneeFilters(const GroupFilters &filters);
    GroupFilters assigneeFilters() const;
    void setCountryFilters(const GroupFilters &filters);
    GroupFilters countryFilters() const;

    ClientSettings();
    ~ClientSettings();

Q_SIGNALS:
    void assigneeFiltersChanged();
    void countryFiltersChanged();

private:
    QSettings *m_settings;
};

#endif // CLIENTSETTINGS_H
