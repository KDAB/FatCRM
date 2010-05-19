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
      mWebsite = other.mWebsite;
      mTyckerSymbol = other.mTyckerSymbol;
      mMemberOf = other.mMemberOf;
      mOwnership = other.mOwnership;
      mIndustry = other.mIndustry;
      mType = other.mType;
      mCampaign = other.mCampaign;
      mAssignedTo = other.mAssignedTo;
      mOfficePhone = other.mOfficePhone;
      mFax = other.mFax;
      mOtherPhone = other.mOtherPhone;
      mEmployees = other.mEmployees;
      mRating = other.mRating;
      mSicCode = other.mSicCode;
      mAnnualRevenue = other.mAnnualRevenue;
      mBillingAddress = other.mBillingAddress;
      mCity = other.mCity;
      mState = other.mState;
      mPostalCode = other.mPostalCode;
      mCountry = other.mCountry;
      mShippingAddress = other.mShippingAddress;
      mShippingCity = other.mShippingCity;
      mShippingState = other.mShippingState;
      mShippingPostalCode = other.mShippingPostalCode;
      mShippingCountry = other.mShippingCountry;
      mPrimaryEmail = other.mPrimaryEmail;
      mOptedOutEmail = other.mOptedOutEmail;
      mInvalidEmail = other.mInvalidEmail;
      mDescription = other.mDescription;
    }

    bool mEmpty;

    QString mId;
    QString mName;
    QString mWebsite;
    QString mTyckerSymbol;
    QString mMemberOf;
    QString mOwnership;
    QString mIndustry;
    QString mType;
    QString mCampaign;
    QString mAssignedTo;
    QString mOfficePhone;
    QString mFax;
    QString mOtherPhone;
    QString mEmployees;
    QString mRating;
    QString mSicCode;
    QString mAnnualRevenue;
    QString mBillingAddress;
    QString mCity;
    QString mState;
    QString mPostalCode;
    QString mCountry;
    QString mShippingAddress;
    QString mShippingCity;
    QString mShippingState;
    QString mShippingPostalCode;
    QString mShippingCountry;
    QString mPrimaryEmail;
    QString mOptedOutEmail;
    QString mInvalidEmail;
    QString mDescription;

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
    if ( d->mWebsite != other.d->mWebsite )
        return false;
    if ( d->mTyckerSymbol != other.d->mTyckerSymbol )
        return false;
    if ( d->mMemberOf != other.d->mMemberOf )
        return false;
    if ( d->mOwnership != other.d->mOwnership )
        return false;
    if ( d->mIndustry != other.d->mIndustry )
        return false;
    if ( d->mType != other.d->mType )
        return false;
    if ( d->mCampaign != other.d->mCampaign )
        return false;
    if ( d->mAssignedTo != other.d->mAssignedTo )
        return false;
    if ( d->mOfficePhone != other.d->mOfficePhone )
        return false;
    if ( d->mFax != other.d->mFax )
        return false;
    if ( d->mOtherPhone != other.d->mOtherPhone )
        return false;
    if ( d->mEmployees != other.d->mEmployees )
        return false;
    if ( d->mRating != other.d->mRating )
        return false;
    if ( d->mSicCode != other.d->mSicCode )
        return false;
    if ( d->mAnnualRevenue != other.d->mAnnualRevenue )
        return false;
    if ( d->mBillingAddress != other.d->mBillingAddress )
        return false;
    if ( d->mCity != other.d->mCity )
        return false;
    if ( d->mState != other.d->mState )
        return false;
    if ( d->mPostalCode != other.d->mPostalCode )
        return false;
    if ( d->mCountry != other.d->mCountry )
        return false;
    if ( d->mShippingAddress != other.d->mShippingAddress )
        return false;
    if ( d->mShippingCity != other.d->mShippingCity )
        return false;
    if ( d->mShippingState != other.d->mShippingState )
        return false;
    if ( d->mShippingPostalCode != other.d->mShippingPostalCode )
        return false;
    if ( d->mShippingCountry != other.d->mShippingCountry )
        return false;
    if ( d->mPrimaryEmail != other.d->mPrimaryEmail )
        return false;
    if ( d->mOptedOutEmail != other.d->mOptedOutEmail )
        return false;
    if ( d->mInvalidEmail != other.d->mInvalidEmail )
        return false;
    if ( d->mDescription != other.d->mDescription )
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

void SugarAccount::setWebsite( const QString &value )
{
    d->mEmpty = false;
    d->mWebsite = value;
}


QString SugarAccount::website() const
{
    return d->mWebsite;
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

void SugarAccount::setMemberOf( const QString &value )
{
    d->mEmpty = false;
    d->mMemberOf = value;
}

QString SugarAccount::memberOf() const
{
    return d->mMemberOf;
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

void SugarAccount::setIndustry( const QString &value )
{
    d->mEmpty = false;
    d->mIndustry = value;
}

QString SugarAccount::industry() const
{
    return d->mIndustry;
}


void SugarAccount::setType( const QString &value )
{
    d->mEmpty = false;
    d->mType = value;
}

QString SugarAccount::type() const
{
    return d->mType;
}

void SugarAccount::setCampaign( const QString &value )
{
    d->mEmpty = false;
    d->mCampaign = value;
}

QString SugarAccount::campaign() const
{
    return d->mCampaign;
}


void SugarAccount::setAssignedTo( const QString &value )
{
    d->mEmpty = false;
    d->mAssignedTo = value;
}

QString SugarAccount::assignedTo() const
{
    return d->mAssignedTo;
}

void SugarAccount::setOfficePhone( const QString &value )
{
    d->mEmpty = false;
    d->mOfficePhone = value;
}

QString SugarAccount::officePhone() const
{
    return d->mOfficePhone;
}

void SugarAccount::setFax( const QString &value )
{
    d->mEmpty = false;
    d->mFax = value;
}

QString SugarAccount::fax() const
{
    return d->mFax;
}


void SugarAccount::setOtherPhone( const QString &value )
{
    d->mEmpty = false;
    d->mOtherPhone = value;
}

QString SugarAccount::otherPhone() const
{
    return d->mOtherPhone;
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


void SugarAccount::setRating( const QString &value )
{
    d->mEmpty = false;
    d->mRating = value;
}

QString SugarAccount::rating() const
{
    return d->mRating;
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

void SugarAccount::setAnnualRevenue( const QString &value )
{
    d->mEmpty = false;
    d->mAnnualRevenue = value;
}

QString SugarAccount::annualRevenue() const
{
    return d->mAnnualRevenue;
}

void SugarAccount::setBillingAddress( const QString &value )
{
    d->mEmpty = false;
    d->mBillingAddress = value;
}

QString SugarAccount::billingAddress() const
{
    return d->mBillingAddress;
}

void SugarAccount::setCity( const QString &value )
{
    d->mEmpty = false;
    d->mCity = value;
}

QString SugarAccount::city() const
{
    return d->mCity;
}

void SugarAccount::setState( const QString &value )
{
    d->mEmpty = false;
    d->mState = value;
}


QString SugarAccount::state() const
{
    return d->mState;
}

void SugarAccount::setPostalCode( const QString &value )
{
    d->mEmpty = false;
    d->mPostalCode = value;
}

QString SugarAccount::postalCode() const
{
    return d->mPostalCode;
}

void SugarAccount::setCountry( const QString &value )
{
    d->mEmpty = false;
    d->mCountry = value;
}

QString SugarAccount::country() const
{
    return d->mCountry;
}

void SugarAccount::setShippingAddress( const QString &value )
{
    d->mEmpty = false;
    d->mShippingAddress = value;
}

QString SugarAccount::shippingAddress() const
{
    return d->mShippingAddress;
}


void SugarAccount::setShippingCity( const QString &value )
{
    d->mEmpty = false;
    d->mShippingCity = value;
}

QString SugarAccount::shippingCity() const
{
    return d->mShippingCity;
}


void SugarAccount::setShippingState( const QString &value )
{
    d->mEmpty = false;
    d->mShippingState = value;
}

QString SugarAccount::shippingState() const
{
    return d->mShippingState;
}

void SugarAccount::setShippingPostalCode( const QString &value )
{
    d->mEmpty = false;
    d->mShippingPostalCode = value;
}

QString SugarAccount::shippingPostalCode() const
{
    return d->mShippingPostalCode;
}

void SugarAccount::setShippingCountry( const QString &value )
{
    d->mEmpty = false;
    d->mShippingCountry = value;
}

QString SugarAccount::shippingCountry() const
{
    return d->mShippingCountry;
}

void SugarAccount::setPrimaryEmail( const QString &value )
{
    d->mEmpty = false;
    d->mPrimaryEmail = value;
}

QString SugarAccount::primaryEmail() const
{
    return d->mPrimaryEmail;
}

void SugarAccount::setOptedOutEmail( const QString &value )
{
    d->mEmpty = false;
    d->mOptedOutEmail = value;
}

QString SugarAccount::optedOutEmail() const
{
    return d->mOptedOutEmail;
}


void SugarAccount::setInvalidEmail( const QString &value )
{
    d->mEmpty = false;
    d->mInvalidEmail = value;
}

QString SugarAccount::invalidEmail() const
{
    return d->mInvalidEmail;
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

QString SugarAccount::mimeType()
{
    return QLatin1String( "application/x-vnd.kdab.crm.account" );
}
