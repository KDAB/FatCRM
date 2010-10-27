#include "campaigndetails.h"

#include "editcalendarbutton.h"
#include "ui_campaigndetails.h"
#include "referenceddatamodel.h"

#include <kdcrmdata/sugarcampaign.h>

CampaignDetails::CampaignDetails( QWidget *parent )
    : Details( Campaign, parent ), mUi( new Ui::CampaignDetails )

{
    mUi->setupUi( this );
    initialize();
}

CampaignDetails::~CampaignDetails()
{
    delete mUi;
}

void CampaignDetails::initialize()
{
    mUi->status->addItems( statusItems() );
    mUi->campaignType->addItems( typeItems() );
    mUi->currency->addItems( currencyItems() );
    connect( mUi->clearStartDateButton, SIGNAL( clicked() ), this, SLOT( slotClearStartDate() ) );
    connect( mUi->startDateCalendarButton->calendarWidget(), SIGNAL( clicked( const QDate& ) ),
             this, SLOT( slotSetStartDate() ) );
    connect( mUi->clearEndDateButton, SIGNAL( clicked() ), this, SLOT( slotClearEndDate() ) );
    connect( mUi->endDateCalendarButton->calendarWidget(), SIGNAL( clicked( const QDate& ) ),
             this, SLOT( slotSetEndDate() ) );

    mUi->assignedUserName->setModel( new ReferencedDataModel( AssignedToRef, this ) );
}

void CampaignDetails::slotSetStartDate()
{
    // TODO FIXME: use locale formatting or dateeditS
    mUi->startDate->setText( mUi->startDateCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mUi->startDateCalendarButton->calendarWidget()->setSelectedDate( QDate::currentDate() );
    mUi->startDateCalendarButton->calendarDialog()->close();
}

void CampaignDetails::slotSetEndDate()
{
    // TODO FIXME: use locale formatting or dateeditS
    mUi->endDate->setText( mUi->endDateCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mUi->endDateCalendarButton->calendarWidget()->setSelectedDate( QDate::currentDate() );
    mUi->endDateCalendarButton->calendarDialog()->close();
}

void CampaignDetails::slotClearStartDate()
{
    mUi->startDate->clear();
}

void CampaignDetails::slotClearEndDate()
{
    mUi->endDate->clear();
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

void CampaignDetails::updateItem( Akonadi::Item &item, const QMap<QString, QString> &data ) const
{
    SugarCampaign campaign;
    if ( item.hasPayload<SugarCampaign>() ) {
        campaign = item.payload<SugarCampaign>();
    }
    campaign.setData( data );

    item.setMimeType( SugarCampaign::mimeType() );
    item.setPayload<SugarCampaign>( campaign );
}
