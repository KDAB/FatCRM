#include "accountdetails.h"

#include <kdcrmdata/sugaraccount.h>

AccountDetails::AccountDetails( QWidget *parent )
    : Details( Account, parent )

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
    QGroupBox* detailsBox = new QGroupBox;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    detailsBox->setLayout( vLayout );
    detailsBox->setTitle( tr( "Details" ) );
    QLabel *nameLabel = new QLabel( tr( "Name:" ) );
    QLineEdit* name = new QLineEdit();
    name->setObjectName( "name" );
    detailGrid->addWidget( nameLabel, 0, 0 );
    detailGrid->addWidget( name, 0, 1 );
    QLabel *websiteLabel = new QLabel( tr( "Website: " ) );
    QLineEdit* website = new QLineEdit();
    website->setObjectName( "website" );
    detailGrid->addWidget( websiteLabel, 1, 0 );
    detailGrid->addWidget( website, 1, 1 );
    QLabel *tickerLabel = new QLabel( tr( "Ticker symbol: " ) );
    QLineEdit* tickerSymbol = new QLineEdit();
    tickerSymbol->setObjectName( "tickerSymbol" );
    detailGrid->addWidget( tickerLabel, 2, 0 );
    detailGrid->addWidget( tickerSymbol, 2, 1 );
    QLabel *memberOfLabel = new QLabel( tr( "Member of: " ) );
    QComboBox* parentName = new QComboBox();
    parentName->setObjectName( "parentName" );
    parentName->insertItem( 0, QString( "" ) );
    detailGrid->addWidget( memberOfLabel, 3, 0 );
    detailGrid->addWidget( parentName, 3, 1 );
    QLabel *ownerShipLabel = new QLabel( tr( "Ownership: " ) );
    QLineEdit* ownership = new QLineEdit();
    ownership->setObjectName( "ownership" );
    detailGrid->addWidget( ownerShipLabel, 4, 0 );
    detailGrid->addWidget( ownership, 4, 1 );
    QLabel *industryLabel = new QLabel( tr( "Industry: " ) );
    QComboBox* industry = new QComboBox();
    industry->setObjectName( "industry" );
    industry->addItems( industryItems() );
    detailGrid->addWidget( industryLabel, 5, 0 );
    detailGrid->addWidget( industry, 5, 1 );
    QLabel *typeLabel = new QLabel( tr( "Type: " ) );
    QComboBox* accountType = new QComboBox();
    accountType->setObjectName( "accountType" );
    accountType->addItems( typeItems() );
    detailGrid->addWidget( typeLabel, 6, 0 );
    detailGrid->addWidget( accountType, 6, 1 );
    QLabel *campaignLabel = new QLabel( tr( "Campaign: " ) );
    QComboBox* campaignName = new QComboBox();
    campaignName->setObjectName( "campaignName" );
    campaignName->insertItem( 0, QString( "" ) );
    detailGrid->addWidget( campaignLabel, 7, 0 );
    detailGrid->addWidget( campaignName, 7, 1 );
    QLabel *assignedToLabel = new QLabel( tr( "Assigned to: " ) );
    QComboBox* assignedUserName = new QComboBox();
    assignedUserName->setObjectName( "assignedUserName" );
    assignedUserName->insertItem( 0, QString( "" ) );
    detailGrid->addWidget( assignedToLabel, 8, 0 );
    detailGrid->addWidget( assignedUserName, 8, 1 );

    return detailsBox;
}

QGroupBox* AccountDetails::buildOtherDetailsGroupBox()
{
    QGroupBox* otherDetailsBox = new QGroupBox;

    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    otherDetailsBox->setLayout( vLayout );
    otherDetailsBox->setTitle( tr( "Other details" ) );

    QLabel *phoneOfficeLabel = new QLabel( tr( "Phone office: " ) );
    QLineEdit* phoneOffice = new QLineEdit();
    phoneOffice->setObjectName( "phoneOffice" );
    detailGrid->addWidget( phoneOfficeLabel, 0, 0 );
    detailGrid->addWidget( phoneOffice, 0, 1 );
    QLabel *phoneFaxLabel = new QLabel( tr( "Fax: " ) );
    QLineEdit* phoneFax = new QLineEdit();
    phoneFax->setObjectName( "phoneFax" );
    detailGrid->addWidget( phoneFaxLabel, 1, 0 );
    detailGrid->addWidget( phoneFax, 1, 1 );
    QLabel *phoneAltLabel = new QLabel( tr( "Other phone: " ) );
    QLineEdit* phoneAlternate = new QLineEdit();
    phoneAlternate->setObjectName( "phoneAlternate" );
    detailGrid->addWidget( phoneAltLabel, 2, 0 );
    detailGrid->addWidget( phoneAlternate, 2, 1 );
    QLabel *employeesLabel = new QLabel( tr( "Employees: " ) );
    QLineEdit* employees = new QLineEdit();
    employees->setObjectName( "employees" );
    detailGrid->addWidget( employeesLabel, 3, 0 );
    detailGrid->addWidget( employees, 3, 1 );
    QLabel *ratingLabel = new QLabel( tr( "Rating: " ) );
    QLineEdit* rating = new QLineEdit();
    rating->setObjectName( "rating" );
    detailGrid->addWidget( ratingLabel, 4, 0 );
    detailGrid->addWidget( rating, 4, 1 );
    QLabel *sicCodeLabel = new QLabel( tr( "SIC code: ") );
    QLineEdit* sicCode = new QLineEdit();
    sicCode->setObjectName( "sicCode" );
    detailGrid->addWidget( sicCodeLabel, 5, 0 );
    detailGrid->addWidget( sicCode, 5, 1 );
    QLabel *annualRevenueLabel = new QLabel( tr( "Annual revenue: " ) );
    QLineEdit* annualRevenue = new QLineEdit();
    annualRevenue->setObjectName( "annualRevenue" );
    detailGrid->addWidget( annualRevenueLabel, 6, 0 );
    detailGrid->addWidget( annualRevenue, 6, 1 );
    QLabel *emailLabel = new QLabel( tr( "Email address: " ) );
    QLineEdit* email1 = new QLineEdit();
    email1->setObjectName( "email1" );
    detailGrid->addWidget( emailLabel, 7, 0 );
    detailGrid->addWidget( email1, 7, 1 );

    return otherDetailsBox;
}

QGroupBox* AccountDetails::buildAddressesGroupBox()
{
    QGroupBox* addressesBox = new QGroupBox;

    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    addressesBox->setLayout( vLayout );
    addressesBox->setTitle( tr( "Addresses" ) );

    QLabel *billingStreetLabel = new QLabel( tr( "Billing address: " ) );
    QLineEdit* billingAddressStreet = new QLineEdit();
    billingAddressStreet->setObjectName( "billingAddressStreet" );
    detailGrid->addWidget( billingStreetLabel, 0, 0 );
    detailGrid->addWidget( billingAddressStreet, 0, 1 );
    QLabel *billingCityLabel = new QLabel( tr( "City: " ) );
    QLineEdit* billingAddressCity = new QLineEdit();
    billingAddressCity->setObjectName( "billingAddressCity" );
    detailGrid->addWidget( billingCityLabel, 1, 0 );
    detailGrid->addWidget( billingAddressCity, 1, 1 );
    QLabel *billingStateLabel = new QLabel( tr( "State: " ) );
    QLineEdit* billingAddressState = new QLineEdit();
    billingAddressState->setObjectName( "billingAddressState" );
    detailGrid->addWidget( billingStateLabel, 2, 0 );
    detailGrid->addWidget( billingAddressState, 2, 1 );
    QLabel *billingPostalCodeLabel = new QLabel( tr( "Postal code: " ) );
    QLineEdit* billingAddressPostalCode = new QLineEdit();
    billingAddressPostalCode->setObjectName( "billingAddressPostalcode" );
    detailGrid->addWidget( billingPostalCodeLabel, 3, 0 );
    detailGrid->addWidget( billingAddressPostalCode, 3, 1 );
    QLabel *billingCountryLabel = new QLabel( tr( "Country: " ) );
    QLineEdit* billingAddressCountry = new QLineEdit();
    billingAddressCountry->setObjectName( "billingAddressCountry" );
    detailGrid->addWidget( billingCountryLabel, 4, 0 );
    detailGrid->addWidget( billingAddressCountry, 4, 1 );
    QLabel *shippingStreetLabel = new QLabel( tr( "Shipping address: " ) );
    QLineEdit* shippingAddressStreet = new QLineEdit();
    shippingAddressStreet->setObjectName( "shippingAddressStreet" );
    detailGrid->addWidget( shippingStreetLabel, 5, 0 );
    detailGrid->addWidget( shippingAddressStreet, 5, 1 );
    QLabel *shippingCityLabel = new QLabel( tr( "City: " ) );
    QLineEdit* shippingAddressCity = new QLineEdit();
    shippingAddressCity->setObjectName( "shippingAddressCity" );
    detailGrid->addWidget( shippingCityLabel, 6, 0 );
    detailGrid->addWidget( shippingAddressCity, 6, 1 );
    QLabel *shippingStateLabel = new QLabel( tr( "State: " ) );
    QLineEdit* shippingAddressState = new QLineEdit();
    shippingAddressState->setObjectName( "shippingAddressState" );
    detailGrid->addWidget( shippingStateLabel, 7, 0 );
    detailGrid->addWidget( shippingAddressState, 7, 1 );
    QLabel *shippingPostalCodeLabel = new QLabel( tr( "Postal code: " ) );
    QLineEdit* shippingAddressPostalCode = new QLineEdit();
    shippingAddressPostalCode->setObjectName( "shippingAddressPostalcode" );
    detailGrid->addWidget( shippingPostalCodeLabel, 8, 0 );
    detailGrid->addWidget( shippingAddressPostalCode, 8, 1 );
    QLabel *shippingCountryLabel = new QLabel( tr( "Country: " ) );
    QLineEdit* shippingAddressCountry = new QLineEdit();
    shippingAddressCountry->setObjectName( "shippingAddressCountry" );
    detailGrid->addWidget( shippingCountryLabel, 9, 0 );
    detailGrid->addWidget( shippingAddressCountry, 9, 1 );

    return addressesBox;
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

QMap<QString, QString> AccountDetails::data( const Akonadi::Item &item ) const
{
    SugarAccount account = item.payload<SugarAccount>();
    return account.data();
}

void AccountDetails::updateItem( Akonadi::Item &item, const QMap<QString, QString> &data ) const
{
    SugarAccount account;
    if ( item.hasPayload<SugarAccount>() ) {
        account = item.payload<SugarAccount>();
    }
    account.setData( data );

    item.setMimeType( SugarAccount::mimeType() );
    item.setPayload<SugarAccount>( account );
}
