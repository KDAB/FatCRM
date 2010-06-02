#include "sugarLead.h"

#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QDebug>

class SugarLead::Private : public QSharedData
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
      mSalutation = other.mSalutation;
      mFirstName = other.mFirstName;
      mLastName = other.mLastName;
      mTitle = other.mTitle;
      mDepartment = other.mDepartment;
      mDoNotCall = other.mDoNotCall;
      mPhoneHome = other.mPhoneHome;
      mPhoneMobile = other.mPhoneMobile;
      mPhoneWork = other.mPhoneWork;
      mPhoneOther = other.mPhoneOther;
      mPhoneFax = other.mPhoneFax;
      mEmail1 = other.mEmail1;
      mEmail2 = other.mEmail2;
      mPrimaryAddressStreet = other.mPrimaryAddressStreet;
      mPrimaryAddressCity = other.mPrimaryAddressCity;
      mPrimaryAddressState = other.mPrimaryAddressState;
      mPrimaryAddressPostalcode = other.mPrimaryAddressPostalcode;
      mPrimaryAddressCountry = other.mPrimaryAddressCountry;
      mAltAddressStreet = other.mAltAddressStreet;
      mAltAddressCity = other.mAltAddressCity;
      mAltAddressState = other.mAltAddressState;
      mAltAddressPostalcode = other.mAltAddressPostalcode;
      mAltAddressCountry = other.mAltAddressCountry;
      mAssistant = other.mAssistant;
      mAssistantPhone = other.mAssistantPhone;
      mConverted = other.mConverted;
      mReferedBy = other.mReferedBy;
      mLeadSource = other.mLeadSource;
      mLeadSourceDescription = other.mLeadSourceDescription;
      mStatus = other.mStatus;
      mStatusDescription = other.mStatusDescription;
      mReportsToId = other.mReportsToId;
      mReportToName = other.mReportToName;
      mAccountName = other.mAccountName;
      mAccountDescription =other.mAccountDescription;
      mContactId = other.mContactId;
      mAccountId = other.mAccountId;
      mOpportunityId = other.mOpportunityId;
      mOpportunityName = other.mOpportunityName;
      mOpportunityAmount = other.mOpportunityAmount;
      mCampaignId = other.mCampaignId;
      mCampaignName = other.mCampaignName;
      mCAcceptStatusFields = other.mCAcceptStatusFields;
      mMAcceptStatusFields = other.mMAcceptStatusFields;
      mBirthdate = other.mBirthdate;
      mPortalName = other.mPortalName;
      mPortalApp = other.mPortalApp;
    }

    bool mEmpty;

    QString mId;
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
    QString mSalutation;
    QString mFirstName;
    QString mLastName;
    QString mTitle;
    QString mDepartment;
    QString mDoNotCall;
    QString mPhoneHome;
    QString mPhoneMobile;
    QString mPhoneWork;
    QString mPhoneOther;
    QString mPhoneFax;
    QString mEmail1;
    QString mEmail2;
    QString mPrimaryAddressStreet;
    QString mPrimaryAddressCity;
    QString mPrimaryAddressState;
    QString mPrimaryAddressPostalcode;
    QString mPrimaryAddressCountry;
    QString mAltAddressStreet;
    QString mAltAddressCity;
    QString mAltAddressState;
    QString mAltAddressPostalcode;
    QString mAltAddressCountry;
    QString mAssistant;
    QString mAssistantPhone;
    QString mConverted;
    QString mReferedBy;
    QString mLeadSource;
    QString mLeadSourceDescription;
    QString mStatus;
    QString mStatusDescription;
    QString mReportsToId;
    QString mReportToName;
    QString mAccountName;
    QString mAccountDescription;
    QString mContactId;
    QString mAccountId;
    QString mOpportunityId;
    QString mOpportunityName;
    QString mOpportunityAmount;
    QString mCampaignId;
    QString mCampaignName;
    QString mCAcceptStatusFields;
    QString mMAcceptStatusFields;
    QString mBirthdate;
    QString mPortalName;
    QString mPortalApp;
};

SugarLead::SugarLead()
  : d( new Private )
{
}


SugarLead::SugarLead( const SugarLead &other )
  : d( other.d )
{
}

SugarLead::~SugarLead()
{
}

SugarLead &SugarLead::operator=( const SugarLead &other )
{
  if ( this != &other ) {
    d = other.d;
  }

  return *this;
}

bool SugarLead::operator==( const SugarLead &other ) const
{
    if ( d->mId != other.d->mId )
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
    if ( d->mSalutation != other.d->mSalutation )
        return false;
    if ( d->mFirstName != other.d->mFirstName )
        return false;
    if ( d->mLastName != other.d->mLastName )
        return false;
    if ( d->mTitle != other.d->mTitle )
        return false;
    if ( d->mDepartment != other.d->mDepartment )
        return false;
    if ( d->mDoNotCall != other.d->mDoNotCall )
        return false;
    if ( d->mPhoneHome != other.d->mPhoneHome )
        return false;
    if ( d->mPhoneMobile != other.d->mPhoneMobile )
        return false;
    if ( d->mPhoneWork != other.d->mPhoneWork )
        return false;
    if ( d->mPhoneOther != other.d->mPhoneOther )
        return false;
    if ( d->mPhoneFax != other.d->mPhoneFax )
        return false;
    if ( d->mEmail1 != other.d->mEmail1 )
        return false;
    if ( d->mEmail2 != other.d->mEmail2 )
        return false;
    if ( d->mPrimaryAddressStreet != other.d->mPrimaryAddressStreet )
        return false;
    if ( d->mPrimaryAddressCity != other.d->mPrimaryAddressCity )
        return false;
    if ( d->mPrimaryAddressState != other.d->mPrimaryAddressState )
        return false;
    if ( d->mPrimaryAddressPostalcode != other.d->mPrimaryAddressPostalcode )
        return false;
    if ( d->mPrimaryAddressCountry != other.d->mPrimaryAddressCountry )
        return false;
    if ( d->mAltAddressStreet != other.d->mAltAddressStreet )
        return false;
    if ( d->mAltAddressCity != other.d->mAltAddressCity )
        return false;
    if ( d->mAltAddressState != other.d->mAltAddressState )
        return false;
    if ( d->mAltAddressPostalcode != other.d->mAltAddressPostalcode )
        return false;
    if ( d->mAltAddressCountry != other.d->mAltAddressCountry )
        return false;
    if ( d->mAssistant != other.d->mAssistant )
        return false;
    if ( d->mAssistantPhone != other.d->mAssistantPhone )
        return false;
    if ( d->mConverted!= other.d->mConverted )
        return false;
    if ( d->mReferedBy != other.d->mReferedBy )
        return false;
    if ( d->mLeadSource != other.d->mLeadSource )
        return false;
    if ( d->mLeadSourceDescription != other.d->mLeadSourceDescription )
        return false;
    if ( d->mStatus != other.d->mStatus )
        return false;
    if ( d->mStatusDescription != other.d->mStatusDescription )
        return false;
    if ( d->mReportsToId != other.d->mReportsToId )
        return false;
    if ( d->mReportToName != other.d->mReportToName )
        return false;
    if ( d->mAccountName != other.d->mAccountName )
        return false;
    if ( d->mAccountDescription != other.d->mAccountDescription )
        return false;
    if ( d->mContactId != other.d->mContactId )
        return false;
    if ( d->mAccountId != other.d->mAccountId )
        return false;
    if ( d->mOpportunityId != other.d->mOpportunityId )
        return false;
    if ( d->mOpportunityName != other.d->mOpportunityName )
        return false;
    if ( d->mOpportunityAmount != other.d->mOpportunityAmount )
        return false;
    if ( d->mCampaignId != other.d->mCampaignId )
        return false;
    if ( d->mCAcceptStatusFields != other.d->mCAcceptStatusFields )
        return false;
    if ( d->mMAcceptStatusFields != other.d->mMAcceptStatusFields )
        return false;
    if ( d->mBirthdate != other.d->mBirthdate )
        return false;
    if ( d->mPortalName != other.d->mPortalName )
        return false;
    if ( d->mPortalApp != other.d->mPortalApp )
        return false;

  return true;
}

bool SugarLead::operator!=( const SugarLead &a ) const
{
  return !( a == *this );
}

bool SugarLead::isEmpty() const
{
  return d->mEmpty;
}

void SugarLead::clear()
{
  *this = SugarLead();
}

void SugarLead::setId( const QString &id )
{
  d->mEmpty = false;
  d->mId = id;
}

QString SugarLead::id() const
{
  return d->mId;
}

void SugarLead::setDateEntered( const QString &value )
{
    d->mEmpty = false;
    d->mDateEntered = value;
}

QString SugarLead::dateEntered() const
{
    return d->mDateEntered;
}

void SugarLead::setDateModified( const QString &value )
{
    d->mEmpty = false;
    d->mDateModified = value;
}

QString SugarLead::dateModified() const
{
    return d->mDateModified;
}

void SugarLead::setModifiedUserId( const QString &value )
{
    d->mEmpty = false;
    d->mModifiedUserId = value;
}

QString SugarLead::modifiedUserId() const
{
    return d->mModifiedUserId;
}

void SugarLead::setModifiedByName( const QString &value )
{
    d->mEmpty = false;
    d->mModifiedByName = value;
}

QString SugarLead::modifiedByName() const
{
    return d->mModifiedByName;
}

void SugarLead::setCreatedBy( const QString &value )
{
    d->mEmpty = false;
    d->mCreatedBy = value;
}

QString SugarLead::createdBy() const
{
    return d->mCreatedBy;
}

void SugarLead::setCreatedByName( const QString &value )
{
    d->mEmpty = false;
    d->mCreatedByName = value;
}

QString SugarLead::createdByName() const
{
    return d->mCreatedByName;
}

void SugarLead::setDescription( const QString &value )
{
    d->mEmpty = false;
    d->mDescription = value;
}

QString SugarLead::description() const
{
    return d->mDescription;
}

void SugarLead::setDeleted( const QString &value )
{
    d->mEmpty = false;
    d->mDeleted = value;
}

QString SugarLead::deleted() const
{
    return d->mDeleted;
}

void SugarLead::setAssignedUserId( const QString &value )
{
    d->mEmpty = false;
    d->mAssignedUserId = value;
}

QString SugarLead::assignedUserId() const
{
    return d->mAssignedUserId;
}

void SugarLead::setAssignedUserName( const QString &value )
{
    d->mEmpty = false;
    d->mAssignedUserName = value;
}

QString SugarLead::assignedUserName() const
{
    return d->mAssignedUserName;
}

void SugarLead::setSalutation( const QString &value );
{
    d->mEmpty = false;
    d->mSalutation = value;
}

QString SugarLead::salutation() const
{
    return d->mSalutation;
}

void SugarLead::setFirstName( const QString &value )
{
    d->mEmpty = false;
    d->mFirstName = value;
}

QString SugarLead::firstName() const
{
    return d->mFirstName;
}

void SugarLead::setLastName( const QString &value )
{
    d->mEmpty = false;
    d->mLastName = value;
}

QString SugarLead::lastName() const
{
    return d->mLastName;
}

void SugarLead::setTitle( const QString &value )
{
    d->mEmpty = false;
    d->mTitle = value;
}

QString SugarLead::title() const
{
    return d->mTitle;
}

void setDepartment( const QString &value )
{
    d->mEmpty = false;
    d->mDepartment = value;
}

QString department() const
{
    return d->mDepartment;
}

void setDoNotCall( const QString &value )
{
    d->mEmpty = false;
    d->mDoNotCall = value;
}

QString doNotCall() const
{
    return d->mDoNotCall;
}

void setPhoneHome( const QString &value )
{
    d->mEmpty = false;
    d->mPhoneHome = value;
}

QString phoneHome() const
{
    return d->mPhoneHome;
}

void setPhoneMobile( const QString &value )
{
    d->mEmpty = false;
    d->mPhoneMobile = value;
}

QString phoneMobile() const
{
    return d->mPhoneMobile;
}

void setPhoneWork( const QString &value )
{
    d->mEmpty = false;
    d->mPhoneWork = value;
}

QString phoneWork() const
{
    return d->mPhoneWork;
}

void setPhoneOther( const QString &value )
{
    d->mEmpty = false;
    d->mPhoneOther = value;
}

QString phoneOther() const
{
    return d->mPhoneOther;
}

void setPhoneFax( const QString &value )
{
    d->mEmpty = false;
    d->mPhoneFax = value;
}

QString phoneFax() const
{
    return d->mPhoneFax;
}

void setEmail1( const QString &value )
{
    d->mEmpty = false;
    d->mEmail1 = value;
}

QString email1() const
{
    return d->mEmail1;
}

void setEmail2( const QString &value )
{
    d->mEmpty = false;
    d->mEmail2 = value;
}

QString email2() const
{
    return d->mEmail2;
}

void SugarLead::setPrimaryAddressStreet( const QString &value )
{
    d->mEmpty = false;
    d->mPrimaryAddressStreet = value;
}

QString SugarLead::primaryAddressStreet() const
{
    return d->mPrimaryAddressStreet;
}

void SugarLead::setPrimaryAddressCity( const QString &value )
{
    d->mEmpty = false;
    d->mPrimaryAddressCity = value;
}

QString SugarLead::primaryAddressCity() const
{
    return d->mPrimaryAddressCity;
}

void SugarLead::setPrimaryAddressState( const QString &value )
{
    d->mEmpty = false;
    d->mPrimaryAddressState = value;
}

QString SugarLead::primaryAddressState() const
{
    return d->mPrimaryAddressState;
}

void SugarLead::setPrimaryAddressPostalcode( const QString &value )
{
    d->mEmpty = false;
    d->mPrimaryAddressPostalcode = value;
}

QString SugarLead::primaryAddressPostalcode() const
{
    return d->mPrimaryAddressPostalcode;
}

void SugarLead::setPrimaryAddressCountry( const QString &value )
{
    d->mEmpty = false;
    d->mPrimaryAddressCountry = value;
}

QString SugarLead::primaryAddressCountry() const
{
    return d->mPrimaryAddressCountry;
}

void SugarLead::setAltAddressStreet( const QString &value )
{
    d->mEmpty = false;
    d->mAltAddressStreet = value;
}

QString SugarLead::altAddressStreet() const
{
    return d->mAltAddressStreet;
}

void SugarLead::setAltAddressCity( const QString &value )
{
    d->mEmpty = false;
    d->mAltAddressCity = value;
}

QString SugarLead::altAddressCity() const
{
    return d->mAltAddressCity;
}

void SugarLead::setAltAddressState( const QString &value )
{
    d->mEmpty = false;
    d->mAltAddressState = value;
}

QString SugarLead::altAddressState() const
{
    return d->mAltAddressState;
}

void SugarLead::setAltAddressPostalcode( const QString &value )
{
    d->mEmpty = false;
    d->mAltAddressPostalcode = value;
}

QString SugarLead::altAddressPostalcode() const
{
    return d->mAltAddressPostalcode;
}

void SugarLead::setAltAddressCountry( const QString &value )
{
    d->mEmpty = false;
    d->mAltAddressCountry = value;
}

QString SugarLead::altAddressCountry() const
{
    return d->mAltAddressCountry;
}

void SugarLead::setAssistant( const QString &value )
{
    d->mEmpty = false;
    d->mAssistant = value;
}

QString SugarLead::assistant() const
{
    return d->mAssistant;
}

void SugarLead::setAssistantPhone( const QString &value )
{
    d->mEmpty = false;
    d->mAssistantPhone = value;
}

QString SugarLead::assistantPhone() const
{
    return d->mAssistantPhone;
}

void SugarLead::setConverted( const QString &value )
{
    d->mEmpty = false;
    d->mConverted = value;
}

QString SugarLead::converted() const
{
    return d->mConverted;
}

void SugarLead::setReferedBy( const QString &value )
{
    d->mEmpty = false;
    d->mReferedBy = value;
}


QString SugarLead::referedBy() const
{
    return d->mReferedBy;
}

void SugarLead::setLeadSource( const QString &value )
{
    d->mEmpty = false;
    d->mLeadSource = value;
}

QString SugarLead::leadSource() const
{
    return d->mLeadSource;
}

void SugarLead::setLeadSourceDescription( const QString &value )
{
    d->mEmpty = false;
    d->mLeadSourceDescription = value;
}

QString SugarLead::leadSourceDescription() const
{
    return d->mLeadSourceDescription;
}

void setStatus( const QString &value )
{
    d->mEmpty = false;
    d->mStatus = value;
}

QString status() const
{
    return d->mStatus;
}

void setStatusDescription( const QString &value )
{
    d->mEmpty = false;
    d->mStatusDescription = value;
}

QString statusDescription() const
{
    return d->mStatusDescription;
}

void SugarLead::setReportsToId( const QString &value )
{
    d->mEmpty = false;
    d->mReportsToId = value;
}

QString SugarLead::reportsToId() const
{
    return d->mReportsToId;
}

void SugarLead::setReportToName( const QString &value )
{
    d->mEmpty = false;
    d->mReportToName = value;
}

QString SugarLead::reportToName() const
{
    return d->mReportToName;
}

void SugarLead::setAccountName( const QString &value )
{
    d->mEmpty = false;
    d->mAccountName = value;
}

QString SugarLead::accountName() const
{
    return d->mAccountName;
}

void SugarLead::setAccountDescription( const QString &value )
{
    d->mEmpty = false;
    d->mAccountDescription = value;
}

QString SugarLead::accountDescription() const
{
    return d->mAccountDescription;
}

void SugarLead::setContactId( const QString &value )
{
    d->mEmpty = false;
    d->mContactId = value;
}

QString SugarLead::contactId() const
{
    return d->mContactId;
}

void SugarLead::setAccountId( const QString &value )
{
    d->mEmpty = false;
    d->mAccountId = value;
}

QString SugarLead::accountId() const
{
    return d->mAccountId;
}

void SugarLead::setOpportunityId( const QString &value )
{
    d->mEmpty = false;
    d->mOpportunityId = value;
}

QString SugarLead::opportunityId() const
{
    return d->mOpportunityId;
}

void SugarLead::setOpportunityName( const QString &value )
{
    d->mEmpty = false;
    d->mOpportunityName = value;
}

QString SugarLead::opportunityName() const
{
    return d->mOpportunityName;
}

void SugarLead::setOpportunityAmount( const QString &value )
{
    d->mEmpty = false;
    d->mOpportunityAmount = value;
}

QString SugarLead::opportunityAmount() const
{
    return d->mOpportunityAmount;
}

void SugarLead::setCampaignId( const QString &value )
{
    d->mEmpty = false;
    d->mCampaignId = value;
}

QString SugarLead::campaignId() const
{
    return d->mCampaignId;
}

void SugarLead::setCampaignName( const QString &value )
{
    d->mEmpty = false;
    d->mCampaignName = value;
}

QString SugarLead::campaignName() const
{
    return d->mCampaignName;
}

void SugarLead::setCAcceptStatusFields( const QString &value )
{
    d->mEmpty = false;
    d->mCAcceptStatusFields = value;
}

QString SugarLead::cAcceptStatusFields() const
{
    return d->mCAcceptStatusFields;
}

void SugarLead::setMAcceptStatusFields( const QString &value )
{
    d->mEmpty = false;
    d->mMAcceptStatusFields = value;
}

QString SugarLead::mMAcceptStatusFields() const
{
    return d->mMAcceptStatusFields;
}

void SugarLead::setBirthdate( const QString &value )
{
    d->mEmpty = false;
    d->mBirthdate = value;
}

QString SugarLead::birthDate() const
{
    return d->mBirthdate;
}

void SugarLead::setPortalName( const QString &value )
{
    d->mEmpty = false;
    d->mPortalName = value;
}

QString SugarLead::portalName() const
{
    return d->mPortalName;
}

void SugarLead::setPortalApp( const QString &value )
{
    d->mEmpty = false;
    d->mPortalApp = value;
}

QString SugarLead::portalApp() const
{
    return d->mPortalApp;
}

QString SugarLead::mimeType()
{
    return QLatin1String( "application/x-vnd.kdab.crm.lead" );
}

