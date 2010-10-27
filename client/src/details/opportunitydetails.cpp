#include "opportunitydetails.h"

#include "editcalendarbutton.h"
#include "ui_opportunitydetails.h"
#include "referenceddatamodel.h"

#include <kdcrmdata/sugaropportunity.h>

OpportunityDetails::OpportunityDetails( QWidget *parent )
    : Details( Opportunity, parent ), mUi( new Ui::OpportunityDetails )

{
    mUi->setupUi( this );
    initialize();
}

OpportunityDetails::~OpportunityDetails()
{
    delete mUi;
}

void OpportunityDetails::initialize()
{
    mUi->accountName->setModel( new ReferencedDataModel( AccountRef, this ) );
    mUi->opportunityType->addItems( typeItems() );
    mUi->leadSource->addItems( sourceItems() );
    mUi->salesStage->addItems( stageItems() );
    mUi->assignedUserName->setModel( new ReferencedDataModel( AssignedToRef, this ) );

    connect( mUi->clearDateButton, SIGNAL( clicked() ), this, SLOT( slotClearDate() ) );
    connect( mUi->calendarButton->calendarWidget(), SIGNAL( clicked( const QDate& ) ),
             this, SLOT( slotSetDateClosed() ) );
    mUi->currency->addItems( currencyItems() );
}

void OpportunityDetails::slotSetDateClosed()
{
    // TODO FIXME: use locale formatting
    mUi->dateClosed->setText( mUi->calendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mUi->calendarButton->calendarWidget()->setSelectedDate( QDate::currentDate() );
    mUi->calendarButton->calendarDialog()->close();
}

void OpportunityDetails::slotClearDate()
{
    mUi->dateClosed->clear();
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

void OpportunityDetails::updateItem( Akonadi::Item &item, const QMap<QString, QString> &data ) const
{
    SugarOpportunity opportunity;
    if ( item.hasPayload<SugarOpportunity>() ) {
        opportunity = item.payload<SugarOpportunity>();
    }
    opportunity.setData( data );

    item.setMimeType( SugarOpportunity::mimeType() );
    item.setPayload<SugarOpportunity>( opportunity );
}
