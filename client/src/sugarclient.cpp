#include "sugarclient.h"

#include <akonadi/agentfilterproxymodel.h>
#include <akonadi/agentinstance.h>
#include <akonadi/agentinstancemodel.h>
#include <akonadi/control.h>

#include <QDockWidget>
#include <QToolBar>

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
  resize( 900, 900 );
  createMenus();
  setupActions();
  createDockWidgets();
  createToolBars();
}

void SugarClient::createMenus()
{
    mViewMenu = menuBar()->addMenu( tr( "&View" ) );
}

void SugarClient::createToolBars()
{
    QToolBar *toolBar = addToolBar( tr( "&View" ) );
    toolBar->addAction( mContactDetailsDock->toggleViewAction() );
}

void SugarClient::createDockWidgets()
{
    mContactDetailsDock = new QDockWidget(tr("Contact Details"), this );
    mContactDetailsWidget = new ContactDetails(mContactDetailsDock);
    mContactDetailsDock->setWidget( mContactDetailsWidget );
    mContactDetailsDock->setHidden( true );
    addDockWidget( Qt::BottomDockWidgetArea, mContactDetailsDock );
    mViewMenu->addAction( mContactDetailsDock->toggleViewAction() );


    connect(mUi.contactsPage,SIGNAL(contactItemChanged()),
            this, SLOT( slotContactItemChanged()));
}

void SugarClient::slotResourceSelectionChanged( int index )
{
    AgentInstance agent = mUi.resourceSelector->itemData( index, AgentInstanceModel::InstanceRole ).value<AgentInstance>();
    if ( agent.isValid() ) {
        emit resourceSelected( agent.identifier().toLatin1() );
    }
}

void SugarClient::slotContactItemChanged()
{
    if ( !mContactDetailsDock->isVisible() && mContactDetailsDock->toggleViewAction()->isChecked() )
        mContactDetailsDock->setVisible( true );
}

void SugarClient::setupActions()
{
    connect( mUi.actionSyncronize, SIGNAL( triggered() ), mUi.contactsPage, SLOT( syncronize() ) );
    connect( mUi.contactsPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
}

void SugarClient::slotShowMessage( const QString& message )
{
    statusBar()->showMessage( message, 3000 );
}

#include "sugarclient.moc"
