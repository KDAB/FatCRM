#include "sugarclient.h"

#include "dbuswinidprovider.h"
#include "enums.h"
#include "resourceconfigdialog.h"
#include "shutdowndialog.h"

#include <akonadi/agentfilterproxymodel.h>
#include <akonadi/agentinstance.h>
#include <akonadi/agentinstancemodel.h>
#include <akonadi/agentmanager.h>
#include <akonadi/control.h>

#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QInputDialog>
#include <QProgressBar>
#include <QToolBar>
#include <QTimer>

using namespace Akonadi;

SugarClient::SugarClient()
    : QMainWindow(),
      mProgressBar( 0 ),
      mProgressBarHideTimer( 0 )
{
    mUi.setupUi( this );
    initialize();

    /*
     * this creates an overlay in case Akonadi is not running,
     * allowing the user to restart it
     */
    Akonadi::Control::widgetNeedsAkonadi( this );
    QMetaObject::invokeMethod( this, "slotDelayedInit", Qt::AutoConnection );

    (void)new DBusWinIdProvider( this );
}

SugarClient::~SugarClient()
{
}

void SugarClient::slotDelayedInit()
{
    Q_FOREACH( const Page *page, mPages ) {
        connect( this, SIGNAL( resourceSelected( QByteArray ) ),
                 page, SLOT( slotResourceSelectionChanged( QByteArray ) ) );
    }

    // initialize additional UI
    mResourceSelector = createResourcesCombo();

    connect( mResourceSelector, SIGNAL( currentIndexChanged( int ) ),
             this, SLOT( slotResourceSelectionChanged( int ) ) );

    mResourceDialog = new ResourceConfigDialog( this );
    connect( mResourceDialog, SIGNAL( resourceSelected( Akonadi::AgentInstance ) ),
             this, SLOT( slotResourceSelected( Akonadi::AgentInstance ) ) );

    initialResourceSelection();

    connect( AgentManager::self(), SIGNAL( instanceError( Akonadi::AgentInstance, QString ) ),
             this, SLOT( slotResourceError( Akonadi::AgentInstance, QString ) ) );
    connect( AgentManager::self(), SIGNAL( instanceOnline( Akonadi::AgentInstance, bool ) ),
             this, SLOT( slotResourceOnline( Akonadi::AgentInstance, bool ) ) );
    connect( AgentManager::self(), SIGNAL( instanceProgressChanged( Akonadi::AgentInstance ) ),
             this, SLOT( slotResourceProgress( Akonadi::AgentInstance ) ) );
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
    mUi.actionSynchronize->setEnabled( false );

    mProgressBar = new QProgressBar( this );
    mProgressBar->setRange( 0, 100 );
    mProgressBar->setMaximumWidth( 100 );
    statusBar()->addPermanentWidget( mProgressBar );
    mProgressBar->hide();

    mProgressBarHideTimer = new QTimer( this );
    mProgressBarHideTimer->setInterval( 1000 );
    connect( mProgressBarHideTimer, SIGNAL( timeout() ), mProgressBar, SLOT( hide() ) );
}

void SugarClient::createMenus()
{
    mViewMenu = menuBar()->addMenu( tr( "&View" ) );
}

void SugarClient::createToolBar()
{
    mDetailsToolBar = addToolBar( tr( "Details controller" ) );
    mDetailsToolBar->addWidget( mUi.showDetails );
    mDetailsToolBar->addWidget( mUi.detachDetails );
}

void SugarClient::createDockWidgets()
{
    mContactDetailsDock = new QDockWidget(tr("Contact Details"), this );
    mContactDetailsWidget = new DetailsWidget( Contact, mContactDetailsDock);
    mContactDetailsDock->setWidget( mContactDetailsWidget );
    addDockWidget( Qt::BottomDockWidgetArea, mContactDetailsDock );
    mViewContactAction = mContactDetailsDock->toggleViewAction();
    mViewMenu->addAction(mViewContactAction);

    mAccountDetailsDock = new QDockWidget(tr("Account Details"), this );
    mAccountDetailsWidget = new DetailsWidget( Account, mAccountDetailsDock);
    mAccountDetailsDock->setWidget( mAccountDetailsWidget );
    addDockWidget( Qt::BottomDockWidgetArea, mAccountDetailsDock );
    mViewAccountAction = mAccountDetailsDock->toggleViewAction();
    mViewMenu->addAction( mViewAccountAction );

    mOpportunityDetailsDock = new QDockWidget(tr("Opportunity Details"), this );
    mOpportunityDetailsWidget = new DetailsWidget( Opportunity, mOpportunityDetailsDock);
    mOpportunityDetailsDock->setWidget( mOpportunityDetailsWidget );
    addDockWidget( Qt::BottomDockWidgetArea, mOpportunityDetailsDock );
    mViewOpportunityAction = mOpportunityDetailsDock->toggleViewAction();
    mViewMenu->addAction( mViewOpportunityAction );

    mLeadDetailsDock = new QDockWidget(tr("Lead Details"), this );
    mLeadDetailsWidget = new DetailsWidget( Lead, mLeadDetailsDock);
    mLeadDetailsDock ->setWidget( mLeadDetailsWidget );
    addDockWidget( Qt::BottomDockWidgetArea, mLeadDetailsDock );
    mViewLeadAction = mLeadDetailsDock->toggleViewAction();
    mViewMenu->addAction( mViewLeadAction );

    mCampaignDetailsDock = new QDockWidget(tr("Campaign Details"), this );
    mCampaignDetailsWidget = new DetailsWidget( Campaign, mCampaignDetailsDock);
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
        const QString context = mResourceSelector->itemText( index );
        emit resourceSelected( agent.identifier().toLatin1() );
        const QString contextTitle =
            agent.isOnline() ? QString ( "SugarCRM Client: %1" ).arg( context )
                             : QString ( "SugarCRM Client: %1 (offline)" ).arg( context );
        setWindowTitle( contextTitle );
        mUi.actionSynchronize->setEnabled( true );
        mResourceDialog->resourceSelectionChanged( agent );
    } else {
        mUi.actionSynchronize->setEnabled( false );
    }
}

void SugarClient::slotResourceSelected( const Akonadi::AgentInstance &resource )
{
    for ( int index = 0; index < mResourceSelector->count(); ++index ) {
        const AgentInstance agent = mResourceSelector->itemData( index, AgentInstanceModel::InstanceRole ).value<AgentInstance>();
        if ( agent.isValid() && agent == resource ) {
            mResourceSelector->setCurrentIndex( index );
            return;
        }
    }
}

void SugarClient::slotReload()
{
    const AgentInstance currentAgent = currentResource();
    if ( currentAgent.isValid() ) {
        emit resourceSelected( currentAgent.identifier().toLatin1() );
        mUi.actionSynchronize->setEnabled( true );
    }
}

void SugarClient::slotSynchronize()
{
    AgentInstance currentAgent = currentResource();
    if ( currentAgent.isValid() ) {
        slotShowMessage( tr( "Synchronizing with server" ) );
        currentAgent.synchronize();
    }
}

void SugarClient::setupActions()
{
    connect( mUi.actionCRMAccounts, SIGNAL( triggered() ), SLOT( slotConfigureResources() ) );
    connect( mUi.actionReload, SIGNAL( triggered() ), this, SLOT( slotReload( ) ) );
    connect( mUi.actionSynchronize, SIGNAL( triggered() ), this, SLOT( slotSynchronize() ) );
    connect( mUi.actionQuit, SIGNAL( triggered() ), this, SLOT( close() ) );
    connect( mUi.showDetails, SIGNAL( toggled( bool ) ), this, SLOT( slotManageDetailsDisplay( bool ) ) );
    connect( mUi.detachDetails, SIGNAL( toggled( bool ) ), this, SLOT( slotManageDetailsDisplay( bool ) ) );

    Q_FOREACH( const Page *page, mPages ) {
        connect( page, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
        connect( this, SIGNAL( displayDetails() ), page, SLOT( slotSetItem() ) );
    }
}

void SugarClient::slotShowMessage( const QString& message )
{
    statusBar()->showMessage( message, 10000 );
}

void SugarClient::createTabs()
{
    Page *page = new AccountsPage( this );
    mPages << page;
    mUi.tabWidget->addTab( page, tr( "&Accounts" ) );

    page = new OpportunitiesPage( this );
    mPages << page;
    mUi.tabWidget->addTab( page, tr( "&Opportunities" ) );

    page = new LeadsPage( this );
    mPages << page;
    mUi.tabWidget->addTab( page, tr( "&Leads" ) );

    page = new ContactsPage( this );
    mPages << page;
    mUi.tabWidget->addTab( page, tr( "&Contacts" ) );

    page = new CampaignsPage( this );
    mPages << page;
    mUi.tabWidget->addTab( page, tr( "&Campaigns" ) );


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

void SugarClient::slotConfigureResources()
{
    mResourceDialog->show();
    mResourceDialog->raise();
}

QComboBox* SugarClient::createResourcesCombo()
{
    // monitor Akonadi agents so we can check for KDCRM specific resources
    AgentInstanceModel *agentModel = new AgentInstanceModel( this );
    AgentFilterProxyModel *agentFilterModel = new AgentFilterProxyModel( this );
    agentFilterModel->setSourceModel( agentModel );
    //initialize member
    QComboBox *container = new QComboBox();
    agentFilterModel->addCapabilityFilter( QString( "KDCRM" ).toLatin1() );
    container->setModel( agentFilterModel );

    QToolBar *resourceToolBar = addToolBar( tr( "CRM Account Selection" ) );
    resourceToolBar->addWidget( container );

    return container;
}

DetailsWidget* SugarClient::detailsWidget( DetailsType type )
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

void SugarClient::closeEvent( QCloseEvent *event )
{
    const ShutdownDialog::ShutdownOptions option = ShutdownDialog::getShutdownOption( this );
    switch ( option ) {
        case ShutdownDialog::CancelShutdown:
            event->ignore();
            return;

        case ShutdownDialog::JustQuit:
            break;

        case ShutdownDialog::QuitAndStopAkonadi:
            Akonadi::Control::stop();
            break;

        case ShutdownDialog::QuitAndAgentsOffline:
            for ( int i = 0; i < mResourceSelector->count(); ++i ) {
                AgentInstance agent = mResourceSelector->itemData( i, AgentInstanceModel::InstanceRole ).value<AgentInstance>();
                if ( agent.isValid() ) {
                    agent.setIsOnline( false );
                }
            }
            break;
    }

    QMainWindow::closeEvent( event );
}

void SugarClient::slotDetailsDisplayDisabled( bool value )
{
    mUi.showDetails->setChecked( value );
}

void SugarClient::slotResourceError( const AgentInstance &resource, const QString &message )
{
    const AgentInstance currentAgent = currentResource();
    if ( currentAgent.isValid() && currentAgent.identifier() == resource.identifier() ) {
        slotShowMessage( message );
    }
}

void SugarClient::slotResourceOnline( const AgentInstance &resource, bool online )
{
    const AgentInstance currentAgent = currentResource();
    if ( currentAgent.isValid() && currentAgent.identifier() == resource.identifier() ) {
        const int index = mResourceSelector->currentIndex();
        const QString context = mResourceSelector->itemText( index );
        const QString contextTitle =
            online ? QString ( "SugarCRM Client: %1" ).arg( context )
                   : QString ( "SugarCRM Client: %1 (offline)" ).arg( context );
        setWindowTitle( contextTitle );
    }
}

void SugarClient::slotResourceProgress( const AgentInstance &resource )
{
    const AgentInstance currentAgent = currentResource();
    if ( currentAgent.isValid() && currentAgent.identifier() == resource.identifier() ) {
        const int progress = resource.progress();
        const QString message = resource.statusMessage();

        mProgressBar->show();
        mProgressBar->setValue( progress );
        if ( progress == 100 ) {
            mProgressBarHideTimer->start();
        } else {
            mProgressBarHideTimer->stop();
        }
        statusBar()->showMessage( message, mProgressBarHideTimer->interval() );
    }
}

AgentInstance SugarClient::currentResource() const
{
    const int index = mResourceSelector->currentIndex();
    return mResourceSelector->itemData( index, AgentInstanceModel::InstanceRole ).value<AgentInstance>();
}

void SugarClient::initialResourceSelection()
{
    const int selectors = mResourceSelector->count();
    if ( selectors == 1 ) {
        slotResourceSelectionChanged( mResourceSelector->currentIndex());
    } else {
        mResourceDialog->show();
        mResourceDialog->raise();
    }
}

void SugarClient::displayDockWidgets( bool value )
{
    if ( mUi.showDetails->isChecked() != value )
        mUi.showDetails->setChecked( value );
}

#include "sugarclient.moc"
