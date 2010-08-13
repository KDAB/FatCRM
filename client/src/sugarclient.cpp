#include "sugarclient.h"

#include "enums.h"
#include "resourceconfigdialog.h"

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

    int selectors = mResourceSelector->count();
    if ( selectors > 1 ) // several resources
        slotLogin();
    else
        slotResourceSelectionChanged( mResourceSelector->currentIndex());
}

void SugarClient::initialize()
{
    resize( 900, 900 );
    createMenus();
    createToolBar();
    createTabs();
    createDockWidgets();
    setupActions();
    mResourceSelector = 0;
    // initialize view actions
    slotManageItemDetailsView( 0 );
    mUi.actionSyncronize->setEnabled( false );
}

void SugarClient::createMenus()
{
    mViewMenu = menuBar()->addMenu( tr( "&View" ) );
}

void SugarClient::createToolBar()
{
    mToolBar = addToolBar( QString( "Details controller" ) );
    mToolBar->addWidget( mUi.showDetails );
    mToolBar->addWidget( mUi.detachDetails );
}

void SugarClient::createDockWidgets()
{
    mContactDetailsDock = new QDockWidget(tr("Contact Details"), this );
    mContactDetailsWidget = new ContactDetails(mContactDetailsDock);
    mContactDetailsDock->setWidget( mContactDetailsWidget );
    addDockWidget( Qt::BottomDockWidgetArea, mContactDetailsDock );
    mViewContactAction = mContactDetailsDock->toggleViewAction();
    mViewMenu->addAction(mViewContactAction);

    mAccountDetailsDock = new QDockWidget(tr("Account Details"), this );
    mAccountDetailsWidget = new AccountDetails(mAccountDetailsDock);
    mAccountDetailsDock->setWidget( mAccountDetailsWidget );
    addDockWidget( Qt::BottomDockWidgetArea, mAccountDetailsDock );
    mViewAccountAction = mAccountDetailsDock->toggleViewAction();
    mViewMenu->addAction( mViewAccountAction );

    mOpportunityDetailsDock = new QDockWidget(tr("Opportunity Details"), this );
    mOpportunityDetailsWidget = new OpportunityDetails(mOpportunityDetailsDock);
    mOpportunityDetailsDock->setWidget( mOpportunityDetailsWidget );
    addDockWidget( Qt::BottomDockWidgetArea, mOpportunityDetailsDock );
    mViewOpportunityAction = mOpportunityDetailsDock->toggleViewAction();
    mViewMenu->addAction( mViewOpportunityAction );

    mLeadDetailsDock = new QDockWidget(tr("Lead Details"), this );
    mLeadDetailsWidget = new LeadDetails(mLeadDetailsDock);
    mLeadDetailsDock ->setWidget( mLeadDetailsWidget );
    addDockWidget( Qt::BottomDockWidgetArea, mLeadDetailsDock );
    mViewLeadAction = mLeadDetailsDock->toggleViewAction();
    mViewMenu->addAction( mViewLeadAction );

    mCampaignDetailsDock = new QDockWidget(tr("Campaign Details"), this );
    mCampaignDetailsWidget = new CampaignDetails(mCampaignDetailsDock);
    mCampaignDetailsDock->setWidget( mCampaignDetailsWidget );
    addDockWidget( Qt::BottomDockWidgetArea, mCampaignDetailsDock );
    mViewCampaignAction = mCampaignDetailsDock->toggleViewAction();
    mViewMenu->addAction( mViewCampaignAction );

    connect( mAccountDetailsDock, SIGNAL( visibilityChanged( bool ) ),
             this, SLOT( slotDetailsDisplayDisabled( bool ) ) );
    connect( mOpportunityDetailsDock, SIGNAL( visibilityChanged( bool ) ),
             this, SLOT( slotDetailsDisplayDisabled( bool ) ) );
    connect( mLeadDetailsDock, SIGNAL( visibilityChanged( bool ) ),
             this, SLOT( slotDetailsDisplayDisabled( bool ) ) );
    connect( mContactDetailsDock, SIGNAL( visibilityChanged( bool ) ),
             this, SLOT( slotDetailsDisplayDisabled( bool ) ) );
    connect( mCampaignDetailsDock, SIGNAL( visibilityChanged( bool ) ),
             this, SLOT( slotDetailsDisplayDisabled( bool ) ) );
}

void SugarClient::slotManageDetailsDisplay( bool value )
{
    if ( sender() == mUi.showDetails ) {
        slotManageItemDetailsView( mUi.tabWidget->currentIndex() );
        if ( value ) // initialize the detail widget fields
            emit displayDetails();
    }
    else // mUi.detachDetails
        detachDockViews( value );
}


void SugarClient::slotResourceSelectionChanged( int index )
{
    AgentInstance agent = mResourceSelector->itemData( index, AgentInstanceModel::InstanceRole ).value<AgentInstance>();
    if ( agent.isValid() ) {
        QString context = mResourceSelector->itemText( index );
        emit resourceSelected( agent.identifier().toLatin1() );
        QString contextTitle = QString ( "SugarCRM Client: " ) + context;
        setWindowTitle(  contextTitle );
        mUi.actionSyncronize->setEnabled( true );
    }
}

void SugarClient::slotReload()
{
    if ( mResourceSelector == 0 )
        return;

    int index = -1;
    for ( int i = 0; i < mResourceSelector->count(); ++i ) {
        if ( mResourceSelector->itemText( i ).contains( "SugarCRM" ) ) {
            index = i;
            break;
        }
    }
    if ( index >= 0 ) {
        AgentInstance agent = mResourceSelector->itemData( index, AgentInstanceModel::InstanceRole ).value<AgentInstance>();
        if ( agent.isValid() ) {
            emit resourceSelected( agent.identifier().toLatin1() );
            mUi.actionSyncronize->setEnabled( true );
        }
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
    connect( mUi.showDetails, SIGNAL( toggled( bool ) ), this, SLOT( slotManageDetailsDisplay( bool ) ) );
    connect( mUi.detachDetails, SIGNAL( toggled( bool ) ), this, SLOT( slotManageDetailsDisplay( bool ) ) );
    connect( mAccountsPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
    connect( mUi.actionSyncronize, SIGNAL( triggered() ), mOpportunitiesPage, SLOT( syncronize() ) );
    connect( mOpportunitiesPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
    connect( mUi.actionSyncronize, SIGNAL( triggered() ), mLeadsPage, SLOT( syncronize() ) );
    connect( mLeadsPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
    connect( mCampaignsPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
    connect( this, SIGNAL( displayDetails() ), mAccountsPage, SLOT( slotSetItem() ) );
    connect( this, SIGNAL( displayDetails() ), mCampaignsPage, SLOT( slotSetItem() ) );
    connect( this, SIGNAL( displayDetails() ), mContactsPage, SLOT( slotSetItem() ) );
    connect( this, SIGNAL( displayDetails() ), mLeadsPage, SLOT( slotSetItem() ) );
    connect( this, SIGNAL( displayDetails() ), mOpportunitiesPage, SLOT( slotSetItem() ) );
    connect( mUi.actionConfigureResources, SIGNAL( triggered() ), SLOT( slotConfigureResources() ) );
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

    bool show = mUi.showDetails->isChecked();
    if ( currentTab == 0 ) { // Accounts
        mContactDetailsDock->setVisible( false );
        mOpportunityDetailsDock->setVisible( false );
        mLeadDetailsDock->setVisible( false );
        mCampaignDetailsDock->setVisible( false );

        mAccountDetailsDock->setVisible( show );
    }
    else if ( currentTab == 1 ) { // Opportunities
        mAccountDetailsDock->setVisible( false );
        mContactDetailsDock->setVisible( false );
        mLeadDetailsDock->setVisible( false );
        mCampaignDetailsDock->setVisible( false );
        mOpportunityDetailsDock->setVisible( show );
    }
    else if ( currentTab == 2 ) { // Leads
        mAccountDetailsDock->setVisible( false );
        mContactDetailsDock->setVisible( false );
        mOpportunityDetailsDock->setVisible( false );
        mCampaignDetailsDock->setVisible( false );
        mLeadDetailsDock->setVisible( show );
    }
    else if ( currentTab == 3 ) { // Contacts
        mAccountDetailsDock->setVisible( false );
        mOpportunityDetailsDock->setVisible( false );
        mLeadDetailsDock->setVisible( false );
        mCampaignDetailsDock->setVisible( false );
        mContactDetailsDock->setVisible( show );
    }
    else if ( currentTab == 4 ) { // Campaigns
        mAccountDetailsDock->setVisible( false );
        mOpportunityDetailsDock->setVisible( false );
        mLeadDetailsDock->setVisible( false );
        mContactDetailsDock->setVisible( false );
        mCampaignDetailsDock->setVisible( show );
    }
}

void SugarClient::detachDockViews( bool value )
{
    mAccountDetailsDock->setFloating( value );
    mOpportunityDetailsDock->setFloating( value );
    mLeadDetailsDock->setFloating( value );
    mCampaignDetailsDock->setFloating( value );
    mContactDetailsDock->setFloating( value );
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

void SugarClient::slotConfigureResources()
{
    ResourceConfigDialog dialog( this );
    dialog.exec();
}

QComboBox* SugarClient::getResourcesCombo()
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

QWidget* SugarClient::detailsWidget( DetailsType type )
{
    switch( type  ) {
    case Account:
        return mAccountDetailsWidget;
    case Opportunity:
        return mOpportunityDetailsWidget;
    case Lead:
        return mLeadDetailsWidget;
    case Contact:
        return mContactDetailsWidget;
    case Campaign:
        return mCampaignDetailsWidget;
    default:
        return 0;
    }
}

void SugarClient::slotDetailsDisplayDisabled( bool value )
{
    mUi.showDetails->setChecked( value );
}

#include "sugarclient.moc"
