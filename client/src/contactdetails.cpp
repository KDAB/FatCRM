#include "contactdetails.h"
#include "sugarclient.h"

#include <akonadi/item.h>

#include <kabc/addressee.h>
#include <kabc/address.h>

using namespace Akonadi;

ContactDetails::ContactDetails( QWidget *parent )
    : AbstractDetails( parent )

{
    mUi.setupUi( this );

    //calendar
    mCalendarButton = new EditCalendarButton(this);
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget( mCalendarButton );
    mUi.calendarWidget->setLayout( buttonLayout );

    connect( mUi.clearButton, SIGNAL( clicked() ),
             this, SLOT( slotClearDate() ) );

    connect( mCalendarButton->calendarWidget(), SIGNAL(clicked(const QDate&)),
             this, SLOT(slotSetBirthday()));

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

    connect( mUi.modifiedDate, SIGNAL( textChanged( const QString& ) ),
             this, SLOT( slotResetCursor( const QString& ) ) );

    QList<QComboBox*> comboBoxes =  mUi.contactInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        connect( cb, SIGNAL( currentIndexChanged( int ) ),
                 this, SLOT( slotEnableSaving() ) );

    connect( mUi.description, SIGNAL( textChanged() ),
             this,  SLOT( slotEnableSaving() ) );
    connect (mUi.doNotCall, SIGNAL( stateChanged( int ) ),
             this, SLOT( slotEnableSaving() ) );
    connect( mUi.saveButton, SIGNAL( clicked() ),
             this, SLOT( slotSaveContact() ) );

    mUi.saveButton->setEnabled( false );
    setEditing( false );
}

void ContactDetails::reset()
{
    QList<QLineEdit*> lineEdits =  mUi.contactInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        disconnect( le, SIGNAL( textChanged( const QString& ) ),
                 this, SLOT( slotEnableSaving() ) );

    QList<QComboBox*> comboBoxes =  mUi.contactInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        disconnect( cb, SIGNAL( currentIndexChanged( int ) ),
                    this, SLOT( slotEnableSaving() ) );

    disconnect( mUi.description, SIGNAL( textChanged() ),
                this,  SLOT( slotEnableSaving() ) );

    disconnect (mUi.doNotCall, SIGNAL( stateChanged( int ) ),
                this, SLOT( slotEnableSaving() ) );
    mUi.saveButton->setEnabled( false );
    setEditing( false );
}

void ContactDetails::setItem (const Item &item )
{
    // new item selected reset flag and saving
    mModifyFlag = true;
    reset();

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
    initialize();
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
    if ( sender()->objectName() != "modifiedDate" ) {
        mUi.saveButton->setEnabled( true );
        setEditing( true );
    }

}

void ContactDetails::slotSaveContact()
{
    if ( !mData.empty() )
        mData.clear();

    QList<QLineEdit*> lineEdits =
        mUi.contactInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        mData[le->objectName()] = le->text();

    QList<QLabel*> labels =
        mUi.contactInformationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel* lab, labels ) {
        QString objName = lab->objectName();
        if ( objName == "modifiedBy" ) {
            mData["modifiedBy"] = lab->text();
            mData["modifiedUserId"] = lab->property( "modifiedUserId" ).toString();
            mData["modifiedUserName"] = lab->property( "modifiedUserName" ).toString();
        }
        else if ( objName == "createdDate" ) {
            mData["createdDate"] = lab->text();
            mData["contactId"] = lab->property( "contactId" ).toString();
            mData["opportunityRoleFields"] =  lab->property( "opportunityRoleFields" ).toString();
            mData["cAcceptStatusFields"] = lab->property( "cAcceptStatusFields" ).toString();
            mData["mAcceptStatusFields"] = lab->property( "mAcceptStatusFields" ).toString();
            mData["deleted"] = lab->property( "deleted" ).toString();
        }
        else if ( objName == "createdBy" ) {
            mData["createdBy"] = lab->text();
            mData["createdById"] = lab->property( "createdById" ).toString();
        }
    }
    mData["salutation"] = mUi.salutation->currentText();
    mData["campaign"] = mUi.campaign->currentText();
    mData["campaignId"] = campaignsData().value( mUi.campaign->currentText() );
    mData["accountName"] = mUi.accountName->currentText();
    mData["accountId"] = accountsData().value(  mUi.accountName->currentText() );
    mData["reportsTo"] = mUi.reportsTo->currentText();
    mData["reportsToId"] = reportsToData().value( mUi.reportsTo->currentText() );
    mData["assignedTo"] = mUi.assignedTo->currentText();
    mData["assignedToId"] = assignedToData().value( mUi.assignedTo->currentText() );
    mData["leadSource"] = mUi.leadSource->currentText();
    mData["description"] = mUi.description->toPlainText();
    mData["doNotCall"] =   mUi.doNotCall->isChecked() ? "1" : "0";

    if ( !mModifyFlag )
        emit saveItem();
    else
        emit modifyItem();
}

void ContactDetails::slotSetBirthday()
{
    mUi.birthDate->setText( mCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mCalendarButton->calendarWidget()->setSelectedDate( QDate::currentDate() );
    mCalendarButton->calendarDialog()->close();
}

void ContactDetails::slotClearDate()
{
    mUi.birthDate->clear();
}

void ContactDetails::addAccountData( const QString &name,  const QString &id )
{
    AbstractDetails::addAccountData( name, id );

    if ( mUi.accountName->findText( name ) < 0 )
        mUi.accountName->addItem( name );
}

void ContactDetails::removeAccountData( const QString &accountName )
{
    if ( accountName.isEmpty() )
        return;
    AbstractDetails::removeAccountData( accountName );
    int index = mUi.accountName->findText( accountName );
    if ( index > 0 ) // always leave the first blank field
        mUi.accountName->removeItem( index );
}

void ContactDetails::addCampaignData( const QString &campaignName,  const QString &campaignId )
{
    AbstractDetails::addCampaignData( campaignName, campaignId );
    if ( mUi.campaign->findText( campaignName ) < 0 )
        mUi.campaign->addItem( campaignName );
}

void ContactDetails::removeCampaignData( const QString &campaignName )
{
    if ( campaignName.isEmpty() )
        return;
    AbstractDetails::removeCampaignData( campaignName );
    int index = mUi.campaign->findText( campaignName );
    if ( index > 0 ) // always leave the first blank field
        mUi.campaign->removeItem( index );
}

void ContactDetails::addReportsToData( const QString &name, const QString &id )
{
    qDebug() << "ContactDetails::addReportsToData - name " << name;
    AbstractDetails::addReportsToData( name, id );
    if ( mUi.reportsTo->findText( name ) < 0 )
        mUi.reportsTo->addItem( name );
}

void ContactDetails::addAssignedToData( const QString &name, const QString &id )
{
    AbstractDetails::addAssignedToData( name, id );
    if ( mUi.assignedTo->findText( name ) < 0 )
        mUi.assignedTo->addItem( name );
}

