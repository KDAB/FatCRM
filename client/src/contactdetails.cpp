#include "contactdetails.h"

#include <kabc/addressee.h>
#include <kabc/address.h>

ContactDetails::ContactDetails( QWidget *parent )
    : Details( parent )

{
    initialize();
}

ContactDetails::~ContactDetails()
{

}

void ContactDetails::initialize()
{
    QHBoxLayout *hLayout = new QHBoxLayout;
    // build the group boxes
    hLayout->addWidget( buildDetailsGroupBox() );
    hLayout->addWidget( buildOtherDetailsGroupBox() );
    hLayout->addWidget( buildAddressesGroupBox() );
    setLayout( hLayout );
}

QGroupBox* ContactDetails::buildDetailsGroupBox()
{
    mDetailsBox = new QGroupBox;

    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    mDetailsBox->setLayout( vLayout );
    mDetailsBox->setTitle( tr( "Details" ) );

    QLabel *fakedSpace = new QLabel( QString() );
    mSalutation = new QComboBox();
    mSalutation->setObjectName( "salutation" );
    mSalutation->addItems( salutationItems() );
    detailGrid->addWidget( fakedSpace, 0, 0 );
    detailGrid->addWidget( mSalutation, 0, 1 );
    QLabel *firstNameLabel = new QLabel( tr( "First name:" ) );
    mFirstName = new QLineEdit();
    mFirstName->setObjectName( "firstName" );
    detailGrid->addWidget( firstNameLabel, 0, 0 );
    detailGrid->addWidget( mFirstName, 0, 1 );
    QLabel *lastNameLabel = new QLabel( tr( "Last name: " ) );
    mLastName = new QLineEdit();
    mLastName->setObjectName( "lastName" );
    detailGrid->addWidget( lastNameLabel, 1, 0 );
    detailGrid->addWidget( mLastName, 1, 1 );
    QLabel *titleLabel = new QLabel( tr( "Job title: " ) );
    mTitle = new QLineEdit();
    mTitle->setObjectName( "title" );
    detailGrid->addWidget( titleLabel, 2, 0 );
    detailGrid->addWidget( mTitle, 2, 1 );
    QLabel *departmentLabel = new QLabel( tr( "Department: " ) );
    mDepartment = new QLineEdit();
    mDepartment->setObjectName( "department" );
    detailGrid->addWidget( departmentLabel, 3, 0 );
    detailGrid->addWidget( mDepartment, 3, 1 );
    QLabel *accountNameLabel = new QLabel( tr( "Account name: " ) );
    mAccountName = new QComboBox();
    mAccountName->setObjectName( "accountName" );
    mAccountName->addItem( 0,  QString( "" ) );
    detailGrid->addWidget( accountNameLabel, 4, 0 );
    detailGrid->addWidget( mAccountName, 4, 1 );
    QLabel *leadSourceLabel = new QLabel( tr( "Lead source: " ) );
    mLeadSource = new QComboBox();
    mLeadSource->setObjectName( "leadSource" );
    mLeadSource->addItems( sourceItems() );
    detailGrid->addWidget( leadSourceLabel, 5, 0 );
    detailGrid->addWidget( mLeadSource, 5, 1 );
    QLabel *campaignLabel = new QLabel( tr( "Campaign: " ) );
    mCampaign = new QComboBox();
    mCampaign->setObjectName( "campaign" );
    mCampaign->addItem( 0, QString( "" ) );
    detailGrid->addWidget( campaignLabel, 6, 0 );
    detailGrid->addWidget( mCampaign, 6, 1 );
    QLabel *assignedToLabel = new QLabel( tr( "Assigned to: " ) );
    mAssignedTo = new QComboBox();
    mAssignedTo->setObjectName( "assignedTo" );
    mAssignedTo->addItem( 0, QString( "" ) );
    detailGrid->addWidget( assignedToLabel, 7, 0 );
    detailGrid->addWidget( mAssignedTo, 7, 1 );
    QLabel *reportsToLabel = new QLabel( tr( "Reports to: " ) );
    mReportsTo = new QComboBox();
    mReportsTo->setObjectName( "reportsTo" );
    mReportsTo->addItem( 0, QString( "" ) );
    detailGrid->addWidget( reportsToLabel, 8, 0 );
    detailGrid->addWidget( mReportsTo, 8, 1 );
    QLabel *primaryEmailLabel = new QLabel( tr( "Primary email: " ) );
    mPrimaryEmail = new QLineEdit();
    mPrimaryEmail->setObjectName( "primaryEmail" );
    detailGrid->addWidget( primaryEmailLabel, 9, 0 );
    detailGrid->addWidget( mPrimaryEmail, 9, 1 );

    return mDetailsBox;
}

QGroupBox* ContactDetails::buildOtherDetailsGroupBox()
{

    // Calendar widgets
    mBirthDate = new QLineEdit();
    mBirthDate->setObjectName( "birthDate" );
    mClearDateButton = new QToolButton();
    mClearDateButton->setText( tr( "Clear" ) );

    connect( mClearDateButton, SIGNAL( clicked() ),
             this, SLOT( slotClearDate() ) );

    QWidget *calendarWidget = new QWidget();
    mCalendarButton = new EditCalendarButton( this );
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget( mBirthDate );
    hLayout->addWidget( mClearDateButton );
    hLayout->addWidget( mCalendarButton );
    calendarWidget->setLayout( hLayout );

    connect( mCalendarButton->calendarWidget(), SIGNAL(clicked(const QDate&)),
             this, SLOT(slotSetBirthday()));

    mOtherDetailsBox = new QGroupBox;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    mOtherDetailsBox->setLayout( vLayout );
    mOtherDetailsBox->setTitle( tr( "Other details" ) );

    QLabel *officePhoneLabel = new QLabel( tr( "Office phone: " ) );
    mOfficePhone = new QLineEdit();
    mOfficePhone->setObjectName( "officePhone" );
    detailGrid->addWidget( officePhoneLabel, 0, 0 );
    detailGrid->addWidget( mOfficePhone, 0, 1 );
    QLabel *mobileLabel = new QLabel( tr( "Mobile: " ) );
    mMobilePhone = new QLineEdit();
    mMobilePhone->setObjectName( "mobilePhone" );
    detailGrid->addWidget( mobileLabel, 1, 0 );
    detailGrid->addWidget( mMobilePhone, 1, 1 );
    QLabel *homePhoneLabel = new QLabel( tr( "Home phone: " ) );
    mHomePhone = new QLineEdit();
    mHomePhone->setObjectName( "homePhone" );
    detailGrid->addWidget( homePhoneLabel, 2, 0 );
    detailGrid->addWidget( mHomePhone, 2, 1 );
    QLabel *otherPhoneLabel = new QLabel( tr( "Other Phone: " ) );
    mOtherPhone = new QLineEdit();
    mOtherPhone->setObjectName( "otherPhone" );
    detailGrid->addWidget( otherPhoneLabel, 3, 0 );
    detailGrid->addWidget( mOtherPhone, 3, 1 );
    QLabel *faxLabel = new QLabel( tr( "Fax: " ) );
    mFax = new QLineEdit();
    mFax->setObjectName( "fax" );
    detailGrid->addWidget( faxLabel, 4, 0 );
    detailGrid->addWidget( mFax, 4, 1 );
    QLabel *birthDateLabel = new QLabel( tr( "Birthdate: ") );
    detailGrid->addWidget( birthDateLabel, 5, 0 );
    detailGrid->addWidget( calendarWidget, 5, 1 );
    QLabel *assistantLabel = new QLabel( tr( "Assistant: " ) );
    mAssistant = new QLineEdit();
    mAssistant->setObjectName( "assistant" );
    detailGrid->addWidget( assistantLabel, 6, 0 );
    detailGrid->addWidget( mAssistant, 6, 1 );
    QLabel *assistantPhoneLabel = new QLabel( tr( "Assistant phone: " ) );
    mAssistantPhone = new QLineEdit();
    mAssistantPhone->setObjectName( "assistantPhone" );
    detailGrid->addWidget( assistantPhoneLabel, 7, 0 );
    detailGrid->addWidget( mAssistantPhone, 7, 1 );
    QLabel *doNotCallLabel = new QLabel( tr( "Do not call: " ) );
    mDoNotCall = new QCheckBox( QString() );
    mDoNotCall->setObjectName( "doNotCall" );
    detailGrid->addWidget( doNotCallLabel, 8, 0 );
    detailGrid->addWidget( mDoNotCall, 8, 1 );

    return mOtherDetailsBox;
}

QGroupBox* ContactDetails::buildAddressesGroupBox()
{
    mAddressesBox = new QGroupBox;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    mAddressesBox->setLayout( vLayout );
    mAddressesBox->setTitle( tr( "Addresses" ) );

    QLabel *primaryLabel = new QLabel( tr( "Primary address: " ) );
    mPrimaryAddress = new QLineEdit();
    mPrimaryAddress->setObjectName( "primaryAddress" );
    detailGrid->addWidget( primaryLabel, 0, 0 );
    detailGrid->addWidget( mPrimaryAddress, 0, 1 );
    QLabel *cityLabel = new QLabel( tr( "City: " ) );
    mCity = new QLineEdit();
    mCity->setObjectName( "city" );
    detailGrid->addWidget( cityLabel, 1, 0 );
    detailGrid->addWidget( mCity, 1, 1 );
    QLabel *stateLabel = new QLabel( tr( "State: " ) );
    mState = new QLineEdit();
    mState->setObjectName( "state" );
    detailGrid->addWidget( stateLabel, 2, 0 );
    detailGrid->addWidget( mState, 2, 1 );
    QLabel *postalCodeLabel = new QLabel( tr( "Postal code: " ) );
    mPostalCode = new QLineEdit();
    mPostalCode->setObjectName( "postalCode" );
    detailGrid->addWidget( postalCodeLabel, 3, 0 );
    detailGrid->addWidget( mPostalCode, 3, 1 );
    QLabel *countryLabel = new QLabel( tr( "Country: " ) );
    mCountry = new QLineEdit();
    mCountry->setObjectName( "country" );
    detailGrid->addWidget( countryLabel, 4, 0 );
    detailGrid->addWidget( mCountry, 4, 1 );
    QLabel *otherAddressLabel = new QLabel( tr( "Other address: " ) );
    mOtherAddress = new QLineEdit();
    mOtherAddress->setObjectName( "otherAddress" );
    detailGrid->addWidget( otherAddressLabel, 5, 0 );
    detailGrid->addWidget( mOtherAddress, 5, 1 );
    QLabel *otherCityLabel = new QLabel( tr( "City: " ) );
    mOtherCity = new QLineEdit();
    mOtherCity->setObjectName( "otherCity" );
    detailGrid->addWidget( otherCityLabel, 6, 0 );
    detailGrid->addWidget( mOtherCity, 6, 1 );
    QLabel *otherStateLabel = new QLabel( tr( "State: " ) );
    mOtherState = new QLineEdit();
    mOtherState->setObjectName( "otherState" );
    detailGrid->addWidget( otherStateLabel, 7, 0 );
    detailGrid->addWidget( mOtherState, 7, 1 );
    QLabel *otherPostalCodeLabel = new QLabel( tr( "Postal code: " ) );
    mOtherPostalCode = new QLineEdit();
    mOtherPostalCode->setObjectName( "otherPostalCode" );
    detailGrid->addWidget( otherPostalCodeLabel, 8, 0 );
    detailGrid->addWidget( mOtherPostalCode, 8, 1 );
    QLabel *otherCountryLabel = new QLabel( tr( "Country: " ) );
    mOtherCountry = new QLineEdit();
    mOtherCountry->setObjectName( "otherCountry" );
    detailGrid->addWidget( otherCountryLabel, 9, 0 );
    detailGrid->addWidget( mOtherCountry, 9, 1 );

    return mAddressesBox;
}

void ContactDetails::slotSetBirthday()
{

   mBirthDate->setText( mCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mCalendarButton->calendarWidget()->setSelectedDate( QDate::currentDate() );
    mCalendarButton->calendarDialog()->close();

}

void ContactDetails::slotClearDate()
{
   mBirthDate->clear();
}

QMap<QString, QString> ContactDetails::data( const Akonadi::Item item ) const
{
    KABC::Addressee contact = item.payload<KABC::Addressee>();
    return contactData( contact );
}

QMap<QString,QString> ContactDetails::contactData( KABC::Addressee addressee ) const
{
    QMap<QString, QString> data;
    data["salutation"] = addressee.custom( "FATCRM", "X-Salutation" );
    data["firstName"] = addressee.givenName();
    data["lastName"] = addressee.familyName();
    data["title"] = addressee.title();
    data["department"] = addressee.department();
    data["accountName"] = addressee.organization();
    data["primaryEmail"] = addressee.preferredEmail();
    data["homePhone"] =addressee.phoneNumber( KABC::PhoneNumber::Home ).number();
    data["mobilePhone"] = addressee.phoneNumber( KABC::PhoneNumber::Cell ).number();
    data["officePhone"] = addressee.phoneNumber( KABC::PhoneNumber::Work ).number();
    data["otherPhone"] = addressee.phoneNumber( KABC::PhoneNumber::Car ).number();
    data["fax"] = addressee.phoneNumber( KABC::PhoneNumber::Fax ).number();

    const KABC::Address address = addressee.address( KABC::Address::Work|KABC::Address::Pref);
    data["primaryAddress"] = address.street();
    data["city"] = address.locality();
    data["state"] = address.region();
    data["postalCode"] = address.postalCode();
    data["country"] = address.country();

    const KABC::Address other = addressee.address( KABC::Address::Home );
    data["otherAddress"] = other.street();
    data["otherCity"] = other.locality();
    data["otherState"] = other.region();
    data["otherPostalCode"] = other.postalCode();
    data["otherCountry"] = other.country();
    data["birthDate"] = QDateTime( addressee.birthday() ).date().toString( QString("yyyy-MM-dd" ) );
    data["assistant"] = addressee.custom( "KADDRESSBOOK", "X-AssistantsName" );
    data["assistantPhone"] = addressee.custom( "FATCRM", "X-AssistantsPhone" );
    data["leadSource"] = addressee.custom( "FATCRM", "X-LeadSourceName" );
    data["campaign"] = addressee.custom( "FATCRM", "X-CampaignName" );
    data["assignedTo"] = addressee.custom( "FATCRM", "X-AssignedUserName" );
    data["reportsTo"] = addressee.custom( "FATCRM", "X-ReportsToUserName" );
    bool donotcall = addressee.custom( "FATCRM", "X-DoNotCall" ).isEmpty() || addressee.custom( "FATCRM", "X-DoNotCall" ) == "0" ? false : true;
    data["doNotCall"] = donotcall;
    data["description"] = addressee.note();
    data["modifiedBy"] = addressee.custom( "FATCRM", "X-ModifiedByName" );
    data["dateModified"] = addressee.custom( "FATCRM", "X-DateModified" );
    data["dateEntered"] = addressee.custom( "FATCRM", "X-DateCreated" );
    data["createdBy"] = addressee.custom( "FATCRM","X-CreatedByName" );
    data["modifiedUserId"] = addressee.custom( "FATCRM", "X-ModifiedUserId" );
    data["modifiedUserName"] = addressee.custom( "FATCRM", "X-ModifiedUserName" );
    data["contactId"] = addressee.custom( "FATCRM", "X-ContactId" );
    data["opportunityRoleFields"] = addressee.custom( "FATCRM", "X-OpportunityRoleFields" );
    data["cAcceptStatusFields"] = addressee.custom( "FATCRM", "X-CacceptStatusFields" );
    data["mAcceptStatusFields"] = addressee.custom( "FATCRM", "X-MacceptStatusFields" );
    data["deleted"] = addressee.custom( "FATCRM", "X-Deleted" );
    data["createdById"] = addressee.custom( "FATCRM", "X-CreatedById" );
    return data;
}
