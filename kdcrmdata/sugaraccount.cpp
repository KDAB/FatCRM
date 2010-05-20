#include "sugaraccount.h"

#include <QtCore/QSharedData>
#include <QtCore/QString>

class SugarAccount::Private : public QSharedData
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
      mAccountType = other.mAccountType;
      mIndustry = other.mIndustry;
      mAnnualRevenue = other.mAnnualRevenue;
      mPhoneFax = other.mPhoneFax;
      mBillingAddressStreet = other.mBillingAddressStreet;
      mBillingAddressCity = other.mBillingAddressCity;
      mBillingAddressState = other.mBillingAddressState;
      mBillingAddressPostalcode = other.mBillingAddressPostalcode;
      mBillingAddressCountry = other.mBillingAddressCountry;
      mRating = other.mRating;
      mPhoneOffice = other.mPhoneOffice;
      mPhoneAlternate = other.mPhoneAlternate;
      mWebsite = other.mWebsite;
      mOwnership = other.mOwnership;
      mEmployees = other.mEmployees;
      mTyckerSymbol = other.mTyckerSymbol;
      mShippingAddressStreet = other.mShippingAddressStreet;
      mShippingAddressCity = other.mShippingAddressCity;
      mShippingAddressState = other.mShippingAddressState;
      mShippingAddressPostalcode = other.mShippingAddressPostalcode;
      mShippingAddressCountry = other.mShippingAddressCountry;
      mEmail1= other.mEmail1;
      mParentId = other.mParentId;
      mParentName = other.mParentName;
      mSicCode = other.mSicCode;
      mCampaignId = other.mCampaignId;
      mCampaignName = other.mCampaignName;
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
    QString mAccountType;
    QString mIndustry;
    QString mAnnualRevenue;
    QString mPhoneFax;
    QString mBillingAddressStreet;
    QString mBillingAddressCity;
    QString mBillingAddressState;
    QString mBillingAddressPostalcode;
    QString mBillingAddressCountry;
    QString mRating;
    QString mPhoneOffice;
    QString mPhoneAlternate;
    QString mWebsite;
    QString mOwnership;
    QString mEmployees;
    QString mTyckerSymbol;
    QString mShippingAddressStreet;
    QString mShippingAddressCity;
    QString mShippingAddressState;
    QString mShippingAddressPostalcode;
    QString mShippingAddressCountry;
    QString mEmail1;
    QString mParentId;
    QString mParentName;
    QString mSicCode;
    QString mCampaignId;
    QString mCampaignName;
};

SugarAccount::SugarAccount()
  : d( new Private )
{
}


SugarAccount::SugarAccount( const SugarAccount &other )
  : d( other.d )
{
}

SugarAccount::~SugarAccount()
{
}

SugarAccount &SugarAccount::operator=( const SugarAccount &other )
{
  if ( this != &other ) {
    d = other.d;
  }

  return *this;
}

bool SugarAccount::operator==( const SugarAccount &other ) const
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
    if ( d->mAccountType != other.d->mAccountType )
        return false;
    if ( d->mIndustry != other.d->mIndustry )
        return false;
    if ( d->mAnnualRevenue != other.d->mAnnualRevenue )
        return false;
    if ( d->mPhoneFax != other.d->mPhoneFax )
        return false;
    if ( d->mBillingAddressStreet != other.d->mBillingAddressStreet )
        return false;
    if ( d->mBillingAddressCity != other.d->mBillingAddressCity )
        return false;
    if ( d->mBillingAddressState != other.d->mBillingAddressState )
        return false;
    if ( d->mBillingAddressPostalcode != other.d->mBillingAddressPostalcode )
        return false;
    if ( d->mBillingAddressCountry != other.d->mBillingAddressCountry )
        return false;
    if ( d->mRating != other.d->mRating )
        return false;
    if ( d->mPhoneOffice != other.d->mPhoneOffice )
        return false;
    if ( d->mPhoneAlternate != other.d->mPhoneAlternate )
        return false;
    if ( d->mWebsite != other.d->mWebsite )
        return false;
    if ( d->mOwnership != other.d->mOwnership )
        return false;
    if ( d->mEmployees != other.d->mEmployees )
        return false;
    if ( d->mTyckerSymbol != other.d->mTyckerSymbol )
        return false;
    if ( d->mShippingAddressStreet != other.d->mShippingAddressStreet )
        return false;
    if ( d->mShippingAddressCity != other.d->mShippingAddressCity )
        return false;
    if ( d->mShippingAddressState != other.d->mShippingAddressState )
        return false;
    if ( d->mShippingAddressPostalcode != other.d->mShippingAddressPostalcode )
        return false;
    if ( d->mShippingAddressCountry != other.d->mShippingAddressCountry )
        return false;
    if ( d->mEmail1 != other.d->mEmail1 )
        return false;
    if ( d->mParentId != other.d->mParentId )
        return false;
    if ( d->mParentName != other.d->mParentName )
        return false;
    if ( d->mSicCode != other.d->mSicCode )
        return false;
    if ( d->mCampaignId != other.d->mCampaignId )
        return false;
    if ( d->mCampaignName != other.d->mCampaignName )
        return false;

  return true;
}

bool SugarAccount::operator!=( const SugarAccount &a ) const
{
  return !( a == *this );
}

bool SugarAccount::isEmpty() const
{
  return d->mEmpty;
}

void SugarAccount::clear()
{
  *this = SugarAccount();
}

void SugarAccount::setId( const QString &id )
{
  d->mEmpty = false;
  d->mId = id;
}

QString SugarAccount::id() const
{
  return d->mId;
}

void SugarAccount::setName( const QString &name )
{
    d->mEmpty = false;
    d->mName = name;
}

QString SugarAccount::name() const
{
    return d->mName;
}

void SugarAccount::setDateEntered( const QString &value )
{
    d->mEmpty = false;
    d->mDateEntered = value;
}

QString SugarAccount::dateEntered() const
{
    return d->mDateEntered;
}

void SugarAccount::setDateModified( const QString &value )
{
    d->mEmpty = false;
    d->mDateModified = value;
}

QString SugarAccount::dateModified() const
{
    return d->mDateModified;
}

void SugarAccount::setModifiedUserId( const QString &value )
{
    d->mEmpty = false;
    d->mModifiedUserId = value;
}

QString SugarAccount::modifiedUserId() const
{
    return d->mModifiedUserId;
}

void SugarAccount::setModifiedByName( const QString &value )
{
    d->mEmpty = false;
    d->mModifiedByName = value;
}

QString SugarAccount::modifiedByName() const
{
    return d->mModifiedByName;
}

void SugarAccount::setCreatedBy( const QString &value )
{
    d->mEmpty = false;
    d->mCreatedBy = value;
}

QString SugarAccount::createdBy() const
{
    return d->mCreatedBy;
}

void SugarAccount::setCreatedByName( const QString &value )
{
    d->mEmpty = false;
    d->mCreatedByName = value;
}

QString SugarAccount::createdByName() const
{
    return d->mCreatedByName;
}

void SugarAccount::setDescription( const QString &value )
{
    d->mEmpty = false;
    d->mDescription = value;
}

QString SugarAccount::description() const
{
    return d->mDescription;
}

void SugarAccount::setDeleted( const QString &value )
{
    d->mEmpty = false;
    d->mDeleted = value;
}

QString SugarAccount::deleted() const
{
    return d->mDeleted;
}

void SugarAccount::setAssignedUserId( const QString &value )
{
    d->mEmpty = false;
    d->mAssignedUserId = value;
}

QString SugarAccount::assignedUserId() const
{
    return d->mAssignedUserId;
}

void SugarAccount::setAssignedUserName( const QString &value )
{
    d->mEmpty = false;
    d->mAssignedUserName = value;
}

QString SugarAccount::assignedUserName() const
{
    return d->mAssignedUserName;
}

void SugarAccount::setAccountType( const QString &value )
{
    d->mEmpty = false;
    d->mAccountType = value;
}

QString SugarAccount::accountType() const
{
    return d->mAccountType;
}

void SugarAccount::setIndustry( const QString &value )
{
    d->mEmpty = false;
    d->mIndustry = value;
}

QString SugarAccount::industry() const
{
    return d->mIndustry;
}

void SugarAccount::setAnnualRevenue( const QString &value )
{
    d->mEmpty = false;
    d->mAnnualRevenue = value;
}

QString SugarAccount::annualRevenue() const
{
    return d->mAnnualRevenue;
}

void SugarAccount::setPhoneFax( const QString &value )
{
    d->mEmpty = false;
    d->mPhoneFax = value;
}

QString SugarAccount::phoneFax() const
{
    return d->mPhoneFax;
}

void SugarAccount::setBillingAddressStreet( const QString &value )
{
    d->mEmpty = false;
    d->mBillingAddressStreet = value;
}

QString SugarAccount::billingAddressStreet() const
{
    return d->mBillingAddressStreet;
}

void SugarAccount::setBillingAddressCity( const QString &value )
{
    d->mEmpty = false;
    d->mBillingAddressCity = value;
}

QString SugarAccount::billingAddressCity() const
{
    return d->mBillingAddressCity;
}

void SugarAccount::setBillingAddressState( const QString &value )
{
    d->mEmpty = false;
    d->mBillingAddressState = value;
}

QString SugarAccount::billingAddressState() const
{
    return d->mBillingAddressState;
}

void SugarAccount::setBillingAddressPostalcode( const QString &value )
{
    d->mEmpty = false;
    d->mBillingAddressPostalcode = value;
}

QString SugarAccount::billingAddressPostalcode() const
{
    return d->mBillingAddressPostalcode;
}

void SugarAccount::setBillingAddressCountry( const QString &value )
{
    d->mEmpty = false;
    d->mBillingAddressCountry = value;
}

QString SugarAccount::billingAddressCountry() const
{
    return d->mBillingAddressCountry;
}

void SugarAccount::setRating( const QString &value )
{
    d->mEmpty = false;
    d->mRating = value;
}

QString SugarAccount::rating() const
{
    return d->mRating;
}

void SugarAccount::setPhoneOffice( const QString &value )
{
    d->mEmpty = false;
    d->mPhoneOffice = value;
}

QString SugarAccount::phoneOffice() const
{
    return d->mPhoneOffice;
}

void SugarAccount::setPhoneAlternate( const QString &value )
{
    d->mEmpty = false;
    d->mPhoneAlternate = value;
}

QString SugarAccount::phoneAlternate() const
{
    return d->mPhoneAlternate;
}

void SugarAccount::setWebsite( const QString &value )
{
    d->mEmpty = false;
    d->mWebsite = value;
}


QString SugarAccount::website() const
{
    return d->mWebsite;
}

void SugarAccount::setOwnership( const QString &value )
{
    d->mEmpty = false;
    d->mOwnership = value;
}

QString SugarAccount::ownership() const
{
    return d->mOwnership;
}

void SugarAccount::setEmployees( const QString &value )
{
    d->mEmpty = false;
    d->mEmployees = value;
}

QString SugarAccount::employees() const
{
    return d->mEmployees;
}

void SugarAccount::setTyckerSymbol( const QString &value )
{
    d->mEmpty = false;
    d->mTyckerSymbol = value;
}

QString SugarAccount::tyckerSymbol() const
{
    return d->mTyckerSymbol;
}

void SugarAccount::setShippingAddressStreet( const QString &value )
{
    d->mEmpty = false;
    d->mShippingAddressStreet = value;
}

QString SugarAccount::shippingAddressStreet() const
{
    return d->mShippingAddressStreet;
}


void SugarAccount::setShippingAddressCity( const QString &value )
{
    d->mEmpty = false;
    d->mShippingAddressCity = value;
}

QString SugarAccount::shippingAddressCity() const
{
    return d->mShippingAddressCity;
}


void SugarAccount::setShippingAddressState( const QString &value )
{
    d->mEmpty = false;
    d->mShippingAddressState = value;
}

QString SugarAccount::shippingAddressState() const
{
    return d->mShippingAddressState;
}

void SugarAccount::setShippingAddressPostalcode( const QString &value )
{
    d->mEmpty = false;
    d->mShippingAddressPostalcode = value;
}

QString SugarAccount::shippingAddressPostalcode() const
{
    return d->mShippingAddressPostalcode;
}

void SugarAccount::setShippingAddressCountry( const QString &value )
{
    d->mEmpty = false;
    d->mShippingAddressCountry = value;
}

QString SugarAccount::shippingAddressCountry() const
{
    return d->mShippingAddressCountry;
}

void SugarAccount::setEmail1( const QString &value )
{
    d->mEmpty = false;
    d->mEmail1 = value;
}

QString SugarAccount::email1() const
{
    return d->mEmail1;
}

void SugarAccount::setParentId( const QString &value )
{
    d->mEmpty = false;
    d->mParentId = value;
}

QString SugarAccount::parentId() const
{
    return d->mParentId;
}

void SugarAccount::setParentName( const QString &value )
{
    d->mEmpty = false;
    d->mParentName = value;
}

QString SugarAccount::parentName() const
{
    return d->mParentName;
}

void SugarAccount::setSicCode( const QString &value )
{
    d->mEmpty = false;
    d->mSicCode = value;
}

QString SugarAccount::sicCode() const
{
    return d->mSicCode;
}

void SugarAccount::setCampaignId( const QString &value )
{
    d->mEmpty = false;
    d->mCampaignId = value;
}

QString SugarAccount::campaignId() const
{
    return d->mCampaignId;
}

void SugarAccount::setCampaignName( const QString &value )
{
    d->mEmpty = false;
    d->mCampaignName = value;
}

QString SugarAccount::campaignName() const
{
    return d->mCampaignName;
}

QString SugarAccount::mimeType()
{
    return QLatin1String( "application/x-vnd.kdab.crm.account" );
}
