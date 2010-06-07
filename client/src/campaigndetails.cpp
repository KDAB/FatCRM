#include "campaigndetails.h"

#include <akonadi/item.h>

#include <kdcrmdata/sugarcampaign.h>

using namespace Akonadi;


CampaignDetails::CampaignDetails( QWidget *parent )
    : QWidget( parent )

{
    mUi.setupUi( this );
    initialize();
}

CampaignDetails::~CampaignDetails()
{
    delete mStartDateCalendarButton;
    delete mEndDateCalendarButton;
}

void CampaignDetails::initialize()
{
    QList<QLineEdit*> lineEdits =  mUi.campaignInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        connect( le, SIGNAL( textChanged( const QString& ) ),
                 this, SLOT( slotEnableSaving() ) );

    QList<QComboBox*> comboBoxes =  mUi.campaignInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        connect( cb, SIGNAL( currentIndexChanged( int ) ),
                 this, SLOT( slotEnableSaving() ) );

     QList<QGroupBox*> groupBoxes =
        mUi.campaignInformationGB->findChildren<QGroupBox*>();
    Q_FOREACH( QGroupBox* gb, groupBoxes ) {
       connect( gb, SIGNAL( toggled( bool ) ),
                this, SLOT( slotEnableSaving() ) );
       connect( gb, SIGNAL( toggled( bool ) ),
                this, SLOT( slotSetModifyFlag( bool ) ) );
    }

    mModifyFlag = false;

    mStartDateCalendarButton = new EditCalendarButton(this);
    QVBoxLayout *startLayout = new QVBoxLayout;
    startLayout->addWidget( mStartDateCalendarButton );
    mUi.startCalendarWidget->setLayout( startLayout );

    mEndDateCalendarButton = new EditCalendarButton(this);
    QVBoxLayout *endLayout = new QVBoxLayout;
    endLayout->addWidget( mEndDateCalendarButton );
    mUi.endCalendarWidget->setLayout( endLayout );

    connect( mStartDateCalendarButton->calendarWidget(), SIGNAL( selectionChanged() ),
             this, SLOT( slotSetDateClosed() ) );

    connect( mEndDateCalendarButton->calendarWidget(), SIGNAL( selectionChanged() ),
             this, SLOT( slotSetDateClosed() ) );

    connect( mUi.objective, SIGNAL( textChanged() ),
             this, SLOT( slotEnableSaving() ) );

    connect( mUi.content, SIGNAL( textChanged() ),
             this,  SLOT( slotEnableSaving() ) );

    connect( mUi.saveButton, SIGNAL( clicked() ),
             this, SLOT( slotSaveCampaign() ) );

    mUi.saveButton->setEnabled( false );
}

void CampaignDetails::setItem (const Item &item )
{
    // campaign info
    const SugarCampaign campaign = item.payload<SugarCampaign>();
    mUi.name->setText( campaign.name() );
    mUi.status->setCurrentIndex( mUi.status->findText( campaign.status() ) );
    mUi.startDate->setText( campaign.startDate() );
    mUi.endDate->setText( campaign.endDate() );
    mUi.campaignType->setCurrentIndex( mUi.campaignType->findText( campaign.campaignType() ) );
    // Pending (michel) - figure out
    mUi.currency->setCurrentIndex(1);
    mUi.currency->setProperty( "currencyId", qVariantFromValue<QString>( campaign.currencyId() ) );
    mUi.budget->setText( campaign.budget() );
    mUi.expectedRevenue->setText( campaign.expectedRevenue() );
    mUi.assignedUserName->setCurrentIndex(mUi.assignedUserName->findText( campaign.assignedUserName() ) );
    mUi.impressions->setText( campaign.impressions() );
    mUi.actualCost->setText( campaign.actualCost() );
    mUi.expectedCost->setText( campaign.expectedCost() );
    mUi.objective->setPlainText( campaign.objective() );
    mUi.content->setText( campaign.content() );
    mUi.modifiedBy->setText( campaign.modifiedByName() );
    mUi.modifiedBy->setProperty( "modifiedUserId", qVariantFromValue<QString>( campaign.modifiedUserId() ) );
    mUi.modifiedDate->setText( campaign.dateModified() );
    mUi.createdDate->setText( campaign.dateEntered() );
    mUi.createdDate->setProperty( "id", qVariantFromValue<QString>( campaign.id( ) ) );
    mUi.createdDate->setProperty( "deleted",  qVariantFromValue<QString>( campaign.deleted( ) ) );
    mUi.createdBy->setText( campaign.createdByName() );
    mUi.createdBy->setProperty( "createdBy", qVariantFromValue<QString>( campaign.createdBy( ) ) );
}

void CampaignDetails::clearFields ()
{
    // reset line edits
    QList<QLineEdit*> lineEdits =
        mUi.campaignInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        le->setText(QString());

    // reset label and properties
    QList<QLabel*> labels =
        mUi.campaignInformationGB->findChildren<QLabel*>();
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
        mUi.campaignInformationGB->findChildren<QGroupBox*>();
    Q_FOREACH( QGroupBox* gb, groupBoxes )
        gb->setChecked( true );

    // reset combos
    QList<QComboBox*> comboBoxes =
        mUi.campaignInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        cb->setCurrentIndex( 0 );

    // initialize other fields
    mUi.objective->clear();
    mUi.content->clear();
    mUi.name->setFocus();


    // we are creating a new contact
    slotSetModifyFlag( false );

}


void CampaignDetails::slotSetModifyFlag( bool value )
{
    mModifyFlag = value;
}

void CampaignDetails::slotEnableSaving()
{
    QList<QGroupBox*> groupBoxes =
        mUi.campaignInformationGB->findChildren<QGroupBox*>();

    Q_FOREACH( QGroupBox* gb, groupBoxes )
        if ( gb->isChecked() ) {
            mUi.saveButton->setEnabled( true );
            return;
        }
    mUi.saveButton->setEnabled( false );
}

void CampaignDetails::slotSaveCampaign()
{
    if ( !mData.empty() )
        mData.clear();

    mUi.modifiedDate->setText( QDateTime::currentDateTime().toString( QString( "yyyy-MM-dd hh:mm:ss") ) );

    QList<QLineEdit*> lineEdits =
        mUi.campaignInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        mData[le->objectName()] = le->text();

    QList<QLabel*> labels =
        mUi.campaignInformationGB->findChildren<QLabel*>();
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
    mData["status"] = mUi.status->currentText();
    mData["campaignType"] = mUi.campaignType->currentText();
    mData["currencyId"] = mUi.currency->property( "currencyId" ).toString();
    mData["objective"] = mUi.objective->toPlainText();
    mData["assignedUserName"] = mUi.assignedUserName->currentText();
    mData["assignedUserId"] = mAssignedToData.value( mUi.assignedUserName->currentText() );
    mData["content"] = mUi.content->toPlainText();

    if ( !mModifyFlag )
        emit saveCampaign();
    else
        emit modifyCampaign();

}

void CampaignDetails::addCampaignData( const QString &campaignName,  const QString &campaignId )
{
    mCampaignsData.insert( campaignName, campaignId );
}

void CampaignDetails::addAssignedToData( const QString &name, const QString &id )
{
    mAssignedToData.insert( name, id );
}

void CampaignDetails::fillCombos()
{
    QList<QString> names = mAssignedToData.uniqueKeys();
    // fill
    // assigned to
    for ( int i = 0; i < names.count(); ++i )
        mUi.assignedUserName->addItem( names[i] );
}

void CampaignDetails::disableGroupBoxes()
{
    QList<QGroupBox*> groupBoxes =
        mUi.campaignInformationGB->findChildren<QGroupBox*>();

    Q_FOREACH( QGroupBox* gb, groupBoxes )
        gb->setChecked( false );
}

void CampaignDetails::slotSetDateClosed()
{
    if ( QObject::sender()->objectName().contains( "start" ) )
        mUi.startDate->setText( mStartDateCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    else
        mUi.endDate->setText( mEndDateCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
}
