#include "sugaropportunity.h"

#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QDebug>

class SugarOpportunity::Private : public QSharedData
{
  public:
    Private()
      : mEmpty( true )
    {

    }

    Private( const Private &other )
      : QSharedData( other )
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
      mDescription = other.mDescription;
      mDeleted = other.mDeleted;
      mAssignedUserId = other.mAssignedUserId;
      mAssignedUserName = other.mAssignedUserName;
      mOpportunityType = other.mOpportunityType;
      mAccountName = other.mAccountName;
      mCampaignId = other.mCampaignId;
      mCampaignName = other.mCampaignName;
      mLeadSource = other.mLeadSource;
      mAmount = other.mAmount;
      mAmountUsDollar = other.mAmountUsDollar;
      mCurrencyId = other.mCurrencyId;
      mCurrencyName = other.mCurrencyName;
      mCurrencySymbol = other.mCurrencySymbol;
      mDateClosed = other.mDateClosed;
      mNextStep = other.mNextStep;
      mSalesStage = other.mSalesStage;
      mProbability = other.mProbability;
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
};

SugarOpportunity::SugarOpportunity()
  : d( new Private )
{
}


SugarOpportunity::SugarOpportunity( const SugarOpportunity &other )
  : d( other.d )
{
}

SugarOpportunity::~SugarOpportunity()
{
}

SugarOpportunity &SugarOpportunity::operator=( const SugarOpportunity &other )
{
  if ( this != &other ) {
    d = other.d;
  }

  return *this;
}

bool SugarOpportunity::operator==( const SugarOpportunity &other ) const
{
    if ( d->mId != other.d->mId )
        return false;
    if ( d->mName !=  other.d->mName )
        return false;
    if ( d->mDateEntered != other.d->mDateEntered )
        return false;
    if ( d->mDateModified != other.d->mDateModified )
        return false;
    if ( d->mModifiedUserId != other.d->mModifiedUserId )
        return false;
    if ( d->mModifiedByName != other.d->mModifiedByName )
        return false;
    if ( d->mCreatedBy != other.d->mCreatedBy )
        return false;
    if ( d->mCreatedByName != other.d->mCreatedByName )
        return false;
    if ( d->mDescription != other.d->mDescription )
        return false;
    if ( d->mDeleted != other.d->mDeleted )
        return false;
    if ( d->mAssignedUserId != other.d->mAssignedUserId )
        return false;
    if ( d->mAssignedUserName != other.d->mAssignedUserName )
        return false;
    if ( d->mOpportunityType != other.d->mOpportunityType )
        return false;
    if ( d->mAccountName != other.d->mAccountName )
        return false;
    if ( d->mCampaignId != other.d->mCampaignId )
        return false;
    if ( d->mCampaignName != other.d->mCampaignName )
        return false;
    if ( d->mLeadSource != other.d->mLeadSource )
        return false;
    if ( d->mAmount != other.d->mAmount )
        return false;
    if ( d->mAmountUsDollar != other.d->mAmountUsDollar )
        return false;
    if ( d->mCurrencyId != other.d->mCurrencyId )
        return false;
    if ( d->mCurrencyName != other.d->mCurrencyName )
        return false;
    if ( d->mCurrencySymbol != other.d->mCurrencySymbol )
        return false;
    if ( d->mDateClosed != other.d->mDateClosed )
        return false;
    if ( d->mNextStep != other.d->mNextStep )
        return false;
    if ( d->mSalesStage != other.d->mSalesStage )
        return false;
    if ( d->mProbability != other.d->mProbability )
        return false;

  return true;
}

bool SugarOpportunity::operator!=( const SugarOpportunity &a ) const
{
  return !( a == *this );
}

bool SugarOpportunity::isEmpty() const
{
  return d->mEmpty;
}

void SugarOpportunity::clear()
{
  *this = SugarOpportunity();
}

void SugarOpportunity::setId( const QString &id )
{
  d->mEmpty = false;
  d->mId = id;
}

QString SugarOpportunity::id() const
{
  return d->mId;
}

void SugarOpportunity::setName( const QString &name )
{
    d->mEmpty = false;
    d->mName = name;
}

QString SugarOpportunity::name() const
{
    return d->mName;
}

void SugarOpportunity::setDateEntered( const QString &value )
{
    d->mEmpty = false;
    d->mDateEntered = value;
}

QString SugarOpportunity::dateEntered() const
{
    return d->mDateEntered;
}

void SugarOpportunity::setDateModified( const QString &value )
{
    d->mEmpty = false;
    d->mDateModified = value;
}

QString SugarOpportunity::dateModified() const
{
    return d->mDateModified;
}

void SugarOpportunity::setModifiedUserId( const QString &value )
{
    d->mEmpty = false;
    d->mModifiedUserId = value;
}

QString SugarOpportunity::modifiedUserId() const
{
    return d->mModifiedUserId;
}

void SugarOpportunity::setModifiedByName( const QString &value )
{
    d->mEmpty = false;
    d->mModifiedByName = value;
}

QString SugarOpportunity::modifiedByName() const
{
    return d->mModifiedByName;
}

void SugarOpportunity::setCreatedBy( const QString &value )
{
    d->mEmpty = false;
    d->mCreatedBy = value;
}

QString SugarOpportunity::createdBy() const
{
    return d->mCreatedBy;
}

void SugarOpportunity::setCreatedByName( const QString &value )
{
    d->mEmpty = false;
    d->mCreatedByName = value;
}

QString SugarOpportunity::createdByName() const
{
    return d->mCreatedByName;
}

void SugarOpportunity::setDescription( const QString &value )
{
    d->mEmpty = false;
    d->mDescription = value;
}

QString SugarOpportunity::description() const
{
    return d->mDescription;
}

void SugarOpportunity::setDeleted( const QString &value )
{
    d->mEmpty = false;
    d->mDeleted = value;
}

QString SugarOpportunity::deleted() const
{
    return d->mDeleted;
}

void SugarOpportunity::setAssignedUserId( const QString &value )
{
    d->mEmpty = false;
    d->mAssignedUserId = value;
}

QString SugarOpportunity::assignedUserId() const
{
    return d->mAssignedUserId;
}

void SugarOpportunity::setAssignedUserName( const QString &value )
{
    d->mEmpty = false;
    d->mAssignedUserName = value;
}

QString SugarOpportunity::assignedUserName() const
{
    return d->mAssignedUserName;
}

void SugarOpportunity::setOpportunityType( const QString &value )
{
    d->mEmpty = false;
    d->mOpportunityType = value;
}

QString SugarOpportunity::opportunityType() const
{
    return d->mOpportunityType;
}

void SugarOpportunity::setAccountName( const QString &value )
{
    d->mEmpty = false;
    d->mAccountName = value;
}

QString SugarOpportunity::accountName() const
{
    return d->mAccountName;
}

void SugarOpportunity::setCampaignId( const QString &value )
{
    d->mEmpty = false;
    d->mCampaignId = value;
}

QString SugarOpportunity::campaignId() const
{
    return d->mCampaignId;
}

void SugarOpportunity::setCampaignName( const QString &value )
{
    d->mEmpty = false;
    d->mCampaignName = value;
}

QString SugarOpportunity::campaignName() const
{
    return d->mCampaignName;
}

void SugarOpportunity::setLeadSource( const QString &value )
{
    d->mEmpty = false;
    d->mLeadSource = value;
}

QString SugarOpportunity::leadSource() const
{
    return d->mLeadSource;
}

void SugarOpportunity::setAmount( const QString &value )
{
    d->mEmpty = false;
    d->mAmount = value;
}

QString SugarOpportunity::amount() const
{
    return d->mAmount;
}

void SugarOpportunity::setAmountUsDollar( const QString &value )
{
    d->mEmpty = false;
    d->mAmountUsDollar = value;
}

QString SugarOpportunity::amountUsDollar() const
{
    return d->mAmountUsDollar;
}

void SugarOpportunity::setCurrencyId( const QString &value )
{
    d->mEmpty = false;
    d->mCurrencyId = value;
}

QString SugarOpportunity::currencyId() const
{
    return d->mCurrencyId;
}

void SugarOpportunity::setCurrencyName( const QString &value )
{
    d->mEmpty = false;
    d->mCurrencyName = value;
}

QString SugarOpportunity::currencyName() const
{
    return d->mCurrencyName;
}

void SugarOpportunity::setCurrencySymbol( const QString &value )
{
    d->mEmpty = false;
    d->mCurrencySymbol = value;
}

QString SugarOpportunity::currencySymbol() const
{
    return d->mCurrencySymbol;
}

void SugarOpportunity::setDateClosed( const QString &value )
{
    d->mEmpty = false;
    d->mDateClosed = value;
}

QString SugarOpportunity::dateClosed() const
{
    return d->mDateClosed;
}

void SugarOpportunity::setNextStep( const QString &value )
{
    d->mEmpty = false;
    d->mNextStep = value;
}

QString SugarOpportunity::nextStep() const
{
    return d->mNextStep;
}

void SugarOpportunity::setSalesStage( const QString &value )
{
    d->mEmpty = false;
    d->mSalesStage = value;
}


QString SugarOpportunity::salesStage() const
{
    return d->mSalesStage;
}

void SugarOpportunity::setProbability( const QString &value )
{
    d->mEmpty = false;
    d->mProbability = value;
}

QString SugarOpportunity::probability() const
{
    return d->mProbability;
}

QString SugarOpportunity::mimeType()
{
    return QLatin1String( "application/x-vnd.kdab.crm.opportunity" );
}

