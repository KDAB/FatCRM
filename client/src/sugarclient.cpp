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

    mAccountDetailsDock = new QDockWidget(tr("Account Details"), this );
    mAccountDetailsWidget = new AccountDetails(mAccountDetailsDock);
    mAccountDetailsDock->setWidget( mAccountDetailsWidget );
    mAccountDetailsDock->setHidden( true );
    addDockWidget( Qt::BottomDockWidgetArea, mAccountDetailsDock );
    mViewMenu->addAction( mAccountDetailsDock->toggleViewAction() );


    connect( mContactsPage, SIGNAL( contactItemChanged() ),
            this, SLOT( slotContactItemChanged() ) );
    connect( mContactsPage, SIGNAL( showDetails() ),
            this, SLOT( slotShowContactDetailsDock() ) );
    connect( mAccountsPage, SIGNAL( accountItemChanged() ),
            this, SLOT( slotAccountItemChanged() ) );
    connect( mAccountsPage, SIGNAL( showDetails() ),
            this, SLOT( slotShowAccountDetailsDock() ) );
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

void SugarClient::setupActions()
{
    connect( mUi.actionSyncronize, SIGNAL( triggered() ), mContactsPage, SLOT( syncronize() ) );
    connect( mContactsPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
    connect( mUi.actionSyncronize, SIGNAL( triggered() ), mAccountsPage, SLOT( syncronize() ) );
    connect( mAccountsPage, SIGNAL( statusMessage( QString ) ), this, SLOT( slotShowMessage( QString ) ) );
}

void SugarClient::slotShowMessage( const QString& message )
{
    statusBar()->showMessage( message, 3000 );
}

void SugarClient::createTabs()
{
    mAccountsPage = new AccountsPage( this );
    mUi.tabWidget->addTab( mAccountsPage, tr( "&Accounts" ) );
    mContactsPage = new ContactsPage( this );
    mUi.tabWidget->addTab( mContactsPage, tr( "&Contacts" ) );

    //set contacts page as current for now
    mUi.tabWidget->setCurrentIndex( 1 );
}
#include "sugarclient.moc"
