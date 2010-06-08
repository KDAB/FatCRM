#include "contactdetails.h"

#include <akonadi/item.h>

#include <kabc/addressee.h>
#include <kabc/address.h>

using namespace Akonadi;

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

    QList<QComboBox*> comboBoxes =  mUi.contactInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        connect( cb, SIGNAL( currentIndexChanged( int ) ),
                 this, SLOT( slotEnableSaving() ) );

     QList<QGroupBox*> groupBoxes =
        mUi.contactInformationGB->findChildren<QGroupBox*>();
    Q_FOREACH( QGroupBox* gb, groupBoxes ) {
       connect( gb, SIGNAL( toggled( bool ) ),
                this, SLOT( slotEnableSaving() ) );
       connect( gb, SIGNAL( toggled( bool ) ),
                this, SLOT( slotSetModifyFlag( bool ) ) );
    }

    mModifyFlag = false;

    connect( mUi.description, SIGNAL( textChanged() ),
             this,  SLOT( slotEnableSaving() ) );
    connect (mUi.doNotCall, SIGNAL( stateChanged( int ) ),
             this, SLOT( slotEnableSaving() ) );


    mCalendarButton = new EditCalendarButton(this);
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget( mCalendarButton );
    mUi.calendarWidget->setLayout( buttonLayout );

    connect( mCalendarButton->calendarWidget(), SIGNAL( selectionChanged() ),
             this, SLOT( slotSetBirthday() ) );

    connect( mUi.saveButton, SIGNAL( clicked() ),
             this, SLOT( slotSaveContact() ) );

    mUi.saveButton->setEnabled( false );
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
    mUi.accountName->setCurrentIndex( mUi.accountName->findText( addressee.organization() ) );
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
    mUi.leadSource->setCurrentIndex( mUi.leadSource->findText( addressee.custom( "FATCRM", "X-LeadSourceName" ) ) );
    mUi.campaign->setCurrentIndex( mUi.campaign->findText( addressee.custom( "FATCRM", "X-CampaignName" ) ) );

    mUi.assignedTo->setCurrentIndex( mUi.assignedTo->findText( addressee.custom( "FATCRM", "X-AssignedUserName" ) ) );
    mUi.reportsTo->setCurrentIndex( mUi.reportsTo->findText( addressee.custom( "FATCRM", "X-ReportsToUserName" ) ) );
    mUi.modifiedBy->setText( addressee.custom( "FATCRM", "X-ModifiedByName" ) );
    mUi.modifiedBy->setProperty( "modifiedUserId", qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-ModifiedUserId" ) ) );
    mUi.modifiedBy->setProperty( "modifiedUserName", qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-ModifiedUserName" ) ) );
    mUi.modifiedDate->setText( addressee.custom( "FATCRM", "X-DateModified" ) );
    mUi.createdDate->setText( addressee.custom( "FATCRM", "X-DateCreated"));
    mUi.createdDate->setProperty( "contactId", qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-ContactId" ) ) );
    mUi.createdDate->setProperty( "opportunityRoleFields", qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-OpportunityRoleFields" ) ) );
    mUi.createdDate->setProperty( "cAcceptStatusFields",  qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-CacceptStatusFields" ) ) );
    mUi.createdDate->setProperty( "mAcceptStatusFields",  qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-MacceptStatusFields" ) ) );
    mUi.createdDate->setProperty( "deleted",  qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-Deleted" ) ) );
    bool donotcall = ( addressee.custom( "FATCRM", "X-DoNotCall" ).isEmpty() || addressee.custom( "FATCRM", "X-DoNotCall" ) == "0" ) ? false : true;
    mUi.doNotCall->setChecked( donotcall );
    mUi.createdBy->setText( addressee.custom( "FATCRM","X-CreatedByName" ) );
    mUi.createdBy->setProperty( "createdById", qVariantFromValue<QString>( addressee.custom( "FATCRM", "X-CreatedById" ) ) );
}

void ContactDetails::clearFields ()
{
    // reset line edits
    QList<QLineEdit*> lineEdits =
        mUi.contactInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        le->setText(QString());

    // reset label and properties
    QList<QLabel*> labels =
        mUi.contactInformationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel* lab, labels ) {
        QString value = lab->objectName();
        if ( value == "modifiedBy" ) {
            lab->clear();
            lab->setProperty( "modifiedUserId", qVariantFromValue<QString>( QString() ) );
            lab->setProperty( "modifiedUserName", qVariantFromValue<QString>( QString() ) );
        }
        else if ( value == "createdDate" ) {
            lab->clear();
            lab->setProperty( "contactId", qVariantFromValue<QString>( QString() ) );
            lab->setProperty( "opportunityRoleFields", qVariantFromValue<QString>( QString() ) );
            lab->setProperty( "cAcceptStatusFields",  qVariantFromValue<QString>( QString() ) );
            lab->setProperty( "mAcceptStatusFields",  qVariantFromValue<QString>( QString() ) );
            lab->setProperty( "deleted", qVariantFromValue<QString>( QString() ) );
        }
        else if ( value == "createdBy" ) {
            lab->clear();
            lab->setProperty( "createdById", qVariantFromValue<QString>( QString() ) );
        }
    }

    // enable
    QList<QGroupBox*> groupBoxes =
        mUi.contactInformationGB->findChildren<QGroupBox*>();
    Q_FOREACH( QGroupBox* gb, groupBoxes )
        gb->setChecked( true );

    // reset combos
    QList<QComboBox*> comboBoxes =
        mUi.contactInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        cb->setCurrentIndex( 0 );

    // initialize other fields
    mUi.doNotCall->setChecked( false );
    mUi.description->clear();
    mUi.firstName->setFocus();


    // we are creating a new contact
    slotSetModifyFlag( false );

}


void ContactDetails::slotSetModifyFlag( bool value )
{
    mModifyFlag = value;
}

void ContactDetails::slotEnableSaving()
{
    QList<QGroupBox*> groupBoxes =
        mUi.contactInformationGB->findChildren<QGroupBox*>();

    Q_FOREACH( QGroupBox* gb, groupBoxes )
        if ( gb->isChecked() ) {
            mUi.saveButton->setEnabled( true );
            return;
        }
    mUi.saveButton->setEnabled( false );
}

void ContactDetails::slotSaveContact()
{
    if ( !mContactData.empty() )
        mContactData.clear();

    mUi.modifiedDate->setText( QDateTime::currentDateTime().toString( QString( "yyyy-MM-dd hh:mm:ss") ) );

    QList<QLineEdit*> lineEdits =
        mUi.contactInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        mContactData[le->objectName()] = le->text();

    QList<QLabel*> labels =
        mUi.contactInformationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel* lab, labels ) {
        QString objName = lab->objectName();
        if ( objName == "modifiedDate" )
            mContactData["modifiedDate"] = lab->text();
        else if ( objName == "modifiedBy" ) {
            mContactData["modifiedBy"] = lab->text();
            mContactData["modifiedUserId"] = lab->property( "modifiedUserId" ).toString();
            mContactData["modifiedUserName"] = lab->property( "modifiedUserName" ).toString();
        }
        else if ( objName == "createdDate" ) {
            mContactData["createdDate"] = lab->text();
            mContactData["contactId"] = lab->property( "contactId" ).toString();
            mContactData["opportunityRoleFields"] =  lab->property( "opportunityRoleFields" ).toString();
            mContactData["cAcceptStatusFields"] = lab->property( "cAcceptStatusFields" ).toString();
            mContactData["mAcceptStatusFields"] = lab->property( "mAcceptStatusFields" ).toString();
            mContactData["deleted"] = lab->property( "deleted" ).toString();
        }
        else if ( objName == "createdBy" ) {
            mContactData["createdBy"] = lab->text();
            mContactData["createdById"] = lab->property( "createdById" ).toString();
        }
    }

    mContactData["salutation"] = mUi.salutation->currentText();
    mContactData["campaign"] = mUi.campaign->currentText();
    mContactData["campaignId"] = mCampaignsData.value( mUi.campaign->currentText() );
    mContactData["accountName"] = mUi.accountName->currentText();
    mContactData["accountId"] = mAccountsData.value(  mUi.accountName->currentText() );
    mContactData["reportsTo"] = mUi.reportsTo->currentText();
    mContactData["reportsToId"] = mReportsToData.value( mUi.reportsTo->currentText() );
    mContactData["assignedTo"] = mUi.assignedTo->currentText();
    mContactData["assignedToId"] = mAssignedToData.value( mUi.assignedTo->currentText() );
    mContactData["leadSource"] = mUi.leadSource->currentText();
    mContactData["description"] = mUi.description->toPlainText();
    mContactData["doNotCall"] =   mUi.doNotCall->isChecked() ? "1" : "0";

    if ( !mModifyFlag )
        emit saveContact();
    else
        emit modifyContact();

}

void ContactDetails::slotSetBirthday()
{
    mUi.birthDate->setText( mCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mCalendarButton->calendarWidget()->close();
}

void ContactDetails::addAccountData( const QString &accountName,  const QString &accountId )
{
    mAccountsData.insert( accountName, accountId );
    if ( mUi.accountName->findText( accountName ) < 0 )
        mUi.accountName->addItem( accountName );
}

void ContactDetails::removeAccountData( const QString &accountName )
{
    mAccountsData.remove( accountName );
    int index = mUi.accountName->findText( accountName );
    if ( index >= 0 )
        mUi.accountName->removeItem( index );
}

void ContactDetails::addCampaignData( const QString &campaignName,  const QString &campaignId )
{
    mCampaignsData.insert( campaignName, campaignId );
    if ( mUi.campaign->findText( campaignName ) < 0 )
        mUi.campaign->addItem( campaignName );
}

void ContactDetails::removeCampaignData( const QString &campaignName )
{
    mCampaignsData.remove( campaignName );
    int index = mUi.campaign->findText( campaignName );
    if ( index >= 0 )
        mUi.campaign->removeItem( index );
}

void ContactDetails::addReportsToData( const QString &name, const QString &id )
{
    mReportsToData.insert( name, id );
    if ( mUi.reportsTo->findText( name ) < 0 )
        mUi.reportsTo->addItem( name );
}

// Pending (michel) add a remove method when we have the source module

void ContactDetails::addAssignedToData( const QString &name, const QString &id )
{
    mAssignedToData.insert( name, id );
    if ( mUi.assignedTo->findText( name ) < 0 )
        mUi.assignedTo->addItem( name );
}

// Pending (michel) add a remove method when we have the source module

void ContactDetails::disableGroupBoxes()
{
    QList<QGroupBox*> groupBoxes =
        mUi.contactInformationGB->findChildren<QGroupBox*>();

    Q_FOREACH( QGroupBox* gb, groupBoxes )
        gb->setChecked( false );
}
