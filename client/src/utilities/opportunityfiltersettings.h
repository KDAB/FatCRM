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

#ifndef OPPORTUNITYFILTERSETTINGS_H
#define OPPORTUNITYFILTERSETTINGS_H

#include <QStringList>
#include <QDate>
class QSettings;

/**
 * Filter oportunities by assignees OR by country.
 * And with a possible max date.
 * And either open, or closed, or both.
 * And a possible range of "modified date".
 */
class OpportunityFilterSettings
{
public:
    OpportunityFilterSettings();
    // default copy ctor and operator= are wanted.

    void setAssignees(const QStringList &assignees, const QString &assigneeGroup);
    QStringList assignees() const { return mAssignees; }
    QString assigneeGroup() const { return mAssigneeGroup; }
    void setCountries(const QStringList &countries, const QString &countryGroup);
    QStringList countries() const { return mCountries; }
    QString countryGroup() const { return mCountryGroup; }
    void setMaxDate(const QDate &maxDate, int comboIndex);
    QDate maxDate() const { return mMaxDate; }
    int maxDateIndex() const { return mMaxDateIndex; }
    void setCustomMaxDate(const QDate &customMaxDate);
    QDate customMaxDate() const { return mCustomMaxDate; }
    void setModifiedAfter(const QDate &modifiedAfter);
    QDate modifiedAfter() const { return mModifiedAfter; }
    void setModifiedBefore(const QDate &modifiedBefore);
    QDate modifiedBefore() const { return mModifiedBefore; }
    void setShowOpenClosed(bool showOpen, bool showClosed);
    bool showOpen() const { return mShowOpen; }
    bool showClosed() const { return mShowClosed; }
    void setShownPriority(const QString &shownPriority);
    QString shownPriority() const { return mShownPriority; }

    QString filterDescription() const;

    static QString otherCountriesSpecialValue() { return QLatin1String("_OTHER_"); }

    void save(QSettings &settings, const QString &prefix) const;
    void load(const QSettings &settings, const QString &prefix);

private:
    QStringList mAssignees; // no filtering if empty
    QStringList mCountries; // no filtering if empty
    QString mAssigneeGroup; // user-visible description for <assignee>
    QString mCountryGroup; // user-visible description for <countries>
    QString mShownPriority;
    QDate mMaxDate;
    QDate mCustomMaxDate;
    QDate mModifiedBefore, mModifiedAfter;
    int mMaxDateIndex = 0;
    bool mShowOpen = true;
    bool mShowClosed = false;
};

#endif // OPPORTUNITYFILTERSETTINGS_H
