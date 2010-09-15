#include "leaddetails.h"
#include "sugarclient.h"

#include <akonadi/item.h>

#include <kdcrmdata/sugarlead.h>

using namespace Akonadi;


LeadDetails::LeadDetails( QWidget *parent )
    : QWidget( parent )

{
    initialize();
}

LeadDetails::~LeadDetails()
{

}

void LeadDetails::initialize()
{
    QHBoxLayout *hLayout = new QHBoxLayout;
    // build the group boxes
    hLayout->addWidget( buildDetailsGroupBox() );
    hLayout->addWidget( buildOtherDetailsGroupBox() );
    hLayout->addWidget( buildAddressesGroupBox() );
    setLayout( hLayout );
}

QGroupBox* LeadDetails::buildDetailsGroupBox()
{
    /* build a BirthDate calendar widget */
    // Calendar widgets
    mBirthdate = new QLineEdit();
    mBirthdate->setObjectName( "birthdate" );
    mClearDateButton = new QToolButton();
    mClearDateButton->setText( tr( "Clear" ) );

    connect( mClearDateButton, SIGNAL( clicked() ),
             this, SLOT( slotClearDate() ) );

    QWidget *calendarWidget = new QWidget();
    mCalendarButton = new EditCalendarButton( this );
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget( mBirthdate );
    hLayout->addWidget( mClearDateButton );
    hLayout->addWidget( mCalendarButton );
    calendarWidget->setLayout( hLayout );

    connect( mCalendarButton->calendarWidget(), SIGNAL(clicked(const QDate&)),
             this, SLOT(slotSetBirthDate()));

    mDetailsBox = new QGroupBox;

    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    mDetailsBox->setLayout( vLayout );
    mDetailsBox->setTitle( tr( "Details" ) );

    QLabel *leadSourceLabel = new QLabel( tr("Lead source: "  ) );
    mLeadSource = new QComboBox();
    mLeadSource->setObjectName( "leadSource" );
    mLeadSource->addItems( sourceItems() );
    detailGrid->addWidget( leadSourceLabel, 0, 0 );
    detailGrid->addWidget( mLeadSource, 0, 1 );
    QLabel *descriptionLabel = new QLabel( tr( "Lead source description: " ) );
    mLeadSourceDescription = new QTextEdit();
    mLeadSourceDescription->setObjectName( "leadSourceDescription" );
    detailGrid->addWidget( descriptionLabel, 1, 0 );
    detailGrid->addWidget( mLeadSourceDescription, 1, 1 );
    QLabel *campaignLabel = new QLabel( tr( "Campaign: " ) );
    mCampaignName = new QComboBox();
    mCampaignName->setObjectName( "campaignName" );
    mCampaignName->addItem( 0, QString( "" ) );
    detailGrid->addWidget( campaignLabel, 2, 0 );
    detailGrid->addWidget( mCampaignName, 2, 1 );
    QLabel *referredByLabel = new QLabel( tr( "Referred by: " ) );
    mReferedBy = new QLineEdit();
    mReferedBy->setObjectName( "referedBy" );
    detailGrid->addWidget( referredByLabel, 3, 0 );
    detailGrid->addWidget( mReferedBy, 3, 1 );
    QLabel *firstNameLabel = new QLabel( tr( "First name: " ) );
    QWidget *salutationWidget = new QWidget();
    QHBoxLayout *widgetLayout = new QHBoxLayout;
    salutationWidget->setLayout( widgetLayout );
    mSalutation = new QComboBox();
    mSalutation->setObjectName( "salutation" );
    mSalutation->addItems( salutationItems() );
    mFirstName = new QLineEdit();
    mFirstName->setObjectName( "firstName" );
    widgetLayout->addWidget( mSalutation );
    widgetLayout->addWidget( mFirstName );
    detailGrid->addWidget( firstNameLabel, 4, 0 );
    detailGrid->addWidget( salutationWidget, 4, 1 );
    QLabel *lastNameLabel = new QLabel( tr( "Last name: " ) );
    mLastName = new QLineEdit();
    mLastName->setObjectName( "lastName" );
    detailGrid->addWidget( lastNameLabel, 5, 0 );
    detailGrid->addWidget( mLastName, 5, 1 );
    QLabel *birthDateLabel = new QLabel( tr( "Birthdate: " ) );
    detailGrid->addWidget( birthDateLabel, 6, 0 );
    detailGrid->addWidget( calendarWidget, 6, 1 );
    QLabel *accountNameLabel = new QLabel( tr( "Account name: " ) );
    mAccountName = new QLineEdit();
    mAccountName->setObjectName( "accountName" );
    detailGrid->addWidget( accountNameLabel, 7, 0 );
    detailGrid->addWidget( mAccountName, 7, 1 );
    QLabel *titleLabel = new QLabel( tr( "Title: " ) );
    mTitle = new QLineEdit();
    mTitle->setObjectName( "title" );
    detailGrid->addWidget( titleLabel, 8, 0 );
    detailGrid->addWidget( mTitle, 8, 1 );
    QLabel *departmentLabel = new QLabel( tr( "Department: " ) );
    mDepartment = new QLineEdit();
    mDepartment->setObjectName( "department" );
    detailGrid->addWidget( departmentLabel, 9, 0 );
    detailGrid->addWidget( mDepartment, 9, 1 );
    QLabel *assignedUserLabel = new QLabel( tr( "Assigned to: " ) );
    mAssignedUserName = new QComboBox();
    mAssignedUserName->setObjectName( "assignedUserName" );
    mAssignedUserName->addItem( 0, QString() );
    detailGrid->addWidget( assignedUserLabel, 10, 0 );
    detailGrid->addWidget( mAssignedUserName, 10, 1 );

    return mDetailsBox;
}

QGroupBox* LeadDetails::buildOtherDetailsGroupBox()
{

    mOtherDetailsBox = new QGroupBox;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    mOtherDetailsBox->setLayout( vLayout );
    mOtherDetailsBox->setTitle( tr( "Other details" ) );

    QLabel *statusLabel = new QLabel( tr( "Status: " ) );
    mStatus = new QComboBox();
    mStatus->setObjectName( "status" );
    mStatus->addItems( statusItems() );
    detailGrid->addWidget( statusLabel, 0, 0 );
    detailGrid->addWidget( mStatus, 0, 1 );
    QLabel *descriptionLabel = new QLabel( tr( "Status description: " ) );
    mStatusDescription = new QTextEdit();
    mStatusDescription->setObjectName( "statusDescription" );
    detailGrid->addWidget( descriptionLabel, 1, 0 );
    detailGrid->addWidget( mStatusDescription, 1, 1 );
    QLabel *opportunityAmountLabel = new QLabel( tr( "Opportunity amount: " ) );
    mOpportunityAmount = new QLineEdit();
    mOpportunityAmount->setObjectName( "opportunityAmount" );
    detailGrid->addWidget( opportunityAmountLabel, 2, 0 );
    detailGrid->addWidget( mOpportunityAmount, 2, 1 );
    QLabel *officePhoneLabel = new QLabel( tr( "Office phone: " ) );
    mPhoneWork = new QLineEdit();
    mPhoneWork->setObjectName( "phoneWork" );
    detailGrid->addWidget( officePhoneLabel, 3, 0 );
    detailGrid->addWidget( mPhoneWork, 3, 1 );
    QLabel *mobileLabel = new QLabel( tr( "Mobile: " ) );
    mPhoneMobile = new QLineEdit();
    mPhoneMobile->setObjectName( "phoneMobile" );
    detailGrid->addWidget( mobileLabel, 4, 0 );
    detailGrid->addWidget( mPhoneMobile, 4, 1 );
    QLabel *homeLabel = new QLabel( tr( "Home phone: ") );
    mPhoneHome = new QLineEdit();
    mPhoneHome->setObjectName( "phoneHome" );
    detailGrid->addWidget( homeLabel, 5, 0 );
    detailGrid->addWidget( mPhoneHome, 5, 1 );
    QLabel *otherPhoneLabel = new QLabel( tr( "Other phone: " ) );
    mPhoneOther = new QLineEdit();
    mPhoneOther->setObjectName( "phoneOther" );
    detailGrid->addWidget( otherPhoneLabel, 6, 0 );
    detailGrid->addWidget( mPhoneOther, 6, 1 );
    QLabel *faxLabel = new QLabel( tr( "Fax: " ) );
    mPhoneFax = new QLineEdit();
    mPhoneFax->setObjectName( "phoneFax" );
    detailGrid->addWidget( faxLabel, 7, 0 );
    detailGrid->addWidget( mPhoneFax, 7, 1 );
    QLabel *doNotCallLabel = new QLabel( tr( "Do not call: " ) );
    mDoNotCall = new QCheckBox( QString() );
    mDoNotCall->setObjectName( "doNotCall" );
    detailGrid->addWidget( doNotCallLabel, 8, 0 );
    detailGrid->addWidget( mDoNotCall, 8, 1 );
    QLabel *emailAddressesLabel = new QLabel( tr( "Email addresses: " ) );
    mEmail1 = new QLineEdit();
    mEmail1->setObjectName( "email1" );
    detailGrid->addWidget( emailAddressesLabel, 9, 0 );
    detailGrid->addWidget( mEmail1, 9, 1 );
    QLabel *fakedSpace = new QLabel( QString() );
    mEmail2 = new QLineEdit();
    mEmail2->setObjectName( "email2" );
    detailGrid->addWidget( fakedSpace, 10, 0 );
    detailGrid->addWidget( mEmail2, 10, 1 );

    return mOtherDetailsBox;
}

QGroupBox* LeadDetails::buildAddressesGroupBox()
{
    mAddressesBox = new QGroupBox;
    mAddressesBox->setMinimumWidth(250);

    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    mAddressesBox->setLayout( vLayout );
    mAddressesBox->setTitle( tr( "Addresses" ) );

    QLabel *primaryLabel = new QLabel( tr( "Primary address: " ) );
    mPrimaryAddressStreet = new QLineEdit();
    mPrimaryAddressStreet->setObjectName( "primaryAddressStreet" );
    detailGrid->addWidget( primaryLabel, 0, 0 );
    detailGrid->addWidget( mPrimaryAddressStreet, 0, 1 );
    QLabel *cityLabel = new QLabel( tr( "City: " ) );
    mPrimaryAddressCity = new QLineEdit();
    mPrimaryAddressCity->setObjectName( "primaryAddressCity" );
    detailGrid->addWidget( cityLabel, 1, 0 );
    detailGrid->addWidget( mPrimaryAddressCity, 1, 1 );
    QLabel *stateLabel = new QLabel( tr( "State: " ) );
    mPrimaryAddressState = new QLineEdit();
    mPrimaryAddressState->setObjectName( "primaryAddressState" );
    detailGrid->addWidget( stateLabel, 2, 0 );
    detailGrid->addWidget( mPrimaryAddressState, 2, 1 );
    QLabel *postalCodeLabel = new QLabel( tr( "Postal code: " ) );
    mPrimaryAddressPostalcode = new QLineEdit();
    mPrimaryAddressPostalcode->setObjectName( "primaryAddressPostalcode" );
    detailGrid->addWidget( postalCodeLabel, 3, 0 );
    detailGrid->addWidget( mPrimaryAddressPostalcode, 3, 1 );
    QLabel *countryLabel = new QLabel( tr( "Country: " ) );
    mPrimaryAddressCountry = new QLineEdit();
    mPrimaryAddressCountry->setObjectName( "primaryAddressCountry" );
    detailGrid->addWidget( countryLabel, 4, 0 );
    detailGrid->addWidget( mPrimaryAddressCountry, 4, 1 );
    QLabel *otherAddressLabel = new QLabel( tr( "Other address: " ) );
    mAltAddressStreet = new QLineEdit();
    mAltAddressStreet->setObjectName( "altAddressStreet" );
    detailGrid->addWidget( otherAddressLabel, 5, 0 );
    detailGrid->addWidget( mAltAddressStreet, 5, 1 );
    QLabel *otherCityLabel = new QLabel( tr( "City: " ) );
    mAltAddressCity = new QLineEdit();
    mAltAddressCity->setObjectName( "altAddressCity" );
    detailGrid->addWidget( otherCityLabel, 6, 0 );
    detailGrid->addWidget( mAltAddressCity, 6, 1 );
    QLabel *otherStateLabel = new QLabel( tr( "State: " ) );
    mAltAddressState = new QLineEdit();
    mAltAddressState->setObjectName( "altAddressState" );
    detailGrid->addWidget( otherStateLabel, 7, 0 );
    detailGrid->addWidget( mAltAddressState, 7, 1 );
    QLabel *otherPostalCodeLabel = new QLabel( tr( "Postal code: " ) );
    mAltAddressPostalcode = new QLineEdit();
    mAltAddressPostalcode->setObjectName( "altAddressPostalcode" );
    detailGrid->addWidget( otherPostalCodeLabel, 8, 0 );
    detailGrid->addWidget( mAltAddressPostalcode, 8, 1 );
    QLabel *otherCountryLabel = new QLabel( tr( "Country: " ) );
    mAltAddressCountry = new QLineEdit();
    mAltAddressCountry->setObjectName( "altAddressCountry" );
    detailGrid->addWidget( otherCountryLabel, 9, 0 );
    detailGrid->addWidget( mAltAddressCountry, 9, 1 );
    QLabel *copyFromPrimary = new QLabel( tr( "Copy primary: " ) );
    mCopyAddressFromPrimary = new QCheckBox();
    mCopyAddressFromPrimary->setObjectName("copyAddressFromPrimary" );
    detailGrid->addWidget( copyFromPrimary );
    detailGrid->addWidget( mCopyAddressFromPrimary );

    connect( mCopyAddressFromPrimary, SIGNAL( toggled( bool ) ),
             this, SLOT( slotCopyFromPrimary( bool ) ) );

    return mAddressesBox;
}

void LeadDetails::slotCopyFromPrimary( bool checked )
{
    if ( !checked )
        return;
    mAltAddressStreet->setText( mPrimaryAddressStreet->text() );
    mAltAddressCity->setText( mPrimaryAddressCity->text() );
    mAltAddressState->setText( mPrimaryAddressState->text() );
    mAltAddressPostalcode->setText( mPrimaryAddressPostalcode->text() );
    mAltAddressCountry->setText( mPrimaryAddressCountry->text() );
}

void LeadDetails::slotSetBirthDate()
{
    mBirthdate->setText( mCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mCalendarButton->calendarWidget()->setSelectedDate( QDate::currentDate() );
    mCalendarButton->calendarDialog()->close();
}

void LeadDetails::slotClearDate()
{
    mBirthdate->clear();
}

QStringList LeadDetails::sourceItems() const
{
    QStringList sources;
    sources << QString("") << QString( "Cold Call" )
            << QString( "Existing Customer" ) << QString( "Self Generated" )
            << QString( "Employee" ) << QString( "Partner" )
            << QString( "Public Relations" ) << QString( "Direct Mail" )
            << QString( "Conference" ) << QString( "Trade Show" )
            << QString( "Web Site" ) << QString( "Word of mouth" )
            << QString( "Email" ) << QString( "Campaign" )
            << QString( "Other" );
    return sources;
}

QStringList LeadDetails::salutationItems() const
{
    QStringList salutations;
    salutations << QString("") << QString( "Mr." )
            << QString( "Ms." ) << QString( "Mrs." )
            << QString( "Dr." ) << QString( "Prof." );
    return salutations;
}

QStringList LeadDetails::statusItems() const
{
    QStringList status;
    status << QString("") << QString( "New" )
           << QString( "Assigned" ) << QString( "In Process" )
           << QString( "Converted" ) << QString( "Recycled" )
           << QString( "Dead" );
    return status;
}
