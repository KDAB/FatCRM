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

#include "sugaropportunity.h"
#include "kdcrmutils.h"
#include "kdcrmfields.h"

#include <KLocalizedString>

#include <QDate>
#include <QDebug>
#include <QSharedData>
#include <QString>

class SugarOpportunity::Private : public QSharedData
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
    QDateTime mDateModified;
    QString mModifiedUserId;
    QString mModifiedByName;
    QString mCreatedBy;
    QString mCreatedByName;
    QString mDescription;
    QString mDeleted;
    QString mAssignedUserId;
    QString mAssignedUserName;
    QString mOpportunityType;
    QString mAccountName;
    QString mAccountId;
    QString mCampaignId;
    QString mCampaignName;
    QString mLeadSource;
    QString mAmount;
    QString mAmountUsDollar;
    QString mCurrencyId;
    QString mCurrencyName;
    QString mCurrencySymbol;
    QString mDateClosed;
    QString mNextStep;
    QString mSalesStage;
    QString mProbability;
    QDate mNextCallDate;
};

SugarOpportunity::SugarOpportunity()
    : d(new Private)
{
}

SugarOpportunity::SugarOpportunity(const SugarOpportunity &other)
    : d(other.d)
{
}

SugarOpportunity::~SugarOpportunity()
{
}

SugarOpportunity &SugarOpportunity::operator=(const SugarOpportunity &other)
{
    if (this != &other) {
        d = other.d;
    }

    return *this;
}

bool SugarOpportunity::operator==(const SugarOpportunity &other) const
{
    if (d->mId != other.d->mId) {
        return false;
    }
    if (d->mName !=  other.d->mName) {
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
    if (d->mDescription != other.d->mDescription) {
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
    if (d->mOpportunityType != other.d->mOpportunityType) {
        return false;
    }
    if (d->mAccountName != other.d->mAccountName) {
        return false;
    }
    if (d->mAccountId != other.d->mAccountId) {
        return false;
    }
    if (d->mCampaignId != other.d->mCampaignId) {
        return false;
    }
    if (d->mCampaignName != other.d->mCampaignName) {
        return false;
    }
    if (d->mLeadSource != other.d->mLeadSource) {
        return false;
    }
    if (d->mAmount != other.d->mAmount) {
        return false;
    }
    if (d->mAmountUsDollar != other.d->mAmountUsDollar) {
        return false;
    }
    if (d->mCurrencyId != other.d->mCurrencyId) {
        return false;
    }
    if (d->mCurrencyName != other.d->mCurrencyName) {
        return false;
    }
    if (d->mCurrencySymbol != other.d->mCurrencySymbol) {
        return false;
    }
    if (d->mDateClosed != other.d->mDateClosed) {
        return false;
    }
    if (d->mNextStep != other.d->mNextStep) {
        return false;
    }
    if (d->mSalesStage != other.d->mSalesStage) {
        return false;
    }
    if (d->mProbability != other.d->mProbability) {
        return false;
    }
    if (d->mNextCallDate != other.d->mNextCallDate) {
        return false;
    }

    return true;
}

bool SugarOpportunity::operator!=(const SugarOpportunity &a) const
{
    return !(a == *this);
}

bool SugarOpportunity::isEmpty() const
{
    return d->mEmpty;
}

void SugarOpportunity::clear()
{
    *this = SugarOpportunity();
}

void SugarOpportunity::setId(const QString &id)
{
    d->mEmpty = false;
    d->mId = id;
}

QString SugarOpportunity::id() const
{
    return d->mId;
}

void SugarOpportunity::setName(const QString &name)
{
    d->mEmpty = false;
    d->mName = name;
}

QString SugarOpportunity::name() const
{
    return d->mName;
}

void SugarOpportunity::setDateEntered(const QString &value)
{
    d->mEmpty = false;
    d->mDateEntered = value;
}

QString SugarOpportunity::dateEntered() const
{
    return d->mDateEntered;
}

QDateTime SugarOpportunity::dateModified() const
{
    return d->mDateModified;
}

void SugarOpportunity::setDateModifiedRaw(const QString &value)
{
    d->mEmpty = false;
    d->mDateModified = KDCRMUtils::dateTimeFromString(value);
}

QString SugarOpportunity::dateModifiedRaw() const
{
    return KDCRMUtils::dateTimeToString(d->mDateModified);
}

void SugarOpportunity::setModifiedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedUserId = value;
}

QString SugarOpportunity::modifiedUserId() const
{
    return d->mModifiedUserId;
}

void SugarOpportunity::setModifiedByName(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedByName = value;
}

QString SugarOpportunity::modifiedByName() const
{
    return d->mModifiedByName;
}

void SugarOpportunity::setCreatedBy(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedBy = value;
}

QString SugarOpportunity::createdBy() const
{
    return d->mCreatedBy;
}

void SugarOpportunity::setCreatedByName(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedByName = value;
}

QString SugarOpportunity::createdByName() const
{
    return d->mCreatedByName;
}

void SugarOpportunity::setDescription(const QString &value)
{
    d->mEmpty = false;
    d->mDescription = value;
}

QString SugarOpportunity::description() const
{
    return d->mDescription;
}

void SugarOpportunity::setDeleted(const QString &value)
{
    d->mEmpty = false;
    d->mDeleted = value;
}

QString SugarOpportunity::deleted() const
{
    return d->mDeleted;
}

void SugarOpportunity::setAssignedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserId = value;
}

QString SugarOpportunity::assignedUserId() const
{
    return d->mAssignedUserId;
}

void SugarOpportunity::setAssignedUserName(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserName = value;
}

QString SugarOpportunity::assignedUserName() const
{
    return d->mAssignedUserName;
}

void SugarOpportunity::setOpportunityType(const QString &value)
{
    d->mEmpty = false;
    d->mOpportunityType = value;
}

QString SugarOpportunity::opportunityType() const
{
    return d->mOpportunityType;
}

void SugarOpportunity::setTempAccountName(const QString &value)
{
    d->mEmpty = false;
    d->mAccountName = value;
}

QString SugarOpportunity::tempAccountName() const
{
    return d->mAccountName;
}

void SugarOpportunity::setAccountId(const QString &value)
{
    d->mEmpty = false;
    d->mAccountId = value;
}

QString SugarOpportunity::accountId() const
{
    return d->mAccountId;
}

void SugarOpportunity::setCampaignId(const QString &value)
{
    d->mEmpty = false;
    d->mCampaignId = value;
}

QString SugarOpportunity::campaignId() const
{
    return d->mCampaignId;
}

void SugarOpportunity::setCampaignName(const QString &value)
{
    d->mEmpty = false;
    d->mCampaignName = value;
}

QString SugarOpportunity::campaignName() const
{
    return d->mCampaignName;
}

void SugarOpportunity::setLeadSource(const QString &value)
{
    d->mEmpty = false;
    d->mLeadSource = value;
}

QString SugarOpportunity::leadSource() const
{
    return d->mLeadSource;
}

void SugarOpportunity::setAmount(const QString &value)
{
    d->mEmpty = false;
    d->mAmount = value;
}

QString SugarOpportunity::amount() const
{
    return d->mAmount;
}

void SugarOpportunity::setAmountUsDollar(const QString &value)
{
    d->mEmpty = false;
    d->mAmountUsDollar = value;
}

QString SugarOpportunity::amountUsDollar() const
{
    return d->mAmountUsDollar;
}

void SugarOpportunity::setCurrencyId(const QString &value)
{
    d->mEmpty = false;
    d->mCurrencyId = value;
}

QString SugarOpportunity::currencyId() const
{
    return d->mCurrencyId;
}

void SugarOpportunity::setCurrencyName(const QString &value)
{
    d->mEmpty = false;
    d->mCurrencyName = value;
}

QString SugarOpportunity::currencyName() const
{
    return d->mCurrencyName;
}

void SugarOpportunity::setCurrencySymbol(const QString &value)
{
    d->mEmpty = false;
    d->mCurrencySymbol = value;
}

QString SugarOpportunity::currencySymbol() const
{
    return d->mCurrencySymbol;
}

void SugarOpportunity::setDateClosed(const QString &value)
{
    d->mEmpty = false;
    d->mDateClosed = value;
}

QString SugarOpportunity::dateClosed() const
{
    return d->mDateClosed;
}

void SugarOpportunity::setNextStep(const QString &value)
{
    d->mEmpty = false;
    d->mNextStep = value;
}

QString SugarOpportunity::nextStep() const
{
    return d->mNextStep;
}

void SugarOpportunity::setSalesStage(const QString &value)
{
    d->mEmpty = false;
    d->mSalesStage = value;
}

QString SugarOpportunity::salesStage() const
{
    return d->mSalesStage;
}

void SugarOpportunity::setProbability(const QString &value)
{
    d->mEmpty = false;
    d->mProbability = value;
}

QString SugarOpportunity::probability() const
{
    return d->mProbability;
}

QDate SugarOpportunity::nextCallDate() const
{
    return d->mNextCallDate;
}

void SugarOpportunity::setNextCallDateRaw(const QString &str)
{
    d->mEmpty = false;
    d->mNextCallDate = KDCRMUtils::dateFromString(str);
}

QString SugarOpportunity::nextCallDateRaw() const
{
    return KDCRMUtils::dateToString(d->mNextCallDate);
}

void SugarOpportunity::setData(const QMap<QString, QString>& data)
{
    d->mEmpty = false;

    d->mId = data.value(KDCRMFields::id());
    d->mName = data.value("name");
    d->mDateEntered = data.value(KDCRMFields::dateEntered());
    d->mDateModified = KDCRMUtils::dateTimeFromString(data.value(KDCRMFields::dateModified()));
    d->mModifiedUserId =  data.value(KDCRMFields::modifiedUserId());
    d->mModifiedByName = data.value(KDCRMFields::modifiedByName());
    d->mCreatedBy = data.value(KDCRMFields::createdBy());
    d->mCreatedByName = data.value(KDCRMFields::createdByName());
    d->mDescription = data.value(KDCRMFields::description());
    d->mDeleted = data.value(KDCRMFields::deleted());
    d->mAssignedUserId = data.value(KDCRMFields::assignedUserId());
    d->mAssignedUserName = data.value(KDCRMFields::assignedUserName());
    d->mOpportunityType = data.value(KDCRMFields::opportunityType());
    d->mAccountName = data.value(KDCRMFields::accountName());
    d->mAccountId = data.value(KDCRMFields::accountId());
    d->mCampaignId = data.value(KDCRMFields::campaignId());
    d->mCampaignName = data.value(KDCRMFields::campaignName());
    d->mLeadSource = data.value(KDCRMFields::leadSource());
    d->mAmount = data.value(KDCRMFields::amount());
    d->mAmountUsDollar = data.value(KDCRMFields::amountUsDollar());
    d->mCurrencyId = data.value(KDCRMFields::currencyId());
    d->mCurrencyName = data.value(KDCRMFields::currencyName());
    d->mCurrencySymbol = data.value(KDCRMFields::currencySymbol());
    d->mDateClosed = data.value(KDCRMFields::dateClosed());
    d->mNextStep = data.value(KDCRMFields::nextStep());
    d->mSalesStage = data.value(KDCRMFields::salesStage());
    d->mProbability = data.value(KDCRMFields::probability());
    d->mNextCallDate = KDCRMUtils::dateFromString(data.value(KDCRMFields::nextCallDate()));
}

QMap<QString, QString> SugarOpportunity::data()
{
    QMap<QString, QString> data;
    data[KDCRMFields::id()] = d->mId;
    data["name"] = d->mName;
    data[KDCRMFields::dateEntered()] = d->mDateEntered;
    data[KDCRMFields::dateModified()] = KDCRMUtils::dateTimeToString(d->mDateModified);
    data[KDCRMFields::modifiedUserId()] = d->mModifiedUserId;
    data[KDCRMFields::modifiedByName()] = d->mModifiedByName;
    data[KDCRMFields::createdBy()] = d->mCreatedBy;
    data[KDCRMFields::createdByName()] = d->mCreatedByName;
    data[KDCRMFields::description()] = d->mDescription;
    data[KDCRMFields::deleted()] = d->mDeleted;
    data[KDCRMFields::assignedUserId()] = d->mAssignedUserId;
    data[KDCRMFields::assignedUserName()] = d->mAssignedUserName;
    data[KDCRMFields::opportunityType()] = d->mOpportunityType;
    data[KDCRMFields::accountName()] = d->mAccountName;
    data[KDCRMFields::accountId()] = d->mAccountId;
    data[KDCRMFields::campaignId()] = d->mCampaignId;
    data[KDCRMFields::campaignName()] = d->mCampaignName;
    data[KDCRMFields::leadSource()] = d->mLeadSource;
    data[KDCRMFields::amount()] = d->mAmount;
    data[KDCRMFields::amountUsDollar()] = d->mAmountUsDollar;
    data[KDCRMFields::currencyId()] = d->mCurrencyId;
    data[KDCRMFields::currencyName()] = d->mCurrencyName;
    data[KDCRMFields::currencySymbol()] = d->mCurrencySymbol;
    data[KDCRMFields::dateClosed()] = d->mDateClosed;
    data[KDCRMFields::nextStep()] = d->mNextStep;
    data[KDCRMFields::salesStage()] = d->mSalesStage;
    data[KDCRMFields::probability()] = d->mProbability;
    data[KDCRMFields::nextCallDate()] = KDCRMUtils::dateToString(d->mNextCallDate);
    return data;
}

QString SugarOpportunity::mimeType()
{
    return QLatin1String("application/x-vnd.kdab.crm.opportunity");
}


Q_GLOBAL_STATIC(SugarOpportunity::AccessorHash, s_accessors)

SugarOpportunity::AccessorHash SugarOpportunity::accessorHash()
{
    AccessorHash &accessors = *s_accessors();
    if (accessors.isEmpty()) {
        accessors.insert(QStringLiteral("id"),
                           OpportunityAccessorPair(&SugarOpportunity::id, &SugarOpportunity::setId, QString()));
        accessors.insert(QStringLiteral("name"),
                           OpportunityAccessorPair(&SugarOpportunity::name, &SugarOpportunity::setName,
                                            i18nc("@item:intable", "Name")));
        accessors.insert(QStringLiteral("date_entered"),
                           OpportunityAccessorPair(&SugarOpportunity::dateEntered, &SugarOpportunity::setDateEntered, QString()));
        accessors.insert(QStringLiteral("date_modified"),
                           OpportunityAccessorPair(&SugarOpportunity::dateModifiedRaw, &SugarOpportunity::setDateModifiedRaw, QString()));
        accessors.insert(QStringLiteral("modified_user_id"),
                           OpportunityAccessorPair(&SugarOpportunity::modifiedUserId, &SugarOpportunity::setModifiedUserId, QString()));
        accessors.insert(QStringLiteral("modified_by_name"),
                           OpportunityAccessorPair(&SugarOpportunity::modifiedByName, &SugarOpportunity::setModifiedByName, QString()));
        accessors.insert(QStringLiteral("created_by"),
                           OpportunityAccessorPair(&SugarOpportunity::createdBy, &SugarOpportunity::setCreatedBy, QString()));
        accessors.insert(QStringLiteral("created_by_name"),
                           OpportunityAccessorPair(&SugarOpportunity::createdByName, &SugarOpportunity::setCreatedByName,
                                            i18nc("@item:intable", "Created By")));
        accessors.insert(QStringLiteral("description"),
                           OpportunityAccessorPair(&SugarOpportunity::description, &SugarOpportunity::setDescription,
                                            i18nc("@item:intable", "Description")));
        accessors.insert(QStringLiteral("deleted"),
                           OpportunityAccessorPair(&SugarOpportunity::deleted, &SugarOpportunity::setDeleted, QString()));
        accessors.insert(QStringLiteral("assigned_user_id"),
                           OpportunityAccessorPair(&SugarOpportunity::assignedUserId, &SugarOpportunity::setAssignedUserId, QString()));
        accessors.insert(QStringLiteral("assigned_user_name"),
                           OpportunityAccessorPair(&SugarOpportunity::assignedUserName, &SugarOpportunity::setAssignedUserName,
                                            i18nc("@item:intable", "Assigned To")));
        accessors.insert(QStringLiteral("opportunity_type"),
                           OpportunityAccessorPair(&SugarOpportunity::opportunityType, &SugarOpportunity::setOpportunityType,
                                            i18nc("@item:intable", "Type")));
        accessors.insert(QStringLiteral("account_name"),
                           OpportunityAccessorPair(&SugarOpportunity::tempAccountName, &SugarOpportunity::setTempAccountName,
                                            i18nc("@item:intable", "Account")));
        // ### I wish this one was available, but SuiteCRM doesn't return it!
        // (see qdbus org.freedesktop.Akonadi.Resource.akonadi_sugarcrm_resource_3 /CRMDebug/modules/Opportunities availableFields)
        accessors.insert(QStringLiteral("account_id"),
                           OpportunityAccessorPair(&SugarOpportunity::accountId, &SugarOpportunity::setAccountId, QString()));
        accessors.insert(QStringLiteral("campaign_id"),
                           OpportunityAccessorPair(&SugarOpportunity::campaignId, &SugarOpportunity::setCampaignId, QString()));
        accessors.insert(QStringLiteral("campaign_name"),
                           OpportunityAccessorPair(&SugarOpportunity::campaignName, &SugarOpportunity::setCampaignName,
                                            i18nc("@item:intable", "Campaign")));
        accessors.insert(QStringLiteral("lead_source"),
                           OpportunityAccessorPair(&SugarOpportunity::leadSource, &SugarOpportunity::setLeadSource,
                                            i18nc("@item:intable", "Lead Source")));
        accessors.insert(QStringLiteral("amount"),
                           OpportunityAccessorPair(&SugarOpportunity::amount, &SugarOpportunity::setAmount,
                                            i18nc("@item:intable", "Amount")));
        accessors.insert(QStringLiteral("amount_usdollar"),
                           OpportunityAccessorPair(&SugarOpportunity::amountUsDollar, &SugarOpportunity::setAmountUsDollar,
                                            i18nc("@item:intable", "Amount in USD")));
        accessors.insert(QStringLiteral("currency_id"),
                           OpportunityAccessorPair(&SugarOpportunity::currencyId, &SugarOpportunity::setCurrencyId, QString()));
        accessors.insert(QStringLiteral("currency_name"),
                           OpportunityAccessorPair(&SugarOpportunity::currencyName, &SugarOpportunity::setCurrencyName,
                                            i18nc("@item:intable", "Currency")));
        accessors.insert(QStringLiteral("currency_symbol"),
                           OpportunityAccessorPair(&SugarOpportunity::currencySymbol, &SugarOpportunity::setCurrencySymbol, QString()));
        accessors.insert(QStringLiteral("date_closed"),
                           OpportunityAccessorPair(&SugarOpportunity::dateClosed, &SugarOpportunity::setDateClosed, QString()));
        accessors.insert(QStringLiteral("next_step"),
                           OpportunityAccessorPair(&SugarOpportunity::nextStep, &SugarOpportunity::setNextStep,
                                            i18nc("@item:intable", "Next Step")));
        accessors.insert(QStringLiteral("sales_stage"),
                           OpportunityAccessorPair(&SugarOpportunity::salesStage, &SugarOpportunity::setSalesStage,
                                            i18nc("@item:intable", "Sales Stage")));
        accessors.insert(QStringLiteral("probability"),
                           OpportunityAccessorPair(&SugarOpportunity::probability, &SugarOpportunity::setProbability,
                                            i18nc("@item:intable", "Probability (percent)")));
        accessors.insert(QStringLiteral("next_call_date_c"),
                           OpportunityAccessorPair(&SugarOpportunity::nextCallDateRaw, &SugarOpportunity::setNextCallDateRaw,
                                            i18nc("@item:intable", "Next Call Date")));

    }
    return accessors;
}
