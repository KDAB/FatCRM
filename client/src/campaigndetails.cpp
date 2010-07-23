#include "campaigndetails.h"

#include <akonadi/item.h>

#include <kdcrmdata/sugarcampaign.h>

using namespace Akonadi;


CampaignDetails::CampaignDetails( QWidget *parent )
    : QWidget( parent )

{
    mUi.setupUi( this );

    // calendars
    mStartDateCalendarButton = new EditCalendarButton(this);
    QVBoxLayout *startLayout = new QVBoxLayout;
    startLayout->addWidget( mStartDateCalendarButton );
    mUi.startCalendarWidget->setLayout( startLayout );

    mEndDateCalendarButton = new EditCalendarButton(this);
    QVBoxLayout *endLayout = new QVBoxLayout;
    endLayout->addWidget( mEndDateCalendarButton );
    mUi.endCalendarWidget->setLayout( endLayout );

    connect( mUi.clearStartDateButton, SIGNAL( clicked() ),
             this, SLOT( slotClearDate() ) );

    connect( mStartDateCalendarButton->calendarWidget(),
             SIGNAL( clicked( const QDate& ) ),
             this, SLOT( slotSetStartDate() ) );

    connect( mUi.clearEndDateButton, SIGNAL( clicked() ),
             this, SLOT( slotClearDate() ) );

    connect( mEndDateCalendarButton->calendarWidget(),
             SIGNAL( clicked( const QDate& ) ),
             this, SLOT( slotSetEndDate() ) );

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

    connect( mUi.objective, SIGNAL( textChanged() ),
             this, SLOT( slotEnableSaving() ) );

    connect( mUi.content, SIGNAL( textChanged() ),
             this,  SLOT( slotEnableSaving() ) );

    connect( mUi.saveButton, SIGNAL( clicked() ),
             this, SLOT( slotSaveCampaign() ) );

    mUi.saveButton->setEnabled( false );
}

void CampaignDetails::reset()
{
    QList<QLineEdit*> lineEdits =  mUi.campaignInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        connect( le, SIGNAL( textChanged( const QString& ) ),
                 this, SLOT( slotEnableSaving() ) );

    QList<QComboBox*> comboBoxes =  mUi.campaignInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        disconnect( cb, SIGNAL( currentIndexChanged( int ) ),
                    this, SLOT( slotEnableSaving() ) );

    disconnect( mUi.objective, SIGNAL( textChanged() ),
                this, SLOT( slotEnableSaving() ) );

    disconnect( mUi.content, SIGNAL( textChanged() ),
                this,  SLOT( slotEnableSaving() ) );
}


void CampaignDetails::setItem (const Item &item )
{
    // new item selected reset flag and saving
    mModifyFlag = true;
    reset();

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
    initialize();
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
        if ( value == "modifiedDate" )
            lab->clear();
        else if ( value == "modifiedBy" ) {
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

    // reset combos
    QList<QComboBox*> comboBoxes =
        mUi.campaignInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        cb->setCurrentIndex( 0 );

    // initialize other fields
    mUi.objective->clear();
    mUi.content->clear();
    mUi.name->setFocus();


    // we are creating a new campaign
    slotSetModifyFlag( false );
}

void CampaignDetails::slotSetModifyFlag( bool value )
{
    mModifyFlag = value;
}

void CampaignDetails::slotEnableSaving()
{
    mUi.saveButton->setEnabled( true );
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

void CampaignDetails::removeCampaignData( const QString &campaignName )
{
    mCampaignsData.remove( campaignName );
}

void CampaignDetails::addAssignedToData( const QString &name, const QString &id )
{
    mAssignedToData.insert( name, id );
    if ( mUi.assignedUserName->findText( name ) < 0 )
        mUi.assignedUserName->addItem( name );
}

void CampaignDetails::slotSetStartDate()
{
    disconnect( mStartDateCalendarButton->calendarWidget(),
             SIGNAL( clicked( const QDate& ) ), this, SLOT( slotSetStartDate() ) );
    mUi.startDate->setText( mStartDateCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mStartDateCalendarButton->calendarWidget()->setSelectedDate( QDate::currentDate() );
    connect( mStartDateCalendarButton->calendarWidget(),
             SIGNAL( clicked( const QDate& ) ), this, SLOT( slotSetStartDate() ) );
    mStartDateCalendarButton->calendarDialog()->close();
}

void CampaignDetails::slotSetEndDate()
{
    disconnect( mEndDateCalendarButton->calendarWidget(),
             SIGNAL( clicked( const QDate& ) ), this, SLOT( slotSetEndDate() ) );
    mUi.endDate->setText( mEndDateCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mEndDateCalendarButton->calendarWidget()->setSelectedDate( QDate::currentDate() );
    connect( mEndDateCalendarButton->calendarWidget(),
             SIGNAL( clicked( const QDate& ) ), this, SLOT( slotSetEndDate() ) );
    mEndDateCalendarButton->calendarDialog()->close();
}

void CampaignDetails::slotClearDate()
{
    if ( sender()->objectName() == "clearStartDateButton" )
        mUi.startDate->clear();
    else
        mUi.endDate->clear();
}
