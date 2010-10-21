#include "leaddetails.h"

#include <kdcrmdata/sugarlead.h>

LeadDetails::LeadDetails( QWidget *parent )
    : Details( Lead, parent )

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

    QGroupBox* detailsBox = new QGroupBox;

    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    detailsBox->setLayout( vLayout );
    detailsBox->setTitle( tr( "Details" ) );

    QLabel *leadSourceLabel = new QLabel( tr("Lead source: "  ) );
    QComboBox* leadSource = new QComboBox();
    leadSource->setObjectName( "leadSource" );
    leadSource->addItems( sourceItems() );
    detailGrid->addWidget( leadSourceLabel, 0, 0 );
    detailGrid->addWidget( leadSource, 0, 1 );
    QLabel *descriptionLabel = new QLabel( tr( "Lead source description: " ) );
    QTextEdit* leadSourceDescription = new QTextEdit();
    leadSourceDescription->setObjectName( "leadSourceDescription" );
    detailGrid->addWidget( descriptionLabel, 1, 0 );
    detailGrid->addWidget( leadSourceDescription, 1, 1 );
    QLabel *campaignLabel = new QLabel( tr( "Campaign: " ) );
    QComboBox* campaignName = new QComboBox();
    campaignName->setObjectName( "campaignName" );
    campaignName->addItem( 0, QString( "" ) );
    detailGrid->addWidget( campaignLabel, 2, 0 );
    detailGrid->addWidget( campaignName, 2, 1 );
    QLabel *referredByLabel = new QLabel( tr( "Referred by: " ) );
    QLineEdit* referedBy = new QLineEdit();
    referedBy->setObjectName( "referedBy" );
    detailGrid->addWidget( referredByLabel, 3, 0 );
    detailGrid->addWidget( referedBy, 3, 1 );
    QLabel *firstNameLabel = new QLabel( tr( "First name: " ) );
    QWidget *salutationWidget = new QWidget();
    QHBoxLayout *widgetLayout = new QHBoxLayout;
    salutationWidget->setLayout( widgetLayout );
    QComboBox* salutation = new QComboBox();
    salutation->setObjectName( "salutation" );
    salutation->addItems( salutationItems() );
    QLineEdit* firstName = new QLineEdit();
    firstName->setObjectName( "firstName" );
    widgetLayout->addWidget( salutation );
    widgetLayout->addWidget( firstName );
    detailGrid->addWidget( firstNameLabel, 4, 0 );
    detailGrid->addWidget( salutationWidget, 4, 1 );
    QLabel *lastNameLabel = new QLabel( tr( "Last name: " ) );
    QLineEdit* lastName = new QLineEdit();
    lastName->setObjectName( "lastName" );
    detailGrid->addWidget( lastNameLabel, 5, 0 );
    detailGrid->addWidget( lastName, 5, 1 );
    QLabel *birthDateLabel = new QLabel( tr( "Birthdate: " ) );
    detailGrid->addWidget( birthDateLabel, 6, 0 );
    detailGrid->addWidget( calendarWidget, 6, 1 );
    QLabel *accountNameLabel = new QLabel( tr( "Account name: " ) );
    QLineEdit* accountName = new QLineEdit();
    accountName->setObjectName( "accountName" );
    detailGrid->addWidget( accountNameLabel, 7, 0 );
    detailGrid->addWidget( accountName, 7, 1 );
    QLabel *titleLabel = new QLabel( tr( "Title: " ) );
    QLineEdit* title = new QLineEdit();
    title->setObjectName( "title" );
    detailGrid->addWidget( titleLabel, 8, 0 );
    detailGrid->addWidget( title, 8, 1 );
    QLabel *departmentLabel = new QLabel( tr( "Department: " ) );
    QLineEdit* department = new QLineEdit();
    department->setObjectName( "department" );
    detailGrid->addWidget( departmentLabel, 9, 0 );
    detailGrid->addWidget( department, 9, 1 );
    QLabel *assignedUserLabel = new QLabel( tr( "Assigned to: " ) );
    QComboBox* assignedUserName = new QComboBox();
    assignedUserName->setObjectName( "assignedUserName" );
    assignedUserName->addItem( 0, QString() );
    detailGrid->addWidget( assignedUserLabel, 10, 0 );
    detailGrid->addWidget( assignedUserName, 10, 1 );

    return detailsBox;
}

QGroupBox* LeadDetails::buildOtherDetailsGroupBox()
{

    QGroupBox* otherDetailsBox = new QGroupBox;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    otherDetailsBox->setLayout( vLayout );
    otherDetailsBox->setTitle( tr( "Other details" ) );

    QLabel *statusLabel = new QLabel( tr( "Status: " ) );
    QComboBox* status = new QComboBox();
    status->setObjectName( "status" );
    status->addItems( statusItems() );
    detailGrid->addWidget( statusLabel, 0, 0 );
    detailGrid->addWidget( status, 0, 1 );
    QLabel *descriptionLabel = new QLabel( tr( "Status description: " ) );
    QTextEdit* statusDescription = new QTextEdit();
    statusDescription->setObjectName( "statusDescription" );
    detailGrid->addWidget( descriptionLabel, 1, 0 );
    detailGrid->addWidget( statusDescription, 1, 1 );
    QLabel *opportunityAmountLabel = new QLabel( tr( "Opportunity amount: " ) );
    QLineEdit* opportunityAmount = new QLineEdit();
    opportunityAmount->setObjectName( "opportunityAmount" );
    detailGrid->addWidget( opportunityAmountLabel, 2, 0 );
    detailGrid->addWidget( opportunityAmount, 2, 1 );
    QLabel *officePhoneLabel = new QLabel( tr( "Office phone: " ) );
    QLineEdit* phoneWork = new QLineEdit();
    phoneWork->setObjectName( "phoneWork" );
    detailGrid->addWidget( officePhoneLabel, 3, 0 );
    detailGrid->addWidget( phoneWork, 3, 1 );
    QLabel *mobileLabel = new QLabel( tr( "Mobile: " ) );
    QLineEdit* phoneMobile = new QLineEdit();
    phoneMobile->setObjectName( "phoneMobile" );
    detailGrid->addWidget( mobileLabel, 4, 0 );
    detailGrid->addWidget( phoneMobile, 4, 1 );
    QLabel *homeLabel = new QLabel( tr( "Home phone: ") );
    QLineEdit* phoneHome = new QLineEdit();
    phoneHome->setObjectName( "phoneHome" );
    detailGrid->addWidget( homeLabel, 5, 0 );
    detailGrid->addWidget( phoneHome, 5, 1 );
    QLabel *otherPhoneLabel = new QLabel( tr( "Other phone: " ) );
    QLineEdit* phoneOther = new QLineEdit();
    phoneOther->setObjectName( "phoneOther" );
    detailGrid->addWidget( otherPhoneLabel, 6, 0 );
    detailGrid->addWidget( phoneOther, 6, 1 );
    QLabel *faxLabel = new QLabel( tr( "Fax: " ) );
    QLineEdit* phoneFax = new QLineEdit();
    phoneFax->setObjectName( "phoneFax" );
    detailGrid->addWidget( faxLabel, 7, 0 );
    detailGrid->addWidget( phoneFax, 7, 1 );
    QLabel *doNotCallLabel = new QLabel( tr( "Do not call: " ) );
    QCheckBox* doNotCall = new QCheckBox( QString() );
    doNotCall->setObjectName( "doNotCall" );
    detailGrid->addWidget( doNotCallLabel, 8, 0 );
    detailGrid->addWidget( doNotCall, 8, 1 );
    QLabel *emailAddressesLabel = new QLabel( tr( "Email addresses: " ) );
    QLineEdit* email1 = new QLineEdit();
    email1->setObjectName( "email1" );
    detailGrid->addWidget( emailAddressesLabel, 9, 0 );
    detailGrid->addWidget( email1, 9, 1 );
    QLabel *fakedSpace = new QLabel( QString() );
    QLineEdit* email2 = new QLineEdit();
    email2->setObjectName( "email2" );
    detailGrid->addWidget( fakedSpace, 10, 0 );
    detailGrid->addWidget( email2, 10, 1 );

    return otherDetailsBox;
}

QGroupBox* LeadDetails::buildAddressesGroupBox()
{
    QGroupBox* addressesBox = new QGroupBox;
    addressesBox->setMinimumWidth(250);

    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    addressesBox->setLayout( vLayout );
    addressesBox->setTitle( tr( "Addresses" ) );

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

    return addressesBox;
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

QStringList LeadDetails::statusItems() const
{
    QStringList status;
    status << QString("") << QString( "New" )
           << QString( "Assigned" ) << QString( "In Process" )
           << QString( "Converted" ) << QString( "Recycled" )
           << QString( "Dead" );
    return status;
}

QMap<QString, QString> LeadDetails::data( const Akonadi::Item &item ) const
{
    SugarLead lead = item.payload<SugarLead>();
    return lead.data();
}

void LeadDetails::updateItem( Akonadi::Item &item, const QMap<QString, QString> &data ) const
{
    SugarLead lead;
    if ( item.hasPayload<SugarLead>() ) {
        lead = item.payload<SugarLead>();
    }
    lead.setData( data );

    item.setMimeType( SugarLead::mimeType() );
    item.setPayload<SugarLead>( lead );
}
