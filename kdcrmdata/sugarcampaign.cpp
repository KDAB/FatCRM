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

#include "sugarcampaign.h"
#include "kdcrmfields.h"

#include <KLocale>

#include <QHash>
#include <QMap>
#include <QSharedData>
#include <QString>

class SugarCampaign::Private : public QSharedData
{
public:
    Private()
        : mEmpty(true)
    {
    }

    bool mEmpty;

    QString mId;
    QString mName;
    QString mDateEntered;
    QString mDateModified;
    QString mModifiedUserId;
    QString mModifiedByName;
    QString mCreatedBy;
    QString mCreatedByName;
    QString mDeleted;
    QString mAssignedUserId;
    QString mAssignedUserName;
    QString mTrackerKey;
    QString mTrackerCount;
    QString mReferUrl;
    QString mTrackerText;
    QString mStartDate;
    QString mEndDate;
    QString mStatus;
    QString mImpressions;
    QString mCurrencyId;
    QString mBudget;
    QString mExpectedCost;
    QString mActualCost;
    QString mExpectedRevenue;
    QString mCampaignType;
    QString mObjective;
    QString mContent;
    QString mFrequency;
};

SugarCampaign::SugarCampaign()
    : d(new Private)
{
}

SugarCampaign::SugarCampaign(const SugarCampaign &other)
    : d(other.d)
{
}

SugarCampaign::~SugarCampaign()
{
}

SugarCampaign &SugarCampaign::operator=(const SugarCampaign &other)
{
    if (this != &other) {
        d = other.d;
    }

    return *this;
}

bool SugarCampaign::isEmpty() const
{
    return d->mEmpty;
}

void SugarCampaign::clear()
{
    *this = SugarCampaign();
}

void SugarCampaign::setId(const QString &id)
{
    d->mEmpty = false;
    d->mId = id;
}

QString SugarCampaign::id() const
{
    return d->mId;
}

void SugarCampaign::setName(const QString &name)
{
    d->mEmpty = false;
    d->mName = name;
}

QString SugarCampaign::name() const
{
    return d->mName;
}

void SugarCampaign::setDateEntered(const QString &value)
{
    d->mEmpty = false;
    d->mDateEntered = value;
}

QString SugarCampaign::dateEntered() const
{
    return d->mDateEntered;
}

void SugarCampaign::setDateModified(const QString &value)
{
    d->mEmpty = false;
    d->mDateModified = value;
}

QString SugarCampaign::dateModified() const
{
    return d->mDateModified;
}

void SugarCampaign::setModifiedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedUserId = value;
}

QString SugarCampaign::modifiedUserId() const
{
    return d->mModifiedUserId;
}

void SugarCampaign::setModifiedByName(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedByName = value;
}

QString SugarCampaign::modifiedByName() const
{
    return d->mModifiedByName;
}

void SugarCampaign::setCreatedBy(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedBy = value;
}

QString SugarCampaign::createdBy() const
{
    return d->mCreatedBy;
}

void SugarCampaign::setCreatedByName(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedByName = value;
}

QString SugarCampaign::createdByName() const
{
    return d->mCreatedByName;
}

void SugarCampaign::setDeleted(const QString &value)
{
    d->mEmpty = false;
    d->mDeleted = value;
}

QString SugarCampaign::deleted() const
{
    return d->mDeleted;
}

void SugarCampaign::setAssignedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserId = value;
}

QString SugarCampaign::assignedUserId() const
{
    return d->mAssignedUserId;
}

void SugarCampaign::setAssignedUserName(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserName = value;
}

QString SugarCampaign::assignedUserName() const
{
    return d->mAssignedUserName;
}

void SugarCampaign::setTrackerKey(const QString &value)
{
    d->mEmpty = false;
    d->mTrackerKey = value;
}

QString SugarCampaign::trackerKey() const
{
    return d->mTrackerKey;
}

void SugarCampaign::setTrackerCount(const QString &value)
{
    d->mEmpty = false;
    d->mTrackerCount = value;
}

QString SugarCampaign::trackerCount() const
{
    return d->mTrackerCount;
}

void SugarCampaign::setReferUrl(const QString &value)
{
    d->mEmpty = false;
    d->mReferUrl = value;
}

QString SugarCampaign::referUrl() const
{
    return d->mReferUrl;
}

void SugarCampaign::setTrackerText(const QString &value)
{
    d->mEmpty = false;
    d->mTrackerText = value;
}

QString SugarCampaign::trackerText() const
{
    return d->mTrackerText;
}

void SugarCampaign::setStartDate(const QString &value)
{
    d->mEmpty = false;
    d->mStartDate = value;
}

QString SugarCampaign::startDate() const
{
    return d->mStartDate;
}

void SugarCampaign::setEndDate(const QString &value)
{
    d->mEmpty = false;
    d->mEndDate = value;
}

QString SugarCampaign::endDate() const
{
    return d->mEndDate;
}

void SugarCampaign::setStatus(const QString &value)
{
    d->mEmpty = false;
    d->mStatus = value;
}

QString SugarCampaign::status() const
{
    return d->mStatus;
}

void SugarCampaign::setImpressions(const QString &value)
{
    d->mEmpty = false;
    d->mImpressions = value;
}

QString SugarCampaign::impressions() const
{
    return d->mImpressions;
}

void SugarCampaign::setCurrencyId(const QString &value)
{
    d->mEmpty = false;
    d->mCurrencyId = value;
}

QString SugarCampaign::currencyId() const
{
    return d->mCurrencyId;
}

void SugarCampaign::setBudget(const QString &value)
{
    d->mEmpty = false;
    d->mBudget = value;
}

QString SugarCampaign::budget() const
{
    return d->mBudget;
}

void SugarCampaign::setExpectedCost(const QString &value)
{
    d->mEmpty = false;
    d->mExpectedCost = value;
}

QString SugarCampaign::expectedCost() const
{
    return d->mExpectedCost;
}

void SugarCampaign::setActualCost(const QString &value)
{
    d->mEmpty = false;
    d->mActualCost = value;
}

QString SugarCampaign::actualCost() const
{
    return d->mActualCost;
}

void SugarCampaign::setExpectedRevenue(const QString &value)
{
    d->mEmpty = false;
    d->mExpectedRevenue = value;
}

QString SugarCampaign::expectedRevenue() const
{
    return d->mExpectedRevenue;
}

void SugarCampaign::setCampaignType(const QString &value)
{
    d->mEmpty = false;
    d->mCampaignType = value;
}

QString SugarCampaign::campaignType() const
{
    return d->mCampaignType;
}

void SugarCampaign::setObjective(const QString &value)
{
    d->mEmpty = false;
    d->mObjective = value;
}

QString SugarCampaign::objective() const
{
    return d->mObjective;
}

void SugarCampaign::setContent(const QString &value)
{
    d->mEmpty = false;
    d->mContent = value;
}

QString SugarCampaign::content() const
{
    return d->mContent;
}

void SugarCampaign::setFrequency(const QString &value)
{
    d->mEmpty = false;
    d->mFrequency = value;
}

QString SugarCampaign::frequency() const
{
    return d->mFrequency;
}

void SugarCampaign::setData(const QMap<QString, QString>& data)
{
    d->mEmpty = false;

    const SugarCampaign::AccessorHash accessors = SugarCampaign::accessorHash();
    QMap<QString, QString>::const_iterator it = data.constBegin();
    for ( ; it != data.constEnd() ; ++it) {
        const SugarCampaign::AccessorHash::const_iterator accessIt = accessors.constFind(it.key());
        if (accessIt != accessors.constEnd()) {
            (this->*(accessIt.value().setter))(it.value());
        } else {
            //TODO: add custom field support
            // d->mCustomFields.insert(it.key(), it.value());
        }
    }
}

QMap<QString, QString> SugarCampaign::data()
{
    QMap<QString, QString> data;

    const SugarCampaign::AccessorHash accessors = SugarCampaign::accessorHash();
    SugarCampaign::AccessorHash::const_iterator it    = accessors.constBegin();
    SugarCampaign::AccessorHash::const_iterator endIt = accessors.constEnd();
    for (; it != endIt; ++it) {
        const SugarCampaign::valueGetter getter = (*it).getter;
        data.insert(it.key(), (this->*getter)());
    }

/*TODO: add custom field support
    // plus custom fields
    QMap<QString, QString>::const_iterator cit = d->mCustomFields.constBegin();
    const QMap<QString, QString>::const_iterator end = d->mCustomFields.constEnd();
    for ( ; cit != end ; ++cit ) {
        data.insert(cit.key(), cit.value());
    }
*/

    return data;
}

QString SugarCampaign::mimeType()
{
    return QLatin1String("application/x-vnd.kdab.crm.campaign");
}

Q_GLOBAL_STATIC(SugarCampaign::AccessorHash, s_accessors)

SugarCampaign::AccessorHash SugarCampaign::accessorHash()
{
    AccessorHash &accessors = *s_accessors();
    if (accessors.isEmpty()) {
        accessors.insert(KDCRMFields::id(),
                         CampaignAccessorPair(&SugarCampaign::id, &SugarCampaign::setId, QString()));
        accessors.insert(KDCRMFields::name(),
                         CampaignAccessorPair(&SugarCampaign::name, &SugarCampaign::setName,
                                              i18nc("@item:intable campaign name", "Name")));
        accessors.insert(KDCRMFields::dateEntered(),
                         CampaignAccessorPair(&SugarCampaign::dateEntered, &SugarCampaign::setDateEntered, QString()));
        accessors.insert(KDCRMFields::dateModified(),
                         CampaignAccessorPair(&SugarCampaign::dateModified, &SugarCampaign::setDateModified, QString()));
        accessors.insert(KDCRMFields::modifiedUserId(),
                         CampaignAccessorPair(&SugarCampaign::modifiedUserId, &SugarCampaign::setModifiedUserId, QString()));
        accessors.insert(KDCRMFields::modifiedByName(),
                         CampaignAccessorPair(&SugarCampaign::modifiedByName, &SugarCampaign::setModifiedByName, QString()));
        accessors.insert(KDCRMFields::createdBy(),
                         CampaignAccessorPair(&SugarCampaign::createdBy, &SugarCampaign::setCreatedBy, QString()));
        accessors.insert(KDCRMFields::createdByName(),
                         CampaignAccessorPair(&SugarCampaign::createdByName, &SugarCampaign::setCreatedByName, QString()));
        accessors.insert(KDCRMFields::deleted(),
                         CampaignAccessorPair(&SugarCampaign::deleted, &SugarCampaign::setDeleted, QString()));
        accessors.insert(KDCRMFields::assignedUserId(),
                         CampaignAccessorPair(&SugarCampaign::assignedUserId, &SugarCampaign::setAssignedUserId, QString()));
        accessors.insert(KDCRMFields::assignedUserName(),
                         CampaignAccessorPair(&SugarCampaign::assignedUserName, &SugarCampaign::setAssignedUserName,
                                              i18nc("@item:intable", "Assigned To")));
        accessors.insert(KDCRMFields::trackerKey(),
                         CampaignAccessorPair(&SugarCampaign::trackerKey, &SugarCampaign::setTrackerKey, QString()));
        accessors.insert(KDCRMFields::trackerCount(),
                         CampaignAccessorPair(&SugarCampaign::trackerCount, &SugarCampaign::setTrackerCount, QString()));
        accessors.insert(KDCRMFields::referUrl(),
                         CampaignAccessorPair(&SugarCampaign::referUrl, &SugarCampaign::setReferUrl,
                                              i18nc("@item:intable", "Referer URL")));
        accessors.insert(KDCRMFields::trackerText(),
                         CampaignAccessorPair(&SugarCampaign::trackerText, &SugarCampaign::setTrackerText,
                                              i18nc("@item:intable", "Tracker")));
        accessors.insert(KDCRMFields::startDate(),
                         CampaignAccessorPair(&SugarCampaign::startDate, &SugarCampaign::setStartDate,
                                              i18nc("@item:intable", "Start Date")));
        accessors.insert(KDCRMFields::endDate(),
                         CampaignAccessorPair(&SugarCampaign::endDate, &SugarCampaign::setEndDate,
                                              i18nc("@item:intable", "End Date")));
        accessors.insert(KDCRMFields::status(),
                         CampaignAccessorPair(&SugarCampaign::status, &SugarCampaign::setStatus,
                                              i18nc("@item:intable", "Status")));
        accessors.insert(KDCRMFields::impressions(),
                         CampaignAccessorPair(&SugarCampaign::impressions, &SugarCampaign::setImpressions,
                                              i18nc("@item:intable", "Impressions")));
        accessors.insert(KDCRMFields::currencyId(),
                         CampaignAccessorPair(&SugarCampaign::currencyId, &SugarCampaign::setCurrencyId,
                                             i18nc("@item:intable", "Currency")));
        accessors.insert(KDCRMFields::budget(),
                         CampaignAccessorPair(&SugarCampaign::budget, &SugarCampaign::setBudget,
                                              i18nc("@item:intable", "Budget")));
        accessors.insert(KDCRMFields::expectedCost(),
                         CampaignAccessorPair(&SugarCampaign::expectedCost, &SugarCampaign::setExpectedCost,
                                              i18nc("@item:intable", "Expected Costs")));
        accessors.insert(KDCRMFields::actualCost(),
                         CampaignAccessorPair(&SugarCampaign::actualCost, &SugarCampaign::setActualCost,
                                              i18nc("@item:intable", "Actual Costs")));
        accessors.insert(KDCRMFields::expectedRevenue(),
                         CampaignAccessorPair(&SugarCampaign::expectedRevenue, &SugarCampaign::setExpectedRevenue,
                                              i18nc("@item:intable", "Expected Revenue")));
        accessors.insert(KDCRMFields::campaignType(),
                         CampaignAccessorPair(&SugarCampaign::campaignType, &SugarCampaign::setCampaignType,
                                              i18nc("@item:intable", "Type")));
        accessors.insert(KDCRMFields::objective(),
                         CampaignAccessorPair(&SugarCampaign::objective, &SugarCampaign::setObjective,
                                              i18nc("@item:intable", "Objective")));
        accessors.insert(KDCRMFields::content(),
                         CampaignAccessorPair(&SugarCampaign::content, &SugarCampaign::setContent,
                                              i18nc("@item:intable", "Content")));
        accessors.insert(KDCRMFields::frequency(),
                         CampaignAccessorPair(&SugarCampaign::frequency, &SugarCampaign::setFrequency,
                                              i18nc("@item:intable", "Frequency")));
    }

    return accessors;
}
