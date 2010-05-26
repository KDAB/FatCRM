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
             mContactsPage, SLOT( slotResourceSelectionChanged( QByteArray ) ) );
    connect( this, SIGNAL( resourceSelected( QByteArray ) ),
             mAccountsPage, SLOT( slotResourceSelectionChanged( QByteArray ) ) );
    connect( this, SIGNAL( resourceSelected( QByteArray ) ),
             mOpportunitiesPage, SLOT( slotResourceSelectionChanged( QByteArray ) ) );


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
  createTabs();
  createDockWidgets();
  createToolBars();
  setupActions();
  // initialize view actions
  slotManageItemDetailsView( 0 );
}

void SugarClient::createMenus()
{
    mViewMenu = menuBar()->addMenu( tr( "&View" ) );
}

void SugarClient::createToolBars()
{
    QToolBar *toolBar = addToolBar( tr( "&View" ) );
   toolBar->addAction( mViewAccountAction );
   toolBar->addSeparator();
   toolBar->addAction( mViewOpportunityAction );
   toolBar->addSeparator();
   toolBar->addAction( mViewContactAction );

}

void SugarClient::createDockWidgets()
{
    mContactDetailsDock = new QDockWidget(tr("Contact Details"), this );
    mContactDetailsWidget = new ContactDetails(mContactDetailsDock);
    mContactDetailsDock->setWidget( mContactDetailsWidget );
    mContactDetailsDock->setHidden( true );
    addDockWidget( Qt::BottomDockWidgetArea, mContactDetailsDock );
    mViewContactAction = mContactDetailsDock->toggleViewAction();
    mViewMenu->addAction(mViewContactAction);

    mAccountDetailsDock = new QDockWidget(tr("Account Details"), this );
    mAccountDetailsWidget = new AccountDetails(mAccountDetailsDock);
    mAccountDetailsDock->setWidget( mAccountDetailsWidget );
    mAccountDetailsDock->setHidden( true );
    addDockWidget( Qt::BottomDockWidgetArea, mAccountDetailsDock );
    mViewAccountAction = mAccountDetailsDock->toggleViewAction();
    mViewMenu->addAction( mViewAccountAction );

    mOpportunityDetailsDock = new QDockWidget(tr("Opportunity Details"), this );
    mOpportunityDetailsWidget = new OpportunityDetails(mAccountDetailsDock);
    mOpportunityDetailsDock->setWidget( mOpportunityDetailsWidget );
    mOpportunityDetailsDock->setHidden( true );
    addDockWidget( Qt::BottomDockWidgetArea, mOpportunityDetailsDock );
    mViewOpportunityAction = mOpportunityDetailsDock->toggleViewAction();
    mViewMenu->addAction( mViewOpportunityAction );

    connect( mContactsPage, SIGNAL( contactItemChanged() ),
            this, SLOT( slotContactItemChanged() ) );
    connect( mContactsPage, SIGNAL( showDetails() ),
            this, SLOT( slotShowContactDetailsDock() ) );
    connect( mAccountsPage, SIGNAL( accountItemChanged() ),
            this, SLOT( slotAccountItemChanged() ) );
    connect( mAccountsPage, SIGNAL( showDetails() ),
            this, SLOT( slotShowAccountDetailsDock() ) );
    connect( mOpportunitiesPage, SIGNAL( opportunityItemChanged() ),
            this, SLOT( slotOpportunityItemChanged() ) );
    connect( mOpportunitiesPage, SIGNAL( showDetails() ),
            this, SLOT( slotShowOpportunityDetailsDock() ) );
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
    if ( mContactDetailsDock->toggleViewAction()->isChecked() )
        mContactDetailsDock->setVisible( true );
    mContactDetailsWidget->disableGroupBoxes();
}

void SugarClient::slotAccountItemChanged()
{
      if ( mAccountDetailsDock->toggleViewAction()->isChecked() )
        mAccountDetailsDock->setVisible( true );
    mAccountDetailsWidget->disableGroupBoxes();

}

void SugarClient::slotOpportunityItemChanged()
{
      if ( mOpportunityDetailsDock->toggleViewAction()->isChecked() )
        mOpportunityDetailsDock->setVisible( true );
    mOpportunityDetailsWidget->disableGroupBoxes();

}


void SugarClient::slotShowContactDetailsDock()
{
    if ( !mContactDetailsDock->toggleViewAction()->isChecked() ) {
        mContactDetailsDock->toggleViewAction()->setChecked( true );
        mContactDetailsDock->setVisible( true );
    }
}

void SugarClient::slotShowAccountDetailsDock()
{
    if ( !mAccountDetailsDock->toggleViewAction()->isChecked() ) {
        mAccountDetailsDock->toggleViewAction()->setChecked( true );
        mAccountDetailsDock->setVisible( true );
    }

}

void SugarClient::slotShowOpportunityDetailsDock()
{
    if ( !mOpportunityDetailsDock->toggleViewAction()->isChecked() ) {
        mOpportunityDetailsDock->toggleViewAction()->setChecked( true );
        mOpportunityDetailsDock->setVisible( true );
    }

}

void SugarClient::setupActions()
{
    connect( mUi.actionSyncronize, SIGNAL( triggered() ), mContactsPage, SLOT( syncronize() ) );
    connect( mContactsPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
    connect( mUi.actionSyncronize, SIGNAL( triggered() ), mAccountsPage, SLOT( syncronize() ) );
    connect( mAccountsPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
    connect( mUi.actionSyncronize, SIGNAL( triggered() ), mOpportunitiesPage, SLOT( syncronize() ) );
    connect( mOpportunitiesPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
}

void SugarClient::slotShowMessage( const QString& message )
{
    statusBar()->showMessage( message, 3000 );
}

void SugarClient::createTabs()
{
    mAccountsPage = new AccountsPage( this );
    mUi.tabWidget->addTab( mAccountsPage, tr( "&Accounts" ) );
    mOpportunitiesPage = new OpportunitiesPage( this );
    mUi.tabWidget->addTab( mOpportunitiesPage, tr( "&Opportunities" ) );
    mContactsPage = new ContactsPage( this );
    mUi.tabWidget->addTab( mContactsPage, tr( "&Contacts" ) );

    //set Accounts page as current
    mUi.tabWidget->setCurrentIndex( 0 );
    connect( mUi.tabWidget, SIGNAL( currentChanged( int ) ),
             this, SLOT( slotManageItemDetailsView( int ) ) );
}

void SugarClient::slotManageItemDetailsView( int currentTab )
{
    if ( currentTab == 0 ) { // Accounts
        if ( mContactDetailsDock->isVisible() )
            mContactDetailsDock->setVisible( false );
        if ( mOpportunitiesPage->isVisible() )
            mOpportunitiesPage->setVisible( false );

        mViewContactAction->setEnabled( false );
        mViewOpportunityAction->setEnabled( false );
        mViewAccountAction->setEnabled( true );
    }
    else if ( currentTab == 1 ) { // Opportunities
        if ( mAccountDetailsDock->isVisible() )
            mAccountDetailsDock->setVisible( false );
        if ( mContactDetailsDock->isVisible() )
            mContactDetailsDock->setVisible( false );

        mViewAccountAction->setEnabled( false );
        mViewContactAction->setEnabled( false );
        mViewOpportunityAction->setEnabled( true );
    }
    else if ( currentTab == 2 ) { // Contacts
         if ( mAccountDetailsDock->isVisible() )
            mAccountDetailsDock->setVisible( false );
        if ( mOpportunityDetailsDock->isVisible() )
            mOpportunityDetailsDock->setVisible( false );

        mViewAccountAction->setEnabled( false );
        mViewOpportunityAction->setEnabled( false );
        mViewContactAction->setEnabled( true );
    }
}
#include "sugarclient.moc"
