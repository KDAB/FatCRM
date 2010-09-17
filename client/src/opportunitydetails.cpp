#include "opportunitydetails.h"

OpportunityDetails::OpportunityDetails( QWidget *parent )
    : Details( parent )

{
    initialize();
}

OpportunityDetails::~OpportunityDetails()
{

}

void OpportunityDetails::initialize()
{
    QHBoxLayout *hLayout = new QHBoxLayout;
    // build the group boxes
    hLayout->addWidget( buildDetailsGroupBox() );
    hLayout->addWidget( buildOtherDetailsGroupBox() );
    setLayout( hLayout );
}

QGroupBox* OpportunityDetails::buildDetailsGroupBox()
{
    mDetailsBox = new QGroupBox;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    mDetailsBox->setLayout( vLayout );
    mDetailsBox->setTitle( tr( "Details" ) );

    QLabel *opportunityNameLabel = new QLabel( tr("Opportunity name: "  ) );
    mName = new QLineEdit();
    mName->setObjectName( "name" );
    detailGrid->addWidget( opportunityNameLabel, 0, 0 );
    detailGrid->addWidget( mName, 0, 1 );
    QLabel *accountNameLabel = new QLabel( tr( "Account name: " ) );
    mAccountName = new QComboBox();
    mAccountName->setObjectName( "accountName" );
    mAccountName->insertItem( 0, QString( "" ) );
    detailGrid->addWidget( accountNameLabel, 1, 0 );
    detailGrid->addWidget( mAccountName, 1, 1 );
    QLabel *typeLabel = new QLabel( tr( "Type: " ) );
    mOpportunityType = new QComboBox();
    mOpportunityType->setObjectName( "opportunityType" );
    mOpportunityType->addItems( typeItems() );
    detailGrid->addWidget( typeLabel, 2, 0 );
    detailGrid->addWidget( mOpportunityType, 2, 1 );
    QLabel *leadSourceLabel = new QLabel( tr( "Lead source: " ) );
    mLeadSource = new QComboBox();
    mLeadSource->setObjectName( "leadSource" );
    mLeadSource->addItems( sourceItems() );
    detailGrid->addWidget( leadSourceLabel, 3, 0 );
    detailGrid->addWidget( mLeadSource, 3, 1 );
    QLabel *campaignLabel = new QLabel( tr( "Campaign: " ) );
    mCampaignName = new QComboBox();
    mCampaignName->setObjectName( "campaignName" );
    mCampaignName->insertItem( 0, QString( "" ) );
    detailGrid->addWidget( campaignLabel, 4, 0 );
    detailGrid->addWidget( mCampaignName, 4, 1 );
    QLabel *salesStageLabel = new QLabel( tr( "Sales stage: " ) );
    mSalesStage = new QComboBox();
    mSalesStage->setObjectName( "salesStage" );
    mSalesStage->addItems( stageItems() );
    detailGrid->addWidget( salesStageLabel, 5, 0 );
    detailGrid->addWidget( mSalesStage, 5, 1 );
    QLabel *assignedToLabel = new QLabel( tr( "Assigned to: " ) );
    mAssignedUserName = new QComboBox();
    mAssignedUserName->setObjectName( "assignedUserName" );
    mAssignedUserName->insertItem( 0, QString( "" ) );
    detailGrid->addWidget( assignedToLabel, 6, 0 );
    detailGrid->addWidget( mAssignedUserName, 6, 1 );

    return mDetailsBox;
}

QGroupBox* OpportunityDetails::buildOtherDetailsGroupBox()
{
        /* build a BirthDate calendar widget */
    // Calendar widgets
    mDateClosed = new QLineEdit();
    mDateClosed->setObjectName( "dateClosed" );
    mClearDateButton = new QToolButton();
    mClearDateButton->setText( tr( "Clear" ) );

    connect( mClearDateButton, SIGNAL( clicked() ),
             this, SLOT( slotClearDate() ) );

    QWidget *calendarWidget = new QWidget();
    mCalendarButton = new EditCalendarButton( this );
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget( mDateClosed );
    hLayout->addWidget( mClearDateButton );
    hLayout->addWidget( mCalendarButton );
    calendarWidget->setLayout( hLayout );

    connect( mCalendarButton->calendarWidget(), SIGNAL(clicked(const QDate&)),
             this, SLOT(slotSetDateClosed()));

    mOtherDetailsBox = new QGroupBox;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    mOtherDetailsBox->setLayout( vLayout );
    mOtherDetailsBox->setTitle( tr( "Other details" ) );

    QLabel *currencyLabel = new QLabel( tr("Currency: "  ) );
    mCurrency = new QComboBox();
    mCurrency->setObjectName( "currency" );
    mCurrency->addItems( currencyItems() );
    detailGrid->addWidget( currencyLabel, 0, 0 );
    detailGrid->addWidget( mCurrency, 0, 1 );
    QLabel *amountLabel = new QLabel( tr( "Amount: " ) );
    mAmount = new QLineEdit();
    mAmount->setObjectName( "amount" );
    detailGrid->addWidget( amountLabel, 1, 0 );
    detailGrid->addWidget( mAmount, 1, 1 );
    QLabel *closeDateLabel = new QLabel( tr( "Expected close date: " ) );
    detailGrid->addWidget( closeDateLabel, 2, 0 );
    detailGrid->addWidget( calendarWidget, 2, 1 );
    QLabel *nextStepLabel = new QLabel( tr( "Next step: " ) );
    mNextStep = new QLineEdit();
    mNextStep->setObjectName( "nextStep" );
    detailGrid->addWidget( nextStepLabel, 3, 0 );
    detailGrid->addWidget( mNextStep, 3, 1 );
    QLabel *probabilityLabel = new QLabel( tr( "Probability(%): " ) );
    mProbability = new QLineEdit();
    mProbability->setObjectName( "probability" );
    detailGrid->addWidget( probabilityLabel, 4, 0 );
    detailGrid->addWidget( mProbability, 4, 1 );

    return mOtherDetailsBox;
}

void OpportunityDetails::slotSetDateClosed()
{
    mDateClosed->setText( mCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mCalendarButton->calendarWidget()->setSelectedDate( QDate::currentDate() );
    mCalendarButton->calendarDialog()->close();
}

void OpportunityDetails::slotClearDate()
{
    mDateClosed->clear();
}

QStringList OpportunityDetails::typeItems() const
{
    QStringList types;
    types << QString("") << QString( "Existing Business" )
          << QString("New Business" );
    return types;
}

QStringList OpportunityDetails::stageItems() const
{
    QStringList stages;
    stages << QString("")
            << QString( "Prospecting" )
            << QString( "Qualification" )
            << QString( "Needs Analysis" )
            << QString( "Value Proposition" )
            << QString( "Id.Decision Makers" )
            << QString( "Perception Analysis" )
            << QString( "Proposal/Price Quote" )
            << QString( "Negociation/Review" )
            << QString( "Closed Won" )
            << QString( "Closed Lost" );
    return stages;
}
