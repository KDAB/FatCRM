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
    QGroupBox* detailsBox = new QGroupBox;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    detailsBox->setLayout( vLayout );
    detailsBox->setTitle( tr( "Details" ) );

    QLabel *opportunityNameLabel = new QLabel( tr("Opportunity name: "  ) );
    QLineEdit* name = new QLineEdit();
    name->setObjectName( "name" );
    detailGrid->addWidget( opportunityNameLabel, 0, 0 );
    detailGrid->addWidget( name, 0, 1 );
    QLabel *accountNameLabel = new QLabel( tr( "Account name: " ) );
    QComboBox* accountName = new QComboBox();
    accountName->setObjectName( "accountName" );
    accountName->insertItem( 0, QString( "" ) );
    detailGrid->addWidget( accountNameLabel, 1, 0 );
    detailGrid->addWidget( accountName, 1, 1 );
    QLabel *typeLabel = new QLabel( tr( "Type: " ) );
    QComboBox* opportunityType = new QComboBox();
    opportunityType->setObjectName( "opportunityType" );
    opportunityType->addItems( typeItems() );
    detailGrid->addWidget( typeLabel, 2, 0 );
    detailGrid->addWidget( opportunityType, 2, 1 );
    QLabel *leadSourceLabel = new QLabel( tr( "Lead source: " ) );
    QComboBox* leadSource = new QComboBox();
    leadSource->setObjectName( "leadSource" );
    leadSource->addItems( sourceItems() );
    detailGrid->addWidget( leadSourceLabel, 3, 0 );
    detailGrid->addWidget( leadSource, 3, 1 );
    QLabel *campaignLabel = new QLabel( tr( "Campaign: " ) );
    QComboBox* campaignName = new QComboBox();
    campaignName->setObjectName( "campaignName" );
    campaignName->insertItem( 0, QString( "" ) );
    detailGrid->addWidget( campaignLabel, 4, 0 );
    detailGrid->addWidget( campaignName, 4, 1 );
    QLabel *salesStageLabel = new QLabel( tr( "Sales stage: " ) );
    QComboBox* salesStage = new QComboBox();
    salesStage->setObjectName( "salesStage" );
    salesStage->addItems( stageItems() );
    detailGrid->addWidget( salesStageLabel, 5, 0 );
    detailGrid->addWidget( salesStage, 5, 1 );
    QLabel *assignedToLabel = new QLabel( tr( "Assigned to: " ) );
    QComboBox* assignedUserName = new QComboBox();
    assignedUserName->setObjectName( "assignedUserName" );
    assignedUserName->insertItem( 0, QString( "" ) );
    detailGrid->addWidget( assignedToLabel, 6, 0 );
    detailGrid->addWidget( assignedUserName, 6, 1 );

    return detailsBox;
}

QGroupBox* OpportunityDetails::buildOtherDetailsGroupBox()
{
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

    QGroupBox* otherDetailsBox = new QGroupBox;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *detailGrid = new QGridLayout;
    vLayout->addLayout( detailGrid );
    vLayout->addStretch();
    otherDetailsBox->setLayout( vLayout );
    otherDetailsBox->setTitle( tr( "Other details" ) );

    QLabel *currencyLabel = new QLabel( tr("Currency: "  ) );
    QComboBox* currency = new QComboBox();
    currency->setObjectName( "currency" );
    currency->addItems( currencyItems() );
    detailGrid->addWidget( currencyLabel, 0, 0 );
    detailGrid->addWidget( currency, 0, 1 );
    QLabel *amountLabel = new QLabel( tr( "Amount: " ) );
    QLineEdit* amount = new QLineEdit();
    amount->setObjectName( "amount" );
    detailGrid->addWidget( amountLabel, 1, 0 );
    detailGrid->addWidget( amount, 1, 1 );
    QLabel *closeDateLabel = new QLabel( tr( "Expected close date: " ) );
    detailGrid->addWidget( closeDateLabel, 2, 0 );
    detailGrid->addWidget( calendarWidget, 2, 1 );
    QLabel *nextStepLabel = new QLabel( tr( "Next step: " ) );
    QLineEdit* nextStep = new QLineEdit();
    nextStep->setObjectName( "nextStep" );
    detailGrid->addWidget( nextStepLabel, 3, 0 );
    detailGrid->addWidget( nextStep, 3, 1 );
    QLabel *probabilityLabel = new QLabel( tr( "Probability(%): " ) );
    QLineEdit* probability = new QLineEdit();
    probability->setObjectName( "probability" );
    detailGrid->addWidget( probabilityLabel, 4, 0 );
    detailGrid->addWidget( probability, 4, 1 );

    return otherDetailsBox;
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

QMap<QString, QString> OpportunityDetails::data( const Akonadi::Item &item ) const
{
    SugarOpportunity opportunity = item.payload<SugarOpportunity>();
    return opportunity.data();
}
