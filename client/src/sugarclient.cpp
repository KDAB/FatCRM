#include "sugarclient.h"


#include <akonadi/agentfilterproxymodel.h>
#include <akonadi/agentinstance.h>
#include <akonadi/agentinstancemodel.h>
#include <akonadi/control.h>

#include <QDockWidget>
#include <QInputDialog>
#include <QToolBar>

using namespace Akonadi;

SugarClient::SugarClient()
    : QMainWindow()
{
    mUi.setupUi( this );
    initialize();

    /*
     * this creates an overlay in case Akonadi is not running,
     * allowing the user to restart it
     */
    Akonadi::Control::widgetNeedsAkonadi( this );
    QMetaObject::invokeMethod( this, "slotDelayedInit", Qt::AutoConnection );

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
    connect( this, SIGNAL( resourceSelected( QByteArray ) ),
             mLeadsPage, SLOT( slotResourceSelectionChanged( QByteArray ) ) );
    connect( this, SIGNAL( resourceSelected( QByteArray ) ),
             mCampaignsPage, SLOT( slotResourceSelectionChanged( QByteArray ) ) );

    //initialize member
    mResourceSelector = getResourcesCombo();

    connect( mResourceSelector, SIGNAL( currentIndexChanged( int ) ),
             this, SLOT( slotResourceSelectionChanged( int ) ) );

    if ( mResourceSelector->count() > 1 ) // several resources
        slotLogin();
    else
        slotResourceSelectionChanged( mResourceSelector->currentIndex());
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
   toolBar->addAction( mViewLeadAction );
   toolBar->addSeparator();
   toolBar->addAction( mViewContactAction );
   toolBar->addSeparator();
   toolBar->addAction( mViewCampaignAction );
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
    mOpportunityDetailsWidget = new OpportunityDetails(mOpportunityDetailsDock);
    mOpportunityDetailsDock->setWidget( mOpportunityDetailsWidget );
    mOpportunityDetailsDock->setHidden( true );
    addDockWidget( Qt::BottomDockWidgetArea, mOpportunityDetailsDock );
    mViewOpportunityAction = mOpportunityDetailsDock->toggleViewAction();
    mViewMenu->addAction( mViewOpportunityAction );

    mLeadDetailsDock = new QDockWidget(tr("Lead Details"), this );
    mLeadDetailsWidget = new LeadDetails(mLeadDetailsDock);
    mLeadDetailsDock->setWidget( mLeadDetailsWidget );
    mLeadDetailsDock->setHidden( true );
    addDockWidget( Qt::BottomDockWidgetArea, mLeadDetailsDock );
    mViewLeadAction = mLeadDetailsDock->toggleViewAction();
    mViewMenu->addAction( mViewLeadAction );

    mCampaignDetailsDock = new QDockWidget(tr("Campaign Details"), this );
    mCampaignDetailsWidget = new CampaignDetails(mCampaignDetailsDock);
    mCampaignDetailsDock->setWidget( mCampaignDetailsWidget );
    mCampaignDetailsDock->setHidden( true );
    addDockWidget( Qt::BottomDockWidgetArea, mCampaignDetailsDock );
    mViewCampaignAction = mCampaignDetailsDock->toggleViewAction();
    mViewMenu->addAction( mViewCampaignAction );

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
    connect( mLeadsPage, SIGNAL( leadItemChanged() ),
            this, SLOT( slotLeadItemChanged() ) );
    connect( mLeadsPage, SIGNAL( showDetails() ),
            this, SLOT( slotShowLeadDetailsDock() ) );
    connect( mCampaignsPage, SIGNAL( campaignItemChanged() ),
            this, SLOT( slotCampaignItemChanged() ) );
    connect( mCampaignsPage, SIGNAL( showDetails() ),
            this, SLOT( slotShowCampaignDetailsDock() ) );
}

void SugarClient::slotResourceSelectionChanged( int index )
{
    AgentInstance agent = mResourceSelector->itemData( index, AgentInstanceModel::InstanceRole ).value<AgentInstance>();
    if ( agent.isValid() ) {
        QString context = mResourceSelector->itemText( index );
        emit resourceSelected( agent.identifier().toLatin1() );
        QString contextTitle = QString ( "SugarCRM Client: " ) + "(" + context + ")";
        setWindowTitle(  contextTitle );
    }
}

void SugarClient::slotReload()
{
    int index = -1;
    for ( int i = 0; i < mResourceSelector->count(); ++i ) {
        if ( mResourceSelector->itemText( i ).contains( "SugarCRM" ) ) {
            index = i;
            break;
        }
    }
    if ( index >= 0 ) {
        AgentInstance agent = mResourceSelector->itemData( index, AgentInstanceModel::InstanceRole ).value<AgentInstance>();
        if ( agent.isValid() )
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

void SugarClient::slotLeadItemChanged()
{
      if ( mLeadDetailsDock->toggleViewAction()->isChecked() )
        mLeadDetailsDock->setVisible( true );
    mLeadDetailsWidget->disableGroupBoxes();
}

void SugarClient::slotCampaignItemChanged()
{
      if ( mCampaignDetailsDock->toggleViewAction()->isChecked() )
        mCampaignDetailsDock->setVisible( true );
    mCampaignDetailsWidget->disableGroupBoxes();
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

void SugarClient::slotShowLeadDetailsDock()
{
    if ( !mLeadDetailsDock->toggleViewAction()->isChecked() ) {
        mLeadDetailsDock->toggleViewAction()->setChecked( true );
        mLeadDetailsDock->setVisible( true );
    }

}

void SugarClient::slotShowCampaignDetailsDock()
{
    if ( !mCampaignDetailsDock->toggleViewAction()->isChecked() ) {
        mCampaignDetailsDock->toggleViewAction()->setChecked( true );
        mCampaignDetailsDock->setVisible( true );
    }

}

void SugarClient::setupActions()
{
    connect( mUi.actionLogin, SIGNAL( triggered() ), this, SLOT( slotLogin() ) );
    connect( mUi.actionSyncronize, SIGNAL( triggered() ), mContactsPage, SLOT( syncronize() ) );
    connect( mUi.actionReload, SIGNAL( triggered() ), this, SLOT( slotReload( ) ) );
    connect( mUi.actionQuit, SIGNAL( triggered() ), qApp, SLOT( quit() ) );
    connect( mContactsPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
    connect( mUi.actionSyncronize, SIGNAL( triggered() ), mAccountsPage, SLOT( syncronize() ) );
    connect( mAccountsPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
    connect( mUi.actionSyncronize, SIGNAL( triggered() ), mOpportunitiesPage, SLOT( syncronize() ) );
    connect( mOpportunitiesPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
    connect( mUi.actionSyncronize, SIGNAL( triggered() ), mLeadsPage, SLOT( syncronize() ) );
    connect( mLeadsPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
    connect( mCampaignsPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
}

void SugarClient::slotShowMessage( const QString& message )
{
    statusBar()->showMessage( message, 10000 );
}

void SugarClient::createTabs()
{
    mAccountsPage = new AccountsPage( this );
    mUi.tabWidget->addTab( mAccountsPage, tr( "&Accounts" ) );
    mOpportunitiesPage = new OpportunitiesPage( this );
    mUi.tabWidget->addTab( mOpportunitiesPage, tr( "&Opportunities" ) );
    mLeadsPage = new LeadsPage( this );
    mUi.tabWidget->addTab( mLeadsPage, tr( "&Leads" ) );
    mContactsPage = new ContactsPage( this );
    mUi.tabWidget->addTab( mContactsPage, tr( "&Contacts" ) );
    mCampaignsPage = new CampaignsPage( this );
    mUi.tabWidget->addTab( mCampaignsPage, tr( "&Campaigns" ) );


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
        if ( mOpportunityDetailsDock->isVisible() )
            mOpportunityDetailsDock->setVisible( false );
        if ( mLeadDetailsDock->isVisible() )
            mLeadDetailsDock->setVisible( false );
        if ( mCampaignDetailsDock->isVisible() )
            mCampaignDetailsDock->setVisible( false );

        mViewContactAction->setEnabled( false );
        mViewOpportunityAction->setEnabled( false );
        mViewLeadAction->setEnabled( false );
        mViewCampaignAction->setEnabled( false );
        mViewAccountAction->setEnabled( true );
    }
    else if ( currentTab == 1 ) { // Opportunities
        if ( mAccountDetailsDock->isVisible() )
            mAccountDetailsDock->setVisible( false );
        if ( mContactDetailsDock->isVisible() )
            mContactDetailsDock->setVisible( false );
        if ( mLeadDetailsDock->isVisible() )
            mLeadDetailsDock->setVisible( false );
        if ( mCampaignDetailsDock->isVisible() )
            mCampaignDetailsDock->setVisible( false );

        mViewAccountAction->setEnabled( false );
        mViewContactAction->setEnabled( false );
        mViewLeadAction->setEnabled( false );
        mViewCampaignAction->setEnabled( false );
        mViewOpportunityAction->setEnabled( true );
    }
    else if ( currentTab == 2 ) { // Leads
        if ( mAccountDetailsDock->isVisible() )
            mAccountDetailsDock->setVisible( false );
        if ( mContactDetailsDock->isVisible() )
            mContactDetailsDock->setVisible( false );
        if ( mOpportunityDetailsDock->isVisible() )
            mOpportunityDetailsDock->setVisible( false );
        if ( mCampaignDetailsDock->isVisible() )
            mCampaignDetailsDock->setVisible( false );

        mViewAccountAction->setEnabled( false );
        mViewContactAction->setEnabled( false );
        mViewOpportunityAction->setEnabled( false );
        mViewCampaignAction->setEnabled( false );
        mViewLeadAction->setEnabled( true );
    }
    else if ( currentTab == 3 ) { // Contacts
        if ( mAccountDetailsDock->isVisible() )
            mAccountDetailsDock->setVisible( false );
        if ( mOpportunityDetailsDock->isVisible() )
            mOpportunityDetailsDock->setVisible( false );
        if ( mLeadDetailsDock->isVisible() )
            mLeadDetailsDock->setVisible( false );
        if ( mCampaignDetailsDock->isVisible() )
            mCampaignDetailsDock->setVisible( false );

        mViewAccountAction->setEnabled( false );
        mViewOpportunityAction->setEnabled( false );
        mViewLeadAction->setEnabled( false );
        mViewCampaignAction->setEnabled( false );
        mViewContactAction->setEnabled( true );
    }
    else if ( currentTab == 4 ) { // Campaigns
        if ( mAccountDetailsDock->isVisible() )
            mAccountDetailsDock->setVisible( false );
        if ( mOpportunityDetailsDock->isVisible() )
            mOpportunityDetailsDock->setVisible( false );
        if ( mLeadDetailsDock->isVisible() )
            mLeadDetailsDock->setVisible( false );
        if ( mContactDetailsDock->isVisible() )
            mContactDetailsDock->setVisible( false );

        mViewAccountAction->setEnabled( false );
        mViewOpportunityAction->setEnabled( false );
        mViewLeadAction->setEnabled( false );
        mViewContactAction->setEnabled( false );
        mViewCampaignAction->setEnabled( true );
    }
}

void SugarClient::slotLogin()
{
    QStringList items;
    for ( int i = 0; i < mResourceSelector->count(); ++i )
        items << mResourceSelector->itemText( i );
    bool ok;
    QString item = QInputDialog::getItem( this, "Select Sugar Resource",
                                          "Resource: ", items, 0, false,
                                          &ok );
    if ( ok && !item.isEmpty() )
        mResourceSelector->setCurrentIndex( mResourceSelector->findText( item ) );

}


QComboBox* SugarClient::getResourcesCombo( )
{
    // monitor Akonadi agents so we can check for SugarCRM specific resources
    AgentInstanceModel *agentModel = new AgentInstanceModel( this );
    AgentFilterProxyModel *agentFilterModel = new AgentFilterProxyModel( this );
    agentFilterModel->setSourceModel( agentModel );
    //initialize member
    QComboBox *container = new QComboBox();
    agentFilterModel->addCapabilityFilter( QString( "SugarCRM" ).toLatin1() );
    container->setModel( agentFilterModel );

    return container;
}
#include "sugarclient.moc"
