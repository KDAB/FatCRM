#include "accountdetails.h"

AccountDetails::AccountDetails( QWidget *parent )
    : Details( parent )

{
    initialize();
}

AccountDetails::~AccountDetails()
{
}

void AccountDetails::initialize()
{
    setObjectName( "accountDetails" );
    QHBoxLayout *hLayout = new QHBoxLayout;
    // build the group boxes
    hLayout->addWidget( buildDetailsGroupBox() );
    hLayout->addWidget( buildOtherDetailsGroupBox() );
    hLayout->addWidget( buildAddressesGroupBox() );
    setLayout( hLayout );
}

QGroupBox* AccountDetails::buildDetailsGroupBox()
{
    mDetailsBox = new QGroupBox;

    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    mDetailsBox->setLayout( vLayout );
    mDetailsBox->setTitle( tr( "Details" ) );
    QLabel *nameLabel = new QLabel( tr( "Name:" ) );
    mName = new QLineEdit();
    mName->setObjectName( "name" );
    detailGrid->addWidget( nameLabel, 0, 0 );
    detailGrid->addWidget( mName, 0, 1 );
    QLabel *websiteLabel = new QLabel( tr( "Website: " ) );
    mWebsite = new QLineEdit();
    mWebsite->setObjectName( "website" );
    detailGrid->addWidget( websiteLabel, 1, 0 );
    detailGrid->addWidget( mWebsite, 1, 1 );
    QLabel *tyckerLabel = new QLabel( tr( "Ticker symbol: " ) );
    mTyckerSymbol = new QLineEdit();
    mTyckerSymbol->setObjectName( "tyckerSymbol" );
    detailGrid->addWidget( tyckerLabel, 2, 0 );
    detailGrid->addWidget( mTyckerSymbol, 2, 1 );
    QLabel *memberOfLabel = new QLabel( tr( "Member of: " ) );
    mParentName = new QComboBox();
    mParentName->setObjectName( "parentName" );
    mParentName->insertItem( 0, QString( "" ) );
    detailGrid->addWidget( memberOfLabel, 3, 0 );
    detailGrid->addWidget( mParentName, 3, 1 );
    QLabel *ownerShipLabel = new QLabel( tr( "Ownership: " ) );
    mOwnership = new QLineEdit();
    mOwnership->setObjectName( "ownership" );
    detailGrid->addWidget( ownerShipLabel, 4, 0 );
    detailGrid->addWidget( mOwnership, 4, 1 );
    QLabel *industryLabel = new QLabel( tr( "Industry: " ) );
    mIndustry = new QComboBox();
    mIndustry->setObjectName( "industry" );
    mIndustry->addItems( industryItems() );
    detailGrid->addWidget( industryLabel, 5, 0 );
    detailGrid->addWidget( mIndustry, 5, 1 );
    QLabel *typeLabel = new QLabel( tr( "Type: " ) );
    mAccountType = new QComboBox();
    mAccountType->setObjectName( "accountType" );
    mAccountType->addItems( typeItems() );
    detailGrid->addWidget( typeLabel, 6, 0 );
    detailGrid->addWidget( mAccountType, 6, 1 );
    QLabel *campaignLabel = new QLabel( tr( "Campaign: " ) );
    mCampaignName = new QComboBox();
    mCampaignName->setObjectName( "campaignName" );
    mCampaignName->insertItem( 0, QString( "" ) );
    detailGrid->addWidget( campaignLabel, 7, 0 );
    detailGrid->addWidget( mCampaignName, 7, 1 );
    QLabel *assignedToLabel = new QLabel( tr( "Assigned to: " ) );
    mAssignedUserName = new QComboBox();
    mAssignedUserName->setObjectName( "assignedUserName" );
    mAssignedUserName->insertItem( 0, QString( "" ) );
    detailGrid->addWidget( assignedToLabel, 8, 0 );
    detailGrid->addWidget( mAssignedUserName, 8, 1 );

    return mDetailsBox;
}

QGroupBox* AccountDetails::buildOtherDetailsGroupBox()
{
    mOtherDetailsBox = new QGroupBox;

    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    mOtherDetailsBox->setLayout( vLayout );
    mOtherDetailsBox->setTitle( tr( "Other details" ) );

    QLabel *phoneOfficeLabel = new QLabel( tr( "Phone office: " ) );
    mPhoneOffice = new QLineEdit();
    mPhoneOffice->setObjectName( "phoneOffice" );
    detailGrid->addWidget( phoneOfficeLabel, 0, 0 );
    detailGrid->addWidget( mPhoneOffice, 0, 1 );
    QLabel *phoneFaxLabel = new QLabel( tr( "Fax: " ) );
    mPhoneFax = new QLineEdit();
    mPhoneFax->setObjectName( "phoneFax" );
    detailGrid->addWidget( phoneFaxLabel, 1, 0 );
    detailGrid->addWidget( mPhoneFax, 1, 1 );
    QLabel *phoneAltLabel = new QLabel( tr( "Other phone: " ) );
    mPhoneAlternate = new QLineEdit();
    mPhoneAlternate->setObjectName( "phoneAlternate" );
    detailGrid->addWidget( phoneAltLabel, 2, 0 );
    detailGrid->addWidget( mPhoneAlternate, 2, 1 );
    QLabel *employeesLabel = new QLabel( tr( "Employees: " ) );
    mEmployees = new QLineEdit();
    mEmployees->setObjectName( "employees" );
    detailGrid->addWidget( employeesLabel, 3, 0 );
    detailGrid->addWidget( mEmployees, 3, 1 );
    QLabel *ratingLabel = new QLabel( tr( "Rating: " ) );
    mRating = new QLineEdit();
    mRating->setObjectName( "rating" );
    detailGrid->addWidget( ratingLabel, 4, 0 );
    detailGrid->addWidget( mRating, 4, 1 );
    QLabel *sicCodeLabel = new QLabel( tr( "SIC code: ") );
    mSicCode = new QLineEdit();
    mSicCode->setObjectName( "sicCode" );
    detailGrid->addWidget( sicCodeLabel, 5, 0 );
    detailGrid->addWidget( mSicCode, 5, 1 );
    QLabel *annualRevenueLabel = new QLabel( tr( "Annual revenue: " ) );
    mAnnualRevenue = new QLineEdit();
    mAnnualRevenue->setObjectName( "annualRevenue" );
    detailGrid->addWidget( annualRevenueLabel, 6, 0 );
    detailGrid->addWidget( mAnnualRevenue, 6, 1 );
    QLabel *emailLabel = new QLabel( tr( "Email address: " ) );
    mEmail1 = new QLineEdit();
    mEmail1->setObjectName( "email1" );
    detailGrid->addWidget( emailLabel, 7, 0 );
    detailGrid->addWidget( mEmail1, 7, 1 );

    return mOtherDetailsBox;
}

QGroupBox* AccountDetails::buildAddressesGroupBox()
{
    mAddressesBox = new QGroupBox;

    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    mAddressesBox->setLayout( vLayout );
    mAddressesBox->setTitle( tr( "Addresses" ) );

    QLabel *billingStreetLabel = new QLabel( tr( "Billing address: " ) );
    mBillingAddressStreet = new QLineEdit();
    mBillingAddressStreet->setObjectName( "billingAddressStreet" );
    detailGrid->addWidget( billingStreetLabel, 0, 0 );
    detailGrid->addWidget( mBillingAddressStreet, 0, 1 );
    QLabel *billingCityLabel = new QLabel( tr( "City: " ) );
    mBillingAddressCity = new QLineEdit();
    mBillingAddressCity->setObjectName( "billingAddressCity" );
    detailGrid->addWidget( billingCityLabel, 1, 0 );
    detailGrid->addWidget( mBillingAddressCity, 1, 1 );
    QLabel *billingStateLabel = new QLabel( tr( "State: " ) );
    mBillingAddressState = new QLineEdit();
    mBillingAddressState->setObjectName( "billingAddressState" );
    detailGrid->addWidget( billingStateLabel, 2, 0 );
    detailGrid->addWidget( mBillingAddressState, 2, 1 );
    QLabel *billingPostalCodeLabel = new QLabel( tr( "Postal code: " ) );
    mBillingAddressPostalCode = new QLineEdit();
    mBillingAddressPostalCode->setObjectName( "billingAddressPostalcode" );
    detailGrid->addWidget( billingPostalCodeLabel, 3, 0 );
    detailGrid->addWidget( mBillingAddressPostalCode, 3, 1 );
    QLabel *billingCountryLabel = new QLabel( tr( "Country: " ) );
    mBillingAddressCountry = new QLineEdit();
    mBillingAddressCountry->setObjectName( "billingAddressCountry" );
    detailGrid->addWidget( billingCountryLabel, 4, 0 );
    detailGrid->addWidget( mBillingAddressCountry, 4, 1 );
    QLabel *shippingStreetLabel = new QLabel( tr( "Shipping address: " ) );
    mShippingAddressStreet = new QLineEdit();
    mShippingAddressStreet->setObjectName( "shippingAddressStreet" );
    detailGrid->addWidget( shippingStreetLabel, 5, 0 );
    detailGrid->addWidget( mShippingAddressStreet, 5, 1 );
    QLabel *shippingCityLabel = new QLabel( tr( "City: " ) );
    mShippingAddressCity = new QLineEdit();
    mShippingAddressCity->setObjectName( "shippingAddressCity" );
    detailGrid->addWidget( shippingCityLabel, 6, 0 );
    detailGrid->addWidget( mShippingAddressCity, 6, 1 );
    QLabel *shippingStateLabel = new QLabel( tr( "State: " ) );
    mShippingAddressState = new QLineEdit();
    mShippingAddressState->setObjectName( "shippingAddressState" );
    detailGrid->addWidget( shippingStateLabel, 7, 0 );
    detailGrid->addWidget( mShippingAddressState, 7, 1 );
    QLabel *shippingPostalCodeLabel = new QLabel( tr( "Postal code: " ) );
    mShippingAddressPostalCode = new QLineEdit();
    mShippingAddressPostalCode->setObjectName( "shippingAddressPostalcode" );
    detailGrid->addWidget( shippingPostalCodeLabel, 8, 0 );
    detailGrid->addWidget( mShippingAddressPostalCode, 8, 1 );
    QLabel *shippingCountryLabel = new QLabel( tr( "Country: " ) );
    mShippingAddressCountry = new QLineEdit();
    mShippingAddressCountry->setObjectName( "shippingAddressCountry" );
    detailGrid->addWidget( shippingCountryLabel, 9, 0 );
    detailGrid->addWidget( mShippingAddressCountry, 9, 1 );

    return mAddressesBox;
}


QStringList AccountDetails::typeItems() const
{
    QStringList types;
    types << QString("") << QString( "Analyst" ) << QString("Competitor" )
          << QString("Customer" ) << QString("Integrator" )
          << QString("Investor" ) << QString("Partner" )
          << QString("Press" ) << QString("Prospect" )
          << QString("Reseller" ) << QString("Other" );
    return types;
}
