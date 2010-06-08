#include "leaddetails.h"

#include <akonadi/item.h>

#include <kdcrmdata/sugarlead.h>

using namespace Akonadi;


LeadDetails::LeadDetails( QWidget *parent )
    : QWidget( parent )

{
    mUi.setupUi( this );
    initialize();
}

LeadDetails::~LeadDetails()
{
    delete mCalendarButton;
}

void LeadDetails::initialize()
{

    QList<QLineEdit*> lineEdits =  mUi.leadInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        connect( le, SIGNAL( textChanged( const QString& ) ),
                 this, SLOT( slotEnableSaving() ) );

    QList<QComboBox*> comboBoxes =  mUi.leadInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        connect( cb, SIGNAL( currentIndexChanged( int ) ),
                 this, SLOT( slotEnableSaving() ) );

     QList<QGroupBox*> groupBoxes =
        mUi.leadInformationGB->findChildren<QGroupBox*>();
    Q_FOREACH( QGroupBox* gb, groupBoxes ) {
       connect( gb, SIGNAL( toggled( bool ) ),
                this, SLOT( slotEnableSaving() ) );
       connect( gb, SIGNAL( toggled( bool ) ),
                this, SLOT( slotSetModifyFlag( bool ) ) );
    }

    mModifyFlag = false;

    mCalendarButton = new EditCalendarButton(this);
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget( mCalendarButton );
    mUi.calendarWidget->setLayout( buttonLayout );

    connect( mCalendarButton->calendarWidget(), SIGNAL( selectionChanged() ),
             this, SLOT( slotSetDateClosed() ) );

    connect( mUi.description, SIGNAL( textChanged() ),
             this,  SLOT( slotEnableSaving() ) );

    connect( mUi.saveButton, SIGNAL( clicked() ),
             this, SLOT( slotSaveLead() ) );

    connect( mUi.copyAddressFromPrimary, SIGNAL( toggled( bool ) ),
             this, SLOT( slotCopyFromPrimary( bool ) ) );

    mUi.saveButton->setEnabled( false );
}

void LeadDetails::setItem (const Item &item )
{
    // lead info
    const SugarLead lead = item.payload<SugarLead>();
    mUi.leadSource->setCurrentIndex( mUi.leadSource->findText( lead.leadSource() ) );
    mUi.leadSourceDescription->setPlainText( lead.leadSourceDescription() );
    mUi.campaignName->setCurrentIndex(mUi.campaignName->findText( lead.campaignName() ) );
    mUi.referedBy->setText( lead.referedBy() );
    mUi.salutation->setCurrentIndex( mUi.salutation->findText( lead.salutation() ) );
    mUi.firstName->setText( lead.firstName() );
    mUi.lastName->setText( lead.lastName() );
    mUi.birthdate->setText( lead.birthdate() );
    mUi.accountName->setText(lead.accountName() );
    mUi.title->setText( lead.title() );
    mUi.department->setText(lead.department() );
    mUi.assignedUserName->setCurrentIndex(mUi.assignedUserName->findText( lead.assignedUserName() ) );
    mUi.status->setCurrentIndex( mUi.status->findText( lead.status() ) );
    mUi.statusDescription->setPlainText( lead.statusDescription() );
    mUi.opportunityAmount->setText( lead.opportunityAmount() );
    mUi.phoneWork->setText( lead.phoneWork() );
    mUi.phoneMobile->setText( lead.phoneMobile() );
    mUi.phoneHome->setText( lead.phoneHome() );
    mUi.phoneOther->setText( lead.phoneOther() );
    mUi.phoneFax->setText( lead.phoneFax() );
    mUi.doNotCall->setChecked( lead.doNotCall() == "1" ? true : false );
    mUi.email1->setText( lead.email1() );
    mUi.email2->setText( lead.email2() );
    mUi.primaryAddressStreet->setText( lead.primaryAddressStreet() );
    mUi.primaryAddressCity->setText( lead.primaryAddressCity() );
    mUi.primaryAddressState->setText( lead.primaryAddressState() );
    mUi.primaryAddressPostalcode->setText( lead.primaryAddressPostalcode() );
    mUi.primaryAddressCountry->setText( lead.primaryAddressCountry() );
    mUi.altAddressStreet->setText( lead.altAddressStreet() );
    mUi.altAddressCity->setText( lead.altAddressCity() );
    mUi.altAddressState->setText( lead.altAddressState() );
    mUi.altAddressPostalcode->setText( lead.altAddressPostalcode() );
    mUi.altAddressCountry->setText( lead.altAddressCountry() );
    mUi.description->setPlainText( lead.description() );
    mUi.modifiedBy->setText( lead.modifiedByName() );
    mUi.modifiedBy->setProperty( "modifiedUserId", qVariantFromValue<QString>( lead.modifiedUserId() ) );
    mUi.modifiedDate->setText( lead.dateModified() );
    mUi.createdDate->setText( lead.dateEntered() );
    mUi.createdDate->setProperty( "id", qVariantFromValue<QString>( lead.id( ) ) );
    mUi.createdDate->setProperty( "deleted",  qVariantFromValue<QString>( lead.deleted( ) ) );
    mUi.createdBy->setText( lead.createdByName() );
    mUi.createdBy->setProperty( "createdBy", qVariantFromValue<QString>( lead.createdBy( ) ) );
}

void LeadDetails::clearFields ()
{
    // reset line edits
    QList<QLineEdit*> lineEdits =
        mUi.leadInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        le->setText(QString());

    // reset label and properties
    QList<QLabel*> labels =
        mUi.leadInformationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel* lab, labels ) {
        QString value = lab->objectName();
        if ( value == "modifiedBy" ) {
            lab->clear();
            lab->setProperty( "modifiedUserId", qVariantFromValue<QString>( QString() ) );
        }
        else if ( value == "createdDate" ) {
            lab->clear();
            lab->setProperty( "id", qVariantFromValue<QString>( QString() ) );
            lab->setProperty( "deleted", qVariantFromValue<QString>( QString() ) );
        }
        else if ( value == "createdBy" ) {
            lab->clear();
            lab->setProperty( "createdBy", qVariantFromValue<QString>( QString() ) );
        }
    }

    // enable
    QList<QGroupBox*> groupBoxes =
        mUi.leadInformationGB->findChildren<QGroupBox*>();
    Q_FOREACH( QGroupBox* gb, groupBoxes )
        gb->setChecked( true );

    // reset combos
    QList<QComboBox*> comboBoxes =
        mUi.leadInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        cb->setCurrentIndex( 0 );

    // reset text fields
     QList<QTextEdit*> textEdits =
        mUi.leadInformationGB->findChildren<QTextEdit*>();
    Q_FOREACH( QTextEdit* te, textEdits )
        te->clear();

    // we are creating a new contact
    slotSetModifyFlag( false );

}


void LeadDetails::slotSetModifyFlag( bool value )
{
    mModifyFlag = value;
}

void LeadDetails::slotEnableSaving()
{
    QList<QGroupBox*> groupBoxes =
        mUi.leadInformationGB->findChildren<QGroupBox*>();

    Q_FOREACH( QGroupBox* gb, groupBoxes )
        if ( gb->isChecked() ) {
            mUi.saveButton->setEnabled( true );
            return;
        }
    mUi.saveButton->setEnabled( false );
}

void LeadDetails::slotSaveLead()
{
    if ( !mData.empty() )
        mData.clear();

    mUi.modifiedDate->setText( QDateTime::currentDateTime().toString( QString( "yyyy-MM-dd hh:mm:ss") ) );

    QList<QLineEdit*> lineEdits =
        mUi.leadInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        mData[le->objectName()] = le->text();

    QList<QLabel*> labels =
        mUi.leadInformationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel* lab, labels ) {
        QString objName = lab->objectName();
        if ( objName == "modifiedDate" )
            mData["modifiedDate"] = lab->text();
        else if ( objName == "modifiedBy" ) {
            mData["modifiedBy"] = lab->text();
            mData["modifiedUserId"] = lab->property( "modifiedUserId" ).toString();
            mData["modifiedUserName"] = lab->property( "modifiedUserName" ).toString();
        }
        else if ( objName == "createdDate" ) {
            mData["createdDate"] = lab->text();
            mData["id"] = lab->property( "id" ).toString();
            mData["deleted"] = lab->property( "deleted" ).toString();
        }
        else if ( objName == "createdBy" ) {
            mData["createdByName"] = lab->text();
            mData["createdBy"] = lab->property( "createdBy" ).toString();
        }
    }
    mData["leadSource"] = mUi.leadSource->currentText();
    mData["leadSourceDescription"] = mUi.leadSourceDescription->toPlainText();
    mData["campaignName"] = mUi.campaignName->currentText();
    mData["campaignId"] = mCampaignsData.value( mUi.campaignName->currentText() );
    mData["salutation"] = mUi.salutation->currentText();
    mData["assignedUserName"] = mUi.assignedUserName->currentText();
    mData["assignedUserId"] = mAssignedToData.value( mUi.assignedUserName->currentText() );
    mData["status"] = mUi.status->currentText();
    mData["statusDescription"] = mUi.statusDescription->toPlainText();
    mData["doNotCall"] = mUi.doNotCall->isChecked() ? "1" : "0";
    mData["description"] = mUi.description->toPlainText();

    if ( !mModifyFlag )
        emit saveLead();
    else
        emit modifyLead();

}

void LeadDetails::addCampaignData( const QString &campaignName,  const QString &campaignId )
{
    mCampaignsData.insert( campaignName, campaignId );
    if ( mUi.campaignName->findText( campaignName ) < 0 )
        mUi.campaignName->addItem( campaignName );
}

void LeadDetails::addAssignedToData( const QString &name, const QString &id )
{
    mAssignedToData.insert( name, id );
    if ( mUi.assignedUserName->findText( name ) < 0 )
        mUi.assignedUserName->addItem( name );
}

void LeadDetails::disableGroupBoxes()
{
    QList<QGroupBox*> groupBoxes =
        mUi.leadInformationGB->findChildren<QGroupBox*>();

    Q_FOREACH( QGroupBox* gb, groupBoxes )
        gb->setChecked( false );
}

void LeadDetails::slotCopyFromPrimary( bool checked )
{
    if ( !checked )
        return;

    mUi.altAddressStreet->setText( mUi.primaryAddressStreet->text() );
    mUi.altAddressCity->setText( mUi.primaryAddressCity->text() );
    mUi.altAddressState->setText( mUi.primaryAddressState->text() );
    mUi.altAddressPostalcode->setText( mUi.primaryAddressPostalcode->text() );
    mUi.altAddressCountry->setText( mUi.primaryAddressCountry->text() );

}

void LeadDetails::slotSetDateClosed()
{
    mUi.birthdate->setText( mCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mCalendarButton->calendarWidget()->close();
}
