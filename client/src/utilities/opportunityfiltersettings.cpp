/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "opportunityfiltersettings.h"
#include "kdcrmdata/kdcrmutils.h"

#include <KLocalizedString>

#include <QSettings>
#include <QDebug>

OpportunityFilterSettings::OpportunityFilterSettings()
{
}

void OpportunityFilterSettings::setAssignees(const QStringList &assignees, const QString &assigneeGroup)
{
    mAssignees = assignees;
    mAssigneeGroup = assigneeGroup;
}

void OpportunityFilterSettings::setCountries(const QStringList &countries, const QString &countryGroup)
{
    mCountries = countries;
    mCountryGroup = countryGroup;
}

void OpportunityFilterSettings::setMaxDate(const QDate &maxDate, int comboIndex)
{
    mMaxDate = maxDate;
    mMaxDateIndex = MaxNextStepDate(comboIndex);
}

void OpportunityFilterSettings::setCustomMaxDate(const QDate &customMaxDate)
{
    mCustomMaxDate = customMaxDate;
}

void OpportunityFilterSettings::setModifiedAfter(const QDate &modifiedAfter)
{
    mModifiedAfter = modifiedAfter;
}

void OpportunityFilterSettings::setModifiedBefore(const QDate &modifiedBefore)
{
    mModifiedBefore = modifiedBefore;
}

void OpportunityFilterSettings::setShowOpenClosed(bool showOpen, bool showClosed)
{
    mShowOpen = showOpen;
    mShowClosed = showClosed;
}

void OpportunityFilterSettings::setShownPriority(const QString &shownPriority)
{
    mShownPriority = shownPriority;
}

void OpportunityFilterSettings::setSearchName(const QString &searchName)
{
    mSearchName = searchName;
}

void OpportunityFilterSettings::setSearchText(const QString &searchText)
{
    mSearchText = searchText;
}

QString OpportunityFilterSettings::filterDescription() const
{
    QString openOrClosed;
    if (!mShowOpen && mShowClosed)
        openOrClosed = i18n("closed");
    else if (mShowOpen && !mShowClosed)
        openOrClosed = i18n("open");

    QString txt;
    if (!mAssignees.isEmpty()) {
        txt = i18n("Assigned to %1", mAssigneeGroup);
    } else if (!mCountries.isEmpty()) {
        txt = i18n("In country %1", mCountryGroup);
    }

    if (!openOrClosed.isEmpty()) {
        txt = i18n("%1, %2", txt, openOrClosed);
    }

    if (!mMaxDate.isNull()) {
        txt = i18n("%1, next step before %2", txt, KDCRMUtils::formatDate(mMaxDate));
    }

    if (!mModifiedAfter.isNull()) {
        const QString after = KDCRMUtils::formatDate(mModifiedAfter);
        const QString before = KDCRMUtils::formatDate(mModifiedBefore);
        if (!mModifiedBefore.isNull()) {
            txt = i18n("%1, modified between %2 and %3", txt, after, before);
        } else {
            txt = i18n("%1, modified after %2", txt, after);
        }
    } else if (!mModifiedBefore.isNull()) {
        const QString before = KDCRMUtils::formatDate(mModifiedBefore);
        txt = i18n("%1, modified before %2", txt, before);
    }

    return txt;
}

void OpportunityFilterSettings::save(QSettings &settings, const QString &prefix) const
{
    // I'm not using settings.beginGroup(prefix) because
    // in the load method that would break const-ness...
    // KConfigGroup is missing in QSettings :)
    settings.setValue(prefix + "/assignees", mAssignees);
    settings.setValue(prefix + "/assigneeGroup", mAssigneeGroup);
    settings.setValue(prefix + "/countries", mCountries);
    settings.setValue(prefix + "/countryGroup", mCountryGroup);
    settings.setValue(prefix + "/customMaxDate", mCustomMaxDate);
    settings.setValue(prefix + "/maxDateIndex", maxNextStepDateToString(mMaxDateIndex));
    settings.setValue(prefix + "/modifiedBefore", mModifiedBefore);
    settings.setValue(prefix + "/modifiedAfter", mModifiedAfter);
    settings.setValue(prefix + "/showOpen", mShowOpen);
    settings.setValue(prefix + "/showClosed", mShowClosed);
    settings.setValue(prefix + "/shownPriority", mShownPriority);

    // Allow saving searches using this function
    settings.setValue(prefix + "/searchName", mSearchName);
    settings.setValue(prefix + "/searchText", mSearchText);
}

static MaxNextStepDate loadOldMaxNextDate(int oldIndex)
{
    // OneMonthAgo didn't exist back then
    switch (oldIndex) {
    case 0:
        return NoDate;
    case 1:
        return Today;
    case 2:
        return EndOfThisWeek;
    case 3:
        return EndOfThisMonth;
    case 4:
        return EndOfThisYear;
    case 5:
        return CustomDate;
    };
    return NoDate;
}

void OpportunityFilterSettings::load(const QSettings &settings, const QString &prefix)
{
    mAssignees = settings.value(prefix + "/assignees").toStringList();
    mAssigneeGroup = settings.value(prefix + "/assigneeGroup").toString();
    mCountries = settings.value(prefix + "/countries").toStringList();
    mCountryGroup = settings.value(prefix + "/countryGroup").toString();
    mCustomMaxDate = settings.value(prefix + "/customMaxDate").toDate();
    const QString maxDateStr = settings.value(prefix + "/maxDateIndex").toString();
    bool oldFormat;
    const int oldIndex = maxDateStr.toInt(&oldFormat);
    if (oldFormat) {
        mMaxDateIndex = loadOldMaxNextDate(oldIndex);
    } else {
        mMaxDateIndex = maxNextStepDateFromString(maxDateStr);
    }
    mModifiedBefore = settings.value(prefix + "/modifiedBefore").toDate();
    mModifiedAfter = settings.value(prefix + "/modifiedAfter").toDate();
    QVariant val = settings.value(prefix + "/showOpen");
    mShowOpen = val.isValid() ? val.toBool() : true;
    mShowClosed = settings.value(prefix + "/showClosed").toBool();
    mShownPriority = settings.value(prefix + "/shownPriority").toString();

    // Allow loading searches using this function
    mSearchName = settings.value(prefix + "/searchName").toString();
    mSearchText = settings.value(prefix + "/searchText").toString();
}

QVector<QString> OpportunityFilterSettings::savedSearches(const QSettings &settings)
{
    const QStringList allSettingsKeys = settings.allKeys();
    QVector<QString> savedSearches;
    for (const QString &key : allSettingsKeys) {
        if (key.startsWith("savedSearch-") && key.endsWith("/searchName")) {
            savedSearches.prepend(settings.value(key).toString());
        }
    }
    return savedSearches;
}

QString OpportunityFilterSettings::searchPrefixFromName(const QSettings &settings, const QString &name)
{
    const QStringList allSettingsKeys = settings.allKeys();
    for (const QString &key : allSettingsKeys) {
        if (key.startsWith("savedSearch-") && key.endsWith("/searchName")) {
            if (settings.value(key).toString() == name) {
                return key.split("/").at(0);
            }
        }
    }
    return QString();
}
