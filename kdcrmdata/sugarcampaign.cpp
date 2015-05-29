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

#include "sugarcampaign.h"
#include "kdcrmfields.h"

#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QDebug>

class SugarCampaign::Private : public QSharedData
{
public:
    Private()
        : mEmpty(true)
    {

    }

    Private(const Private &other)
        : QSharedData(other)
    {
        mEmpty = other.mEmpty;

        mId = other.mId;
        mName = other.mName;
        mDateEntered = other.mDateEntered;
        mDateModified = other.mDateModified;
        mModifiedUserId = other.mModifiedUserId;
        mModifiedByName = other.mModifiedByName;
        mCreatedBy = other.mCreatedBy;
        mCreatedByName = other.mCreatedByName;
        mDeleted = other.mDeleted;
        mAssignedUserId = other.mAssignedUserId;
        mAssignedUserName = other.mAssignedUserName;
        mTrackerKey = other.mTrackerKey;
        mTrackerCount = other.mTrackerCount;
        mReferUrl = other.mReferUrl;
        mTrackerText = other.mTrackerText;
        mStartDate = other.mStartDate;
        mEndDate = other.mEndDate;
        mStatus = other.mStatus;
        mImpressions = other.mImpressions;
        mCurrencyId = other.mCurrencyId;
        mBudget = other.mBudget;
        mExpectedCost = other.mExpectedCost;
        mActualCost = other.mActualCost;
        mExpectedRevenue = other.mExpectedRevenue;
        mCampaignType = other.mCampaignType;
        mObjective = other.mObjective;
        mContent = other.mContent;
        mFrequency = other.mFrequency;
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

bool SugarCampaign::operator==(const SugarCampaign &other) const
{
    if (d->mId != other.d->mId) {
        return false;
    }
    if (d->mName != other.d->mName) {
        return false;
    }
    if (d->mDateEntered != other.d->mDateEntered) {
        return false;
    }
    if (d->mDateModified != other.d->mDateModified) {
        return false;
    }
    if (d->mModifiedUserId != other.d->mModifiedUserId) {
        return false;
    }
    if (d->mModifiedByName != other.d->mModifiedByName) {
        return false;
    }
    if (d->mCreatedBy != other.d->mCreatedBy) {
        return false;
    }
    if (d->mCreatedByName != other.d->mCreatedByName) {
        return false;
    }
    if (d->mDeleted != other.d->mDeleted) {
        return false;
    }
    if (d->mAssignedUserId != other.d->mAssignedUserId) {
        return false;
    }
    if (d->mAssignedUserName != other.d->mAssignedUserName) {
        return false;
    }
    if (d->mTrackerKey != other.d->mTrackerKey) {
        return false;
    }
    if (d->mTrackerCount != other.d->mTrackerCount) {
        return false;
    }
    if (d->mReferUrl != other.d->mReferUrl) {
        return false;
    }
    if (d->mTrackerText != other.d->mTrackerText) {
        return false;
    }
    if (d->mStartDate != other.d->mStartDate) {
        return false;
    }
    if (d->mEndDate != other.d->mEndDate) {
        return false;
    }
    if (d->mStatus != other.d->mStatus) {
        return false;
    }
    if (d->mImpressions != other.d->mImpressions) {
        return false;
    }
    if (d->mCurrencyId != other.d->mCurrencyId) {
        return false;
    }
    if (d->mBudget != other.d->mBudget) {
        return false;
    }
    if (d->mExpectedCost != other.d->mExpectedCost) {
        return false;
    }
    if (d->mActualCost != other.d->mActualCost) {
        return false;
    }
    if (d->mExpectedRevenue != other.d->mExpectedRevenue) {
        return false;
    }
    if (d->mCampaignType != other.d->mCampaignType) {
        return false;
    }
    if (d->mObjective != other.d->mObjective) {
        return false;
    }
    if (d->mContent != other.d->mContent) {
        return false;
    }
    if (d->mFrequency != other.d->mFrequency) {
        return false;
    }

    return true;
}

bool SugarCampaign::operator!=(const SugarCampaign &a) const
{
    return !(a == *this);
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
    d->mId = data.value("id");
    d->mName = data.value("name");
    d->mDateEntered = data.value(KDCRMFields::dateEntered());
    d->mDateModified = data.value(KDCRMFields::dateModified());
    d->mModifiedUserId = data.value(KDCRMFields::modifiedUserId());
    d->mModifiedByName = data.value(KDCRMFields::modifiedUserId());
    d->mCreatedBy = data.value(KDCRMFields::createdBy());
    d->mCreatedByName = data.value(KDCRMFields::createdByName());
    d->mDeleted = data.value(KDCRMFields::deleted());
    d->mAssignedUserId = data.value(KDCRMFields::assignedUserId());
    d->mAssignedUserName = data.value(KDCRMFields::assignedUserName());
    d->mTrackerKey = data.value("trackerKey");
    d->mTrackerCount = data.value("trackerCount");
    d->mReferUrl = data.value("referUrl");
    d->mTrackerText = data.value("trackerText");
    d->mStartDate = data.value("startDate");
    d->mEndDate = data.value("endDate");
    d->mStatus = data.value("status");
    d->mImpressions = data.value("impressions");
    d->mCurrencyId = data.value(KDCRMFields::currencyId());
    d->mBudget = data.value("budget");
    d->mExpectedCost = data.value("expectedCost");
    d->mActualCost = data.value("actualCost");
    d->mExpectedRevenue = data.value("expectedRevenue");
    d->mCampaignType = data.value(KDCRMFields::campaignType());
    d->mObjective = data.value("objective");
    d->mContent = data.value(KDCRMFields::content());
    d->mFrequency = data.value("frequency");

}

QMap<QString, QString> SugarCampaign::data()
{
    QMap<QString, QString> data;
    data["id"] = d->mId;
    data["name"] = d->mName;
    data[KDCRMFields::dateEntered()] = d->mDateEntered;
    data[KDCRMFields::dateModified()] = d->mDateModified;
    data[KDCRMFields::modifiedUserId()] = d->mModifiedUserId;
    data["modifiedUserName"] = d->mModifiedByName;
    data[KDCRMFields::createdBy()] = d->mCreatedBy;
    data[KDCRMFields::createdByName()] = d->mCreatedByName;
    data[KDCRMFields::deleted()] = d->mDeleted;
    data[KDCRMFields::assignedUserId()] = d->mAssignedUserId;
    data[KDCRMFields::assignedUserName()] = d->mAssignedUserName;
    data["trackerKey"] = d->mTrackerKey;
    data["trackerCount"] = d->mTrackerCount;
    data["referUrl"] = d->mReferUrl;
    data["trackerText"] = d->mTrackerText;
    data["startDate"] = d->mStartDate;
    data["endDate"] = d->mEndDate;
    data["status"] = d->mStatus;
    data["impressions"] = d->mImpressions;
    data[KDCRMFields::currencyId()] = d->mCurrencyId;
    data["budget"] = d->mBudget;
    data["expectedCost"] = d->mExpectedCost;
    data["actualCost"] = d->mActualCost;
    data["expectedRevenue"] = d->mExpectedRevenue;
    data[KDCRMFields::campaignType()] = d->mCampaignType;
    data["objective"] = d->mObjective;
    data[KDCRMFields::content()] = d->mContent;
    data["frequency"] = d->mFrequency;
    return data;
}

QString SugarCampaign::mimeType()
{
    return QLatin1String("application/x-vnd.kdab.crm.campaign");
}

