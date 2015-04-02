#include "sugaropportunity.h"

#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QDate>
#include <QDebug>

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
    QString mDateModified;
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

void SugarOpportunity::setDateModified(const QString &value)
{
    d->mEmpty = false;
    d->mDateModified = value;
}

QString SugarOpportunity::dateModified() const
{
    return d->mDateModified;
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

void SugarOpportunity::setAccountName(const QString &value)
{
    d->mEmpty = false;
    d->mAccountName = value;
}

QString SugarOpportunity::accountName() const
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

void SugarOpportunity::setNextCallDate(const QDate &value)
{
    d->mEmpty = false;
    d->mNextCallDate = value;
}

QDate SugarOpportunity::nextCallDate() const
{
    return d->mNextCallDate;
}

void SugarOpportunity::setData(QMap<QString, QString> data)
{
    d->mEmpty = false;

    d->mId = data.value("id");
    d->mName = data.value("name");
    d->mDateEntered = data.value("dateEntered");
    d->mDateModified = data.value("dateModified");
    d->mModifiedUserId =  data.value("modifiedUserId");
    d->mModifiedByName = data.value("modifiedByName");
    d->mCreatedBy = data.value("createdBy");
    d->mCreatedByName = data.value("createdByName");
    d->mDescription = data.value("description");
    d->mDeleted = data.value("deleted");
    d->mAssignedUserId = data.value("assignedUserId");
    d->mAssignedUserName = data.value("assignedUserName");
    d->mOpportunityType = data.value("opportunityType");
    d->mAccountName = data.value("accountName");
    d->mAccountId = data.value("accountId");
    d->mCampaignId = data.value("campaignId");
    d->mCampaignName = data.value("campaignName");
    d->mLeadSource = data.value("leadSource");
    d->mAmount = data.value("amount");
    d->mAmountUsDollar = data.value("amountUsDollar");
    d->mCurrencyId = data.value("currencyId");
    d->mCurrencyName = data.value("currencyName");
    d->mCurrencySymbol = data.value("currencySymbol");
    d->mDateClosed = data.value("dateClosed");
    d->mNextStep = data.value("nextStep");
    d->mSalesStage = data.value("salesStage");
    d->mProbability = data.value("probability");
    d->mNextCallDate = QDate::fromString(data.value("nextCallDate"), "yyyy-MM-dd");
}

QMap<QString, QString> SugarOpportunity::data()
{
    QMap<QString, QString> data;
    data["id"] = d->mId;
    data["name"] = d->mName;
    data["dateEntered"] = d->mDateEntered;
    data["dateModified"] = d->mDateModified;
    data["modifiedUserId"] = d->mModifiedUserId;
    data["modifiedByName"] = d->mModifiedByName;
    data["createdBy"] = d->mCreatedBy;
    data["createdByName"] = d->mCreatedByName;
    data["description"] = d->mDescription;
    data["deleted"] = d->mDeleted;
    data["assignedUserId"] = d->mAssignedUserId;
    data["assignedUserName"] = d->mAssignedUserName;
    data["opportunityType"] = d->mOpportunityType;
    data["accountName"] = d->mAccountName;
    data["accountId"] = d->mAccountName;
    data["campaignId"] = d->mCampaignId;
    data["campaignName"] = d->mCampaignName;
    data["leadSource"] = d->mLeadSource;
    data["amount"] = d->mAmount;
    data["amountUsDollar"] = d->mAmountUsDollar;
    data["currencyId"] = d->mCurrencyId;
    data["currencyName"] = d->mCurrencyName;
    data["currencySymbol"] = d->mCurrencySymbol;
    data["dateClosed"] = d->mDateClosed;
    data["nextStep"] = d->mNextStep;
    data["salesStage"] = d->mSalesStage;
    data["probability"] = d->mProbability;
    data["nextCallDate"] = d->mNextCallDate.toString("yyyy-MM-dd");
    return data;
}

QString SugarOpportunity::mimeType()
{
    return QLatin1String("application/x-vnd.kdab.crm.opportunity");
}

