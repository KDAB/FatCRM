
#include "campaigndetails.h"

#include <kdcrmdata/sugarcampaign.h>

CampaignDetails::CampaignDetails( QWidget *parent )
    : Details( parent )

{
    initialize();
}

CampaignDetails::~CampaignDetails()
{

}

void CampaignDetails::initialize()
{
    QHBoxLayout *hLayout = new QHBoxLayout;
    // build the group boxes
    hLayout->addWidget( buildDetailsGroupBox() );
    hLayout->addWidget( buildOtherDetailsGroupBox() );
    setLayout( hLayout );
}

QGroupBox* CampaignDetails::buildDetailsGroupBox()
{
    // Calendar widgets
    mStartDate = new QLineEdit();
    mStartDate->setObjectName( "startDate" );
    mClearStartDateButton = new QToolButton();
    mClearStartDateButton->setText( tr( "Clear" ) );

    connect( mClearStartDateButton, SIGNAL( clicked() ),
             this, SLOT( slotClearDate() ) );

    QWidget *startCalendarWidget = new QWidget();
    mStartDateCalendarButton = new EditCalendarButton( this );
    QHBoxLayout *startLayout = new QHBoxLayout;
    startLayout->addWidget( mStartDate );
    startLayout->addWidget( mClearStartDateButton );
    startLayout->addWidget( mStartDateCalendarButton );
    startCalendarWidget->setLayout( startLayout );

    connect( mStartDateCalendarButton->calendarWidget(), SIGNAL(clicked(const QDate&)),
             this, SLOT(slotSetStartDate()));

    mEndDate = new QLineEdit();
    mEndDate->setObjectName( "endDate" );
    mClearEndDateButton = new QToolButton();
    mClearEndDateButton->setText( tr( "Clear" ) );

    connect( mClearEndDateButton, SIGNAL( clicked() ),
             this, SLOT( slotClearDate() ) );

    QWidget *endCalendarWidget = new QWidget();
    mEndDateCalendarButton = new EditCalendarButton( this );
    QHBoxLayout *endLayout = new QHBoxLayout;
    endLayout->addWidget( mEndDate );
    endLayout->addWidget( mClearEndDateButton );
    endLayout->addWidget( mEndDateCalendarButton );
    endCalendarWidget->setLayout( endLayout );

    connect( mEndDateCalendarButton->calendarWidget(), SIGNAL(clicked(const QDate&)),
             this, SLOT(slotSetEndDate()));

    // Box
    QGroupBox* detailsBox = new QGroupBox;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    detailsBox->setLayout( vLayout );
    detailsBox->setTitle( tr( "Details" ) );

    QLabel *nameLabel = new QLabel( tr( "Name:" ) );
    QLineEdit *name = new QLineEdit();
    name->setObjectName( "name" );
    detailGrid->addWidget( nameLabel, 0, 0 );
    detailGrid->addWidget( name, 0, 1 );
    QLabel *statusLabel = new QLabel( tr( "Status: " ) );
    QComboBox* status = new QComboBox();
    status->setObjectName( "status" );
    status->addItems( statusItems() );
    detailGrid->addWidget( statusLabel, 1, 0 );
    detailGrid->addWidget( status, 1, 1 );
    QLabel *startDateLabel = new QLabel( tr( "Start date: " ) );
    detailGrid->addWidget( startDateLabel, 2, 0 );
    detailGrid->addWidget( startCalendarWidget, 2, 1 );
    QLabel *endDateLabel = new QLabel( tr( "End date: " ) );
    detailGrid->addWidget( endDateLabel, 3, 0 );
    detailGrid->addWidget( endCalendarWidget, 3, 1 );
    QLabel *typeLabel = new QLabel( tr( "Type: " ) );
    QComboBox* campaignType = new QComboBox();
    campaignType->setObjectName( "campaignType" );
    campaignType->addItems( typeItems() );
    detailGrid->addWidget( typeLabel, 4, 0 );
    detailGrid->addWidget( campaignType, 4, 1 );
    QLabel *currencyLabel = new QLabel( tr( "Currency: " ) );
    QComboBox* currency = new QComboBox();
    currency->setObjectName( "currency" );
    currency->addItems( currencyItems() );
    detailGrid->addWidget( currencyLabel, 5, 0 );
    detailGrid->addWidget( currency, 5, 1 );

    return detailsBox;
}

QGroupBox* CampaignDetails::buildOtherDetailsGroupBox()
{
    QGroupBox* otherDetailsBox = new QGroupBox;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    otherDetailsBox->setLayout( vLayout );
    otherDetailsBox->setTitle( tr( "Other details" ) );

    QLabel *assignedToLabel = new QLabel( tr( "Assigned to: " ) );
    QComboBox* assignedUserName = new QComboBox();
    assignedUserName->setObjectName( "assignedUserName" );
    assignedUserName->addItem( 0, QString( "" ) );
    detailGrid->addWidget( assignedToLabel, 0, 0 );
    detailGrid->addWidget( assignedUserName, 0, 1 );
    QLabel *impressionsLabel = new QLabel( tr( "Impressions: " ) );
    QLineEdit* impressions = new QLineEdit();
    impressions->setObjectName( "impressions" );
    detailGrid->addWidget( impressionsLabel, 1, 0 );
    detailGrid->addWidget( impressions, 1, 1 );
    QLabel *actualCostLabel = new QLabel( tr( "Actual cost: " ) );
    QLineEdit* actualCost = new QLineEdit();
    actualCost->setObjectName( "actualCost" );
    detailGrid->addWidget( actualCostLabel, 2, 0 );
    detailGrid->addWidget( actualCost, 2, 1 );
    QLabel *expectedCostLabel = new QLabel( tr( "Expected cost: " ) );
    QLineEdit* expectedCost = new QLineEdit();
    expectedCost->setObjectName( "expectedCost" );
    detailGrid->addWidget( expectedCostLabel, 3, 0 );
    detailGrid->addWidget( expectedCost, 3, 1 );
    QLabel *objectiveLabel = new QLabel( tr( "Objective: " ) );
    QTextEdit* objective = new QTextEdit();
    objective->setObjectName( "objective" );
    detailGrid->addWidget( objectiveLabel, 4, 0 );
    detailGrid->addWidget( objective, 4, 1 );

    return otherDetailsBox;
}

void CampaignDetails::slotSetStartDate()
{
    mStartDate->setText( mStartDateCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mStartDateCalendarButton->calendarWidget()->setSelectedDate( QDate::currentDate() );
    mStartDateCalendarButton->calendarWidget()->close();
}

void CampaignDetails::slotSetEndDate()
{
    mEndDate->setText( mEndDateCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mEndDateCalendarButton->calendarWidget()->setSelectedDate( QDate::currentDate() );
    mEndDateCalendarButton->calendarWidget()->close();
}

void CampaignDetails::slotClearDate()
{
    if ( sender()->objectName() == "clearStartDateButton" )
        mStartDate->clear();
    else
        mEndDate->clear();
}

QStringList CampaignDetails::statusItems() const
{
    QStringList status;
    status << QString( "" ) << QString( "Planning" )
           << QString( "Active" ) << QString( "Inactive" )
           << QString( "Complete" ) << QString( "In Queue" )
           << QString( "Sending" );
    return status;
}

QStringList CampaignDetails::typeItems() const
{
    QStringList types;
    types << QString( "" ) << QString( "Telesales" )
          << QString( "Mail" ) << QString( "Email" )
          << QString( "Print" ) << QString( "Web" )
          << QString( "Radio" ) << QString( "Television" )
          << QString( "Newsletter" );
    return types;
}

QMap<QString, QString> CampaignDetails::data( const Akonadi::Item &item ) const
{
    SugarCampaign campaign = item.payload<SugarCampaign>();
    return campaign.data();
}
