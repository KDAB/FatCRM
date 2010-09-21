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
    QGroupBox* detailsBox = new QGroupBox;

    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    detailsBox->setLayout( vLayout );
    detailsBox->setTitle( tr( "Details" ) );

    QLabel *fakedSpace = new QLabel( QString() );
    QComboBox* salutation = new QComboBox();
    salutation->setObjectName( "salutation" );
    salutation->addItems( salutationItems() );
    detailGrid->addWidget( fakedSpace, 0, 0 );
    detailGrid->addWidget( salutation, 0, 1 );
    QLabel *firstNameLabel = new QLabel( tr( "First name:" ) );
    QLineEdit* firstName = new QLineEdit();
    firstName->setObjectName( "firstName" );
    detailGrid->addWidget( firstNameLabel, 0, 0 );
    detailGrid->addWidget( firstName, 0, 1 );
    QLabel *lastNameLabel = new QLabel( tr( "Last name: " ) );
    QLineEdit* lastName = new QLineEdit();
    lastName->setObjectName( "lastName" );
    detailGrid->addWidget( lastNameLabel, 1, 0 );
    detailGrid->addWidget( lastName, 1, 1 );
    QLabel *titleLabel = new QLabel( tr( "Job title: " ) );
    QLineEdit* title = new QLineEdit();
    title->setObjectName( "title" );
    detailGrid->addWidget( titleLabel, 2, 0 );
    detailGrid->addWidget( title, 2, 1 );
    QLabel *departmentLabel = new QLabel( tr( "Department: " ) );
    QLineEdit* department = new QLineEdit();
    department->setObjectName( "department" );
    detailGrid->addWidget( departmentLabel, 3, 0 );
    detailGrid->addWidget( department, 3, 1 );
    QLabel *accountNameLabel = new QLabel( tr( "Account name: " ) );
    QComboBox* accountName = new QComboBox();
    accountName->setObjectName( "accountName" );
    accountName->addItem( 0,  QString( "" ) );
    detailGrid->addWidget( accountNameLabel, 4, 0 );
    detailGrid->addWidget( accountName, 4, 1 );
    QLabel *leadSourceLabel = new QLabel( tr( "Lead source: " ) );
    QComboBox* leadSource = new QComboBox();
    leadSource->setObjectName( "leadSource" );
    leadSource->addItems( sourceItems() );
    detailGrid->addWidget( leadSourceLabel, 5, 0 );
    detailGrid->addWidget( leadSource, 5, 1 );
    QLabel *campaignLabel = new QLabel( tr( "Campaign: " ) );
    QComboBox* campaign = new QComboBox();
    campaign->setObjectName( "campaign" );
    campaign->addItem( 0, QString( "" ) );
    detailGrid->addWidget( campaignLabel, 6, 0 );
    detailGrid->addWidget( campaign, 6, 1 );
    QLabel *assignedToLabel = new QLabel( tr( "Assigned to: " ) );
    QComboBox* assignedTo = new QComboBox();
    assignedTo->setObjectName( "assignedTo" );
    assignedTo->addItem( 0, QString( "" ) );
    detailGrid->addWidget( assignedToLabel, 7, 0 );
    detailGrid->addWidget( assignedTo, 7, 1 );
    QLabel *reportsToLabel = new QLabel( tr( "Reports to: " ) );
    QComboBox* reportsTo = new QComboBox();
    reportsTo->setObjectName( "reportsTo" );
    reportsTo->addItem( 0, QString( "" ) );
    detailGrid->addWidget( reportsToLabel, 8, 0 );
    detailGrid->addWidget( reportsTo, 8, 1 );
    QLabel *primaryEmailLabel = new QLabel( tr( "Primary email: " ) );
    QLineEdit* primaryEmail = new QLineEdit();
    primaryEmail->setObjectName( "primaryEmail" );
    detailGrid->addWidget( primaryEmailLabel, 9, 0 );
    detailGrid->addWidget( primaryEmail, 9, 1 );

    return detailsBox;
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

    QGroupBox* otherDetailsBox = new QGroupBox;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    otherDetailsBox->setLayout( vLayout );
    otherDetailsBox->setTitle( tr( "Other details" ) );

    QLabel *officePhoneLabel = new QLabel( tr( "Office phone: " ) );
    QLineEdit* officePhone = new QLineEdit();
    officePhone->setObjectName( "officePhone" );
    detailGrid->addWidget( officePhoneLabel, 0, 0 );
    detailGrid->addWidget( officePhone, 0, 1 );
    QLabel *mobileLabel = new QLabel( tr( "Mobile: " ) );
    QLineEdit* mobilePhone = new QLineEdit();
    mobilePhone->setObjectName( "mobilePhone" );
    detailGrid->addWidget( mobileLabel, 1, 0 );
    detailGrid->addWidget( mobilePhone, 1, 1 );
    QLabel *homePhoneLabel = new QLabel( tr( "Home phone: " ) );
    QLineEdit* homePhone = new QLineEdit();
    homePhone->setObjectName( "homePhone" );
    detailGrid->addWidget( homePhoneLabel, 2, 0 );
    detailGrid->addWidget( homePhone, 2, 1 );
    QLabel *otherPhoneLabel = new QLabel( tr( "Other Phone: " ) );
    QLineEdit* otherPhone = new QLineEdit();
    otherPhone->setObjectName( "otherPhone" );
    detailGrid->addWidget( otherPhoneLabel, 3, 0 );
    detailGrid->addWidget( otherPhone, 3, 1 );
    QLabel *faxLabel = new QLabel( tr( "Fax: " ) );
    QLineEdit* fax = new QLineEdit();
    fax->setObjectName( "fax" );
    detailGrid->addWidget( faxLabel, 4, 0 );
    detailGrid->addWidget( fax, 4, 1 );
    QLabel *birthDateLabel = new QLabel( tr( "Birthdate: ") );
    detailGrid->addWidget( birthDateLabel, 5, 0 );
    detailGrid->addWidget( calendarWidget, 5, 1 );
    QLabel *assistantLabel = new QLabel( tr( "Assistant: " ) );
    QLineEdit* assistant = new QLineEdit();
    assistant->setObjectName( "assistant" );
    detailGrid->addWidget( assistantLabel, 6, 0 );
    detailGrid->addWidget( assistant, 6, 1 );
    QLabel *assistantPhoneLabel = new QLabel( tr( "Assistant phone: " ) );
    QLineEdit* assistantPhone = new QLineEdit();
    assistantPhone->setObjectName( "assistantPhone" );
    detailGrid->addWidget( assistantPhoneLabel, 7, 0 );
    detailGrid->addWidget( assistantPhone, 7, 1 );
    QLabel *doNotCallLabel = new QLabel( tr( "Do not call: " ) );
    QCheckBox* doNotCall = new QCheckBox( QString() );
    doNotCall->setObjectName( "doNotCall" );
    detailGrid->addWidget( doNotCallLabel, 8, 0 );
    detailGrid->addWidget( doNotCall, 8, 1 );

    return otherDetailsBox;
}

QGroupBox* ContactDetails::buildAddressesGroupBox()
{
    QGroupBox* addressesBox = new QGroupBox;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    addressesBox->setLayout( vLayout );
    addressesBox->setTitle( tr( "Addresses" ) );

    QLabel *primaryLabel = new QLabel( tr( "Primary address: " ) );
    QLineEdit* primaryAddress = new QLineEdit();
    primaryAddress->setObjectName( "primaryAddress" );
    detailGrid->addWidget( primaryLabel, 0, 0 );
    detailGrid->addWidget( primaryAddress, 0, 1 );
    QLabel *cityLabel = new QLabel( tr( "City: " ) );
    QLineEdit* city = new QLineEdit();
    city->setObjectName( "city" );
    detailGrid->addWidget( cityLabel, 1, 0 );
    detailGrid->addWidget( city, 1, 1 );
    QLabel *stateLabel = new QLabel( tr( "State: " ) );
    QLineEdit* state = new QLineEdit();
    state->setObjectName( "state" );
    detailGrid->addWidget( stateLabel, 2, 0 );
    detailGrid->addWidget( state, 2, 1 );
    QLabel *postalCodeLabel = new QLabel( tr( "Postal code: " ) );
    QLineEdit* postalCode = new QLineEdit();
    postalCode->setObjectName( "postalCode" );
    detailGrid->addWidget( postalCodeLabel, 3, 0 );
    detailGrid->addWidget( postalCode, 3, 1 );
    QLabel *countryLabel = new QLabel( tr( "Country: " ) );
    QLineEdit* country = new QLineEdit();
    country->setObjectName( "country" );
    detailGrid->addWidget( countryLabel, 4, 0 );
    detailGrid->addWidget( country, 4, 1 );
    QLabel *otherAddressLabel = new QLabel( tr( "Other address: " ) );
    QLineEdit* otherAddress = new QLineEdit();
    otherAddress->setObjectName( "otherAddress" );
    detailGrid->addWidget( otherAddressLabel, 5, 0 );
    detailGrid->addWidget( otherAddress, 5, 1 );
    QLabel *otherCityLabel = new QLabel( tr( "City: " ) );
    QLineEdit* otherCity = new QLineEdit();
    otherCity->setObjectName( "otherCity" );
    detailGrid->addWidget( otherCityLabel, 6, 0 );
    detailGrid->addWidget( otherCity, 6, 1 );
    QLabel *otherStateLabel = new QLabel( tr( "State: " ) );
    QLineEdit* otherState = new QLineEdit();
    otherState->setObjectName( "otherState" );
    detailGrid->addWidget( otherStateLabel, 7, 0 );
    detailGrid->addWidget( otherState, 7, 1 );
    QLabel *otherPostalCodeLabel = new QLabel( tr( "Postal code: " ) );
    QLineEdit* otherPostalCode = new QLineEdit();
    otherPostalCode->setObjectName( "otherPostalCode" );
    detailGrid->addWidget( otherPostalCodeLabel, 8, 0 );
    detailGrid->addWidget( otherPostalCode, 8, 1 );
    QLabel *otherCountryLabel = new QLabel( tr( "Country: " ) );
    QLineEdit* otherCountry = new QLineEdit();
    otherCountry->setObjectName( "otherCountry" );
    detailGrid->addWidget( otherCountryLabel, 9, 0 );
    detailGrid->addWidget( otherCountry, 9, 1 );

    return addressesBox;
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

QMap<QString, QString> ContactDetails::data( const Akonadi::Item &item ) const
{
    KABC::Addressee contact = item.payload<KABC::Addressee>();
    return contactData( contact );
}

QMap<QString,QString> ContactDetails::contactData( const KABC::Addressee &addressee ) const
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
    data["doNotCall"] = addressee.custom( "FATCRM", "X-DoNotCall" );
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
