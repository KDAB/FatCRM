#include "sugarclient.h"

#include <akonadi/agentfilterproxymodel.h>
#include <akonadi/agentinstance.h>
#include <akonadi/agentinstancemodel.h>
#include <akonadi/control.h>

using namespace Akonadi;

SugarClient::SugarClient()
    : QMainWindow()
{
    mUi.setupUi( this );

    /*
     * this creates an overlay in case Akonadi is not running,
     * allowing the user to restart it
     */
    Akonadi::Control::widgetNeedsAkonadi( this );

    QMetaObject::invokeMethod( this, "slotDelayedInit", Qt::QueuedConnection );
    initialize();
}

SugarClient::~SugarClient()
{
}

void SugarClient::slotDelayedInit()
{
    connect( this, SIGNAL( resourceSelected( QByteArray ) ),
             mUi.contactsPage, SLOT( slotResourceSelectionChanged( QByteArray ) ) );

    // monitor Akonadi agents so we can check for SugarCRM specific resources
    AgentInstanceModel *agentModel = new AgentInstanceModel( this );
    AgentFilterProxyModel *agentFilterModel = new AgentFilterProxyModel( this );
    agentFilterModel->addCapabilityFilter( QLatin1String( "SugarCRM" ) );
    agentFilterModel->setSourceModel( agentModel );
    mUi.resourceSelector->setModel( agentFilterModel );

    connect( mUi.resourceSelector, SIGNAL( currentIndexChanged( int ) ),
             this, SLOT( slotResourceSelectionChanged( int ) ) );

    slotResourceSelectionChanged( mUi.resourceSelector->currentIndex() );
}

void SugarClient::initialize()
{
  resize( 900, 400 );
}

void SugarClient::slotResourceSelectionChanged( int index )
{
    AgentInstance agent = mUi.resourceSelector->itemData( index, AgentInstanceModel::InstanceRole ).value<AgentInstance>();
    if ( agent.isValid() ) {
        emit resourceSelected( agent.identifier().toLatin1() );
    }
}

#include "sugarclient.moc"
