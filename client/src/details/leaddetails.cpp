#include "leaddetails.h"

#include "editcalendarbutton.h"
#include "ui_leaddetails.h"
#include "referenceddatamodel.h"

#include <kdcrmdata/sugarlead.h>

LeadDetails::LeadDetails( QWidget *parent )
    : Details( Lead, parent ), mUi( new Ui::LeadDetails )

{
    mUi->setupUi( this );
    initialize();
}

LeadDetails::~LeadDetails()
{
    delete mUi;
}

void LeadDetails::initialize()
{
    mUi->leadSource->addItems( sourceItems() );
    mUi->campaignName->setModel( new ReferencedDataModel( CampaignRef, this ) );
    mUi->salutation->addItems( salutationItems() );
    mUi->accountName->setModel( new ReferencedDataModel( AccountRef, this ) );
    mUi->assignedUserName->setModel( new ReferencedDataModel( AssignedToRef, this ) );

    connect( mUi->clearDateButton, SIGNAL( clicked() ), this, SLOT( slotClearDate() ) );
    connect( mUi->calendarButton->calendarWidget(), SIGNAL( clicked( const QDate& ) ),
             this, SLOT( slotSetBirthDate() ) );

    mUi->status->addItems( statusItems() );
}

void LeadDetails::slotSetBirthDate()
{
    // TODO FIXME: use locale formatting
    mUi->birthdate->setText( mUi->calendarButton->calendarWidget()->selectedDate().toString( QString( "yyyy-MM-dd" ) ) );
    mUi->calendarButton->calendarWidget()->setSelectedDate( QDate::currentDate() );
    mUi->calendarButton->calendarDialog()->close();
}

void LeadDetails::slotClearDate()
{
    mUi->birthdate->clear();
}

QStringList LeadDetails::statusItems() const
{
    QStringList status;
    status << QString("") << QString( "New" )
           << QString( "Assigned" ) << QString( "In Process" )
           << QString( "Converted" ) << QString( "Recycled" )
           << QString( "Dead" );
    return status;
}

QMap<QString, QString> LeadDetails::data( const Akonadi::Item &item ) const
{
    SugarLead lead = item.payload<SugarLead>();
    return lead.data();
}

void LeadDetails::updateItem( Akonadi::Item &item, const QMap<QString, QString> &data ) const
{
    SugarLead lead;
    if ( item.hasPayload<SugarLead>() ) {
        lead = item.payload<SugarLead>();
    }
    lead.setData( data );

    item.setMimeType( SugarLead::mimeType() );
    item.setPayload<SugarLead>( lead );
}
