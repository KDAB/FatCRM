#include "abstractdetails.h"
#include "sugarclient.h"

AbstractDetails::AbstractDetails( QWidget* parent )
    : QWidget( parent )
{
    setEditing( false );
}


AbstractDetails::~AbstractDetails()
{

}

void AbstractDetails::addAccountData( const QString &name,  const QString &id )
{
    QString dataKey;
    dataKey = mAccountsData.key( id );
    removeAccountData( dataKey );
    mAccountsData.insert( name, id );
}

void AbstractDetails::removeAccountData( const QString &name )
{
    mAccountsData.remove( name );
}


void AbstractDetails::addCampaignData( const QString &name,  const QString &id )
{
    QString dataKey;
    dataKey = mCampaignsData.key( id );
    removeCampaignData( dataKey );
    mCampaignsData.insert( name, id );
}

void AbstractDetails::removeCampaignData( const QString &name )
{
    mCampaignsData.remove( name );

}

void AbstractDetails::addAssignedToData( const QString &name, const QString &id )
{
    if ( mAssignedToData.values().contains( id ) )
        mAssignedToData.remove( mAssignedToData.key( id ) );
    mAssignedToData.insert( name, id );
}

void AbstractDetails::addReportsToData( const QString &name, const QString &id )
{
    if ( mReportsToData.values().contains( id ) )
        mReportsToData.remove( mReportsToData.key( id ) );
    mReportsToData.insert( name, id );
}

void AbstractDetails::slotResetCursor( const QString& text)
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( !w )
        return;
    if ( !text.isEmpty() ) {
        do {
            QApplication::restoreOverrideCursor();
        } while ( QApplication::overrideCursor() != 0 );
        if ( !w->isEnabled() )
            w->setEnabled( true );
    }
}
