#include "contactdetails.h"

#include <akonadi/item.h>

#include <kabc/addressee.h>
#include <kabc/address.h>

#include <QMouseEvent>

using namespace Akonadi;


EditCalendarButton::EditCalendarButton( QWidget *parent )
    : QToolButton( parent ), mCalendar(new QCalendarWidget())
{
    setText( tr( "&Edit" ) );
}

EditCalendarButton::~EditCalendarButton()
{
    delete mCalendar;
}


void EditCalendarButton::mousePressEvent( QMouseEvent* e )
{
    if ( mCalendar->isVisible() )
        mCalendar->hide();
    else {
        mCalendar->move( e->globalPos() );
        mCalendar->show();
        mCalendar->raise();
    }
}

ContactDetails::ContactDetails( QWidget *parent )
    : QWidget( parent )

{
    mUi.setupUi( this );
    initialize();
}

ContactDetails::~ContactDetails()
{
    delete mCalendarButton;
}

void ContactDetails::initialize()
{

    QList<QLineEdit*> lineEdits =  mUi.contactInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        connect( le, SIGNAL( textChanged( const QString& ) ),
                 this, SLOT( slotEnableSaving() ) );

    connect( mUi.salutation, SIGNAL( currentIndexChanged( int ) ),
             this, SLOT( slotEnableSaving() ) );
    connect( mUi.description, SIGNAL( textChanged() ),
             this,  SLOT( slotEnableSaving() ) );

    mCalendarButton = new EditCalendarButton(this);
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget( mCalendarButton );
    mUi.calendarWidget->setLayout( buttonLayout );

    connect( mCalendarButton->calendarWidget(), SIGNAL( selectionChanged() ),
             this, SLOT( slotSetBirthday() ) );

    mModifyFlag = false;

    connect( mUi.detailsBox,  SIGNAL( toggled( bool ) ),
             this,SLOT( slotSetModifyFlag( bool ) ) );
    connect( mUi.otherDetailsBox,  SIGNAL( toggled( bool ) ),
             this,SLOT( slotSetModifyFlag( bool ) ) );
    connect( mUi.addressesBox,  SIGNAL( toggled( bool ) ),
             this,SLOT( slotSetModifyFlag( bool ) ) );
    connect( mUi.descriptionBox,  SIGNAL( toggled( bool ) ),
             this,SLOT( slotSetModifyFlag( bool ) ) );

    connect( mUi.saveButton, SIGNAL( clicked() ),
             this, SLOT( slotSaveContact() ) );
}

void ContactDetails::setItem (const Item &item )
{
    // contact info
    const KABC::Addressee addressee = item.payload<KABC::Addressee>();
    mUi.salutation->setCurrentIndex( mUi.salutation->findText( addressee.custom( "FATCRM", "X-Salutation" ) ) );
    mUi.firstName->setText( addressee.givenName() );
    mUi.lastName->setText( addressee.familyName() );
    mUi.title->setText( addressee.title() );
    mUi.department->setText( addressee.department() );
    mUi.accountName->setText( addressee.organization() );
    mUi.accountName->setProperty( "accountId", qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-AccountId" ) ) );
    mUi.primaryEmail->setText( addressee.preferredEmail() );
    mUi.homePhone->setText(addressee.phoneNumber( KABC::PhoneNumber::Home ).number() );
    mUi.mobilePhone->setText( addressee.phoneNumber( KABC::PhoneNumber::Cell ).number() );
    mUi.officePhone->setText( addressee.phoneNumber( KABC::PhoneNumber::Work ).number() );
    mUi.otherPhone->setText( addressee.phoneNumber( KABC::PhoneNumber::Car ).number() );
    mUi.fax->setText( addressee.phoneNumber( KABC::PhoneNumber::Fax ).number() );

    const KABC::Address address = addressee.address( KABC::Address::Work|KABC::Address::Pref);
    mUi.primaryAddress->setText( address.street() );
    mUi.city->setText( address.locality() );
    mUi.state->setText( address.region() );
    mUi.postalCode->setText( address.postalCode() );
    mUi.country->setText( address.country() );

    const KABC::Address other = addressee.address( KABC::Address::Home );
    mUi.otherAddress->setText( other.street() );
    mUi.otherCity->setText( other.locality() );
    mUi.otherState->setText( other.region() );
    mUi.otherPostalCode->setText( other.postalCode() );
    mUi.otherCountry->setText( other.country() );
    mUi.birthDate->setText( QDateTime( addressee.birthday() ).date().toString( QString("yyyy-MM-dd" ) ) );
    mUi.description->setPlainText( addressee.note() );
    mUi.assistant->setText( addressee.custom( "KADDRESSBOOK", "X-AssistantsName" ) );
    mUi.assistantPhone->setText( addressee.custom( "FATCRM", "X-AssistantsPhone" ) );
    mUi.leadSource->setText( addressee.custom( "FATCRM", "X-LeadSourceName" ) );
    mUi.campaign->setText( addressee.custom( "FATCRM", "X-CampaignName" ) );
    mUi.campaign->setProperty( "campaignId",  qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-CampaignId" ) ) );
    mUi.assignedTo->setText( addressee.custom( "FATCRM", "X-AssignedUserName" ) );
    mUi.assignedTo->setProperty( "assignedToId",  qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-AssignedUserId" ) ) );
    mUi.reportsTo->setText( addressee.custom( "FATCRM", "X-ReportsToUserName" ) );
    mUi.reportsTo->setProperty( "reportsToId",  qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-ReportsToUserId" ) ) );
    mUi.modifiedBy->setText( addressee.custom( "FATCRM", "X-ModifiedByName" ) );
    mUi.modifiedBy->setProperty( "modifiedUserId", qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-ModifiedUserId" ) ) );
    mUi.modifiedBy->setProperty( "modifiedUserName", qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-ModifiedUserName" ) ) );
    mUi.modifiedDate->setText( addressee.custom( "FATCRM", "X-DateModified" ) );
    mUi.createdDate->setText( addressee.custom( "FATCRM", "X-DateCreated"));
    mUi.createdDate->setProperty( "contactId", qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-ContactId" ) ) );
    mUi.createdDate->setProperty( "opportunityRoleFields", qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-OpportunityRoleFields" ) ) );
    mUi.createdBy->setText( addressee.custom( "FATCRM","X-CreatedByName" ) );
    mUi.createdBy->setProperty( "createdById", qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-CreatedById" ) ) );
}

void ContactDetails::clearFields ()
{
    QList<QLineEdit*> lineEdits =
        mUi.contactInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits ) {
        QString value = le->objectName();
        if ( !le->text().isEmpty() ) le->clear();
        if ( value == "campaign" )
            le->setProperty( "campaignId", qVariantFromValue<QString>( QString() ) );
        else if ( value == "accountName" )
            le->setProperty( "accountId", qVariantFromValue<QString>( QString() ) );
        else if ( value == "assignedTo" )
            le->setProperty( "assignedToId", qVariantFromValue<QString>( QString() ) );
        else if ( value == "reportsTo" )
            le->setProperty( "reportsToId", qVariantFromValue<QString>( QString() ) );
        else if ( value == "modifiedBy" ) {
            le->setProperty( "modifiedUserId", qVariantFromValue<QString>( QString() ) );
            le->setProperty( "modifiedUserName", qVariantFromValue<QString>( QString() ) );
        }
        else if ( value == "createdDate" )
            le->setProperty( "contactId", qVariantFromValue<QString>( QString() ) );
        else if ( value == "createdBy" )
            le->setProperty( "createdById", qVariantFromValue<QString>( QString() ) );
    }

    mUi.salutation->setCurrentIndex( 0 );
    mUi.description->clear();
    mUi.firstName->setFocus();
    // enable
    mUi.detailsBox->setChecked( true );
    mUi.otherDetailsBox->setChecked( true );
    mUi.addressesBox->setChecked( true );
    mUi.descriptionBox->setChecked( true );
    // we are creating a new contact
    slotSetModifyFlag( false );
}


void ContactDetails::slotSetModifyFlag( bool value )
{
    mModifyFlag = value;
}

void ContactDetails::slotEnableSaving()
{
    mUi.saveButton->setEnabled( true );
}

void ContactDetails::slotSaveContact()
{
    if ( !mContactData.empty() )
        mContactData.clear();

    mUi.modifiedDate->setText( QDateTime::currentDateTime().toString( QString( "yyyy-MM-dd hh:mm:ss") ) );
    QList<QLineEdit*> lineEdits =
        mUi.contactInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits ) {
        QString objName = le->objectName();
        mContactData[objName] = le->text();
        if ( objName == "campaign" )
            mContactData["campaignId"] = le->property( "campaignId" ).toString();
        else if ( objName == "accountName" )
            mContactData["accountId"] = le->property( "accountId" ).toString();
        else if ( objName == "assignedTo" )
            mContactData["assignedToId"] = le->property( "assignedToId" ).toString();
        else if ( objName == "reportsTo" )
            mContactData["reportsToId"] = le->property( "reportsToId" ).toString();
        else if ( objName == "modifiedBy" ) {
            mContactData["modifiedUserId"] = le->property( "modifiedUserId" ).toString();
            mContactData["modifiedUserName"] = le->property( "modifiedUserName" ).toString();
        }
        else if ( objName == "createdDate" ) {
            mContactData["contactId"] = le->property( "contactId" ).toString();
            mContactData["opportunityRoleFields"] =  le->property( "opportunityRoleFields" ).toString();
        }
        else if ( objName == "createdBy" )
            mContactData["createdById"] = le->property( "createdById" ).toString();
    }
    mContactData["salutation"] = mUi.salutation->currentText();
    mContactData["description"] = mUi.description->toPlainText();

    if ( !mModifyFlag )
        emit saveContact();
    else
        emit modifyContact();

    mUi.saveButton->setEnabled( false );
}

void ContactDetails::slotSetBirthday()
{
    mUi.birthDate->setText( mCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );

}


