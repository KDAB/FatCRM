#include "sugarclient.h"

#include "dbuswinidprovider.h"
#include "enums.h"
#include "resourceconfigdialog.h"
#include "clientsettings.h"
#include "configurationdialog.h"
#include "collectionmanager.h"
#include "notesrepository.h"

#include <akonadi/agentfilterproxymodel.h>
#include <akonadi/agentinstance.h>
#include <akonadi/agentinstancemodel.h>
#include <akonadi/agentmanager.h>
#include <akonadi/control.h>

#include <QCloseEvent>
#include <QCheckBox>
#include <QComboBox>
#include <QDockWidget>
#include <QInputDialog>
#include <QProgressBar>
#include <QToolBar>
#include <QTimer>

using namespace Akonadi;

SugarClient::SugarClient()
    : QMainWindow(),
      mProgressBar(0),
      mProgressBarHideTimer(0),
      mCollectionManager(new CollectionManager(this)),
      mNotesRepository(new NotesRepository(this))
{
    mUi.setupUi(this);
    initialize();

    /*
     * this creates an overlay in case Akonadi is not running,
     * allowing the user to restart it
     */
    Akonadi::Control::widgetNeedsAkonadi(this);
    QMetaObject::invokeMethod(this, "slotDelayedInit", Qt::AutoConnection);

    (void)new DBusWinIdProvider(this);

     ClientSettings::self()->restoreWindowSize("main", this);
}

SugarClient::~SugarClient()
{
    ClientSettings::self()->saveWindowSize("main", this);
}

void SugarClient::slotDelayedInit()
{
    Q_FOREACH (const Page *page, mPages) {
        connect(this, SIGNAL(resourceSelected(QByteArray)),
                page, SLOT(slotResourceSelectionChanged(QByteArray)));
        connect(page, SIGNAL(ignoreModifications(bool)),
                this, SLOT(slotIgnoreModifications(bool)));
    }

    // initialize additional UI
    mResourceSelector = createResourcesCombo();

    connect(mResourceSelector, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotResourceSelectionChanged(int)));

    mResourceDialog = new ResourceConfigDialog(this);
    connect(mResourceDialog, SIGNAL(resourceSelected(Akonadi::AgentInstance)),
            this, SLOT(slotResourceSelected(Akonadi::AgentInstance)));

    connect(mCollectionManager, SIGNAL(collectionResult(QString,Akonadi::Collection)),
            this, SLOT(slotCollectionResult(QString,Akonadi::Collection)));

    initialResourceSelection();

    connect(AgentManager::self(), SIGNAL(instanceError(Akonadi::AgentInstance,QString)),
            this, SLOT(slotResourceError(Akonadi::AgentInstance,QString)));
    connect(AgentManager::self(), SIGNAL(instanceOnline(Akonadi::AgentInstance,bool)),
            this, SLOT(slotResourceOnline(Akonadi::AgentInstance,bool)));
    connect(AgentManager::self(), SIGNAL(instanceProgressChanged(Akonadi::AgentInstance)),
            this, SLOT(slotResourceProgress(Akonadi::AgentInstance)));
    connect(AgentManager::self(), SIGNAL(instanceStatusChanged(Akonadi::AgentInstance)),
            this, SLOT(slotResourceProgress(Akonadi::AgentInstance)));
}

void SugarClient::initialize()
{
    Q_INIT_RESOURCE(icons);

    resize(900, 900);
    createActions();
    createTabs();
    setupActions();
    mResourceSelector = 0;
    // initialize view actions
    mUi.actionSynchronize->setEnabled(false);
    mUi.actionOfflineMode->setEnabled(false);

    mProgressBar = new QProgressBar(this);
    mProgressBar->setRange(0, 100);
    mProgressBar->setMaximumWidth(100);
    statusBar()->addPermanentWidget(mProgressBar);
    mProgressBar->hide();

    mProgressBarHideTimer = new QTimer(this);
    mProgressBarHideTimer->setInterval(1000);
    connect(mProgressBarHideTimer, SIGNAL(timeout()), mProgressBar, SLOT(hide()));
}

void SugarClient::createActions()
{
    // the File menu is handled in Qt Designer

    mViewMenu = menuBar()->addMenu(tr("&View"));
    QAction *printAction = new QAction(tr("Print Report..."), this);
    printAction->setShortcut(QKeySequence::Print);
    printAction->setIcon(QIcon(":/icons/document-print-preview.png"));
    connect(printAction, SIGNAL(triggered()), this, SLOT(slotPrintReport()));
    mViewMenu->addAction(printAction);
    mViewMenu->addSeparator();

    mSettingsMenu = menuBar()->addMenu(tr("&Settings"));
    QAction *configureAction = new QAction(tr("Configure FatCRM..."), this);
    connect(configureAction, SIGNAL(triggered()), this, SLOT(slotConfigure()));
    mSettingsMenu->addAction(configureAction);

    QToolBar *detailsToolBar = addToolBar(tr("Details Toolbar"));
    mShowDetails = new QCheckBox(tr("Show Details"));
    detailsToolBar->addWidget(mShowDetails);
    connect(mShowDetails, SIGNAL(toggled(bool)), SLOT(slotShowDetails(bool)));
    detailsToolBar->addAction(printAction);
}

void SugarClient::slotResourceSelectionChanged(int index)
{
    AgentInstance agent = mResourceSelector->itemData(index, AgentInstanceModel::InstanceRole).value<AgentInstance>();
    if (agent.isValid()) {
        const QString context = mResourceSelector->itemText(index);
        const QByteArray identifier = agent.identifier().toLatin1();
        emit resourceSelected(identifier);
        const QString contextTitle =
            agent.isOnline() ? QString("SugarCRM Client: %1").arg(context)
            : QString("SugarCRM Client: %1 (offline)").arg(context);
        setWindowTitle(contextTitle);
        mUi.actionSynchronize->setEnabled(true);
        mUi.actionOfflineMode->setEnabled(true);
        mUi.actionOfflineMode->setChecked(!agent.isOnline());
        mResourceDialog->resourceSelectionChanged(agent);
        slotResourceProgress(agent);
        mCollectionManager->setResource(identifier);
    } else {
        mUi.actionSynchronize->setEnabled(false);
        mUi.actionSynchronize->setEnabled(false);
    }
}

void SugarClient::slotResourceSelected(const Akonadi::AgentInstance &resource)
{
    for (int index = 0; index < mResourceSelector->count(); ++index) {
        const AgentInstance agent = mResourceSelector->itemData(index, AgentInstanceModel::InstanceRole).value<AgentInstance>();
        if (agent.isValid() && agent == resource) {
            mResourceSelector->setCurrentIndex(index);
            return;
        }
    }
}

void SugarClient::slotToggleOffline(bool offline)
{
    AgentInstance currentAgent = currentResource();
    const bool online = !offline;
    if (currentAgent.isValid() && currentAgent.isOnline() != online) {
        kDebug() << "setting agent" << currentAgent.identifier() << "to online=" << online;
        currentAgent.setIsOnline(online);
    }
}

void SugarClient::slotSynchronize()
{
    AgentInstance currentAgent = currentResource();
    if (currentAgent.isValid()) {
        slotShowMessage(tr("Synchronizing with server"));
        if (!currentAgent.isOnline())
            currentAgent.setIsOnline(true);
        currentAgent.synchronize();
    }
}

void SugarClient::slotSynchronizeCollection(const Collection &collection)
{
    AgentInstance currentAgent = currentResource();
    if (currentAgent.isValid()) {
        slotShowMessage(tr("Synchronizing '%1' with server").arg(collection.name()));
        if (!currentAgent.isOnline())
            currentAgent.setIsOnline(true);
        AgentManager::self()->synchronizeCollection(collection);
    }
}

void SugarClient::setupActions()
{
    const QIcon reloadIcon =
        (style() != 0 ? style()->standardIcon(QStyle::SP_BrowserReload, 0, 0)
         : QIcon());
    if (!reloadIcon.isNull()) {
        mUi.actionSynchronize->setIcon(reloadIcon);
    }

    connect(mUi.actionCRMAccounts, SIGNAL(triggered()), SLOT(slotConfigureResources()));
    connect(mUi.actionOfflineMode, SIGNAL(toggled(bool)), this, SLOT(slotToggleOffline(bool)));
    connect(mUi.actionSynchronize, SIGNAL(triggered()), this, SLOT(slotSynchronize()));
    connect(mUi.actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    Q_FOREACH (const Page *page, mPages) {
        connect(page, SIGNAL(statusMessage(QString)), this, SLOT(slotShowMessage(QString)));
        connect(this, SIGNAL(displayDetails()), page, SLOT(slotSetItem()));
        connect(page, SIGNAL(showDetailsChanged(bool)), this, SLOT(slotPageShowDetailsChanged()));
        connect(page, SIGNAL(synchronizeCollection(Akonadi::Collection)), this, SLOT(slotSynchronizeCollection(Akonadi::Collection)));
    }
}

void SugarClient::slotShowMessage(const QString &message)
{
    kDebug() << message;
    statusBar()->showMessage(message);
}

void SugarClient::createTabs()
{
    Page *page = new AccountsPage(this);
    mPages << page;
    mUi.tabWidget->addTab(page, tr("&Accounts"));
    mViewMenu->addAction(page->showDetailsAction(tr("&Account Details")));

    page = new OpportunitiesPage(this);
    page->setNotesRepository(mNotesRepository);
    mPages << page;
    mUi.tabWidget->addTab(page, tr("&Opportunities"));
    mViewMenu->addAction(page->showDetailsAction(tr("&Opportunity Details")));

#if 0
    page = new LeadsPage(this);
    mPages << page;
    mUi.tabWidget->addTab(page, tr("&Leads"));
    mViewMenu->addAction(page->showDetailsAction(tr("&Lead Details")));
#endif

    page = new ContactsPage(this);
    mPages << page;
    mUi.tabWidget->addTab(page, tr("&Contacts"));
    mViewMenu->addAction(page->showDetailsAction(tr("&Contact Details")));

#if 0
    page = new CampaignsPage(this);
    mPages << page;
    mUi.tabWidget->addTab(page, tr("&Campaigns"));
    mViewMenu->addAction(page->showDetailsAction(tr("C&ampaign Details")));
#endif

    connect(mUi.tabWidget, SIGNAL(currentChanged(int)), SLOT(slotCurrentTabChanged(int)));

    //set Opportunities page as current
    mShowDetails->setChecked(mPages[ 1 ]->showsDetails());
    mUi.tabWidget->setCurrentIndex(1);
}

void SugarClient::slotConfigureResources()
{
    mResourceDialog->show();
    mResourceDialog->raise();
}

QComboBox *SugarClient::createResourcesCombo()
{
    QComboBox *container = new QComboBox();

    // monitor Akonadi agents so we can check for KDCRM specific resources
    AgentInstanceModel *agentModel = new AgentInstanceModel(this);
    AgentFilterProxyModel *agentFilterModel = new AgentFilterProxyModel(this);
    agentFilterModel->addCapabilityFilter(QString("KDCRM").toLatin1());
    agentFilterModel->setSourceModel(agentModel);

    // Remove this and use agentFilterModel on the last line when everyone has kdepimlibs >= 4.14.7
#if 1
    WorkaroundFilterProxyModel *workaround = new WorkaroundFilterProxyModel(this);
    workaround->setSourceModel(agentFilterModel);
    container->setModel(workaround);
#endif


    QToolBar *resourceToolBar = addToolBar(tr("CRM Account Selection"));
    resourceToolBar->addWidget(container);
    resourceToolBar->addAction(mUi.actionSynchronize);

    return container;
}

void SugarClient::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
}

void SugarClient::slotResourceError(const AgentInstance &resource, const QString &message)
{
    const AgentInstance currentAgent = currentResource();
    if (currentAgent.isValid() && currentAgent.identifier() == resource.identifier()) {
        slotShowMessage(message);
    }
}

void SugarClient::slotResourceOnline(const AgentInstance &resource, bool online)
{
    const AgentInstance currentAgent = currentResource();
    if (currentAgent.isValid() && currentAgent.identifier() == resource.identifier()) {
        const int index = mResourceSelector->currentIndex();
        const QString context = mResourceSelector->itemText(index);
        const QString contextTitle =
            online ? QString("SugarCRM Client: %1").arg(context)
            : QString("SugarCRM Client: %1 (offline)").arg(context);
        setWindowTitle(contextTitle);
        mUi.actionOfflineMode->setChecked(!online);
    }
}

void SugarClient::slotResourceProgress(const AgentInstance &resource)
{
    const AgentInstance currentAgent = currentResource();
    if (currentAgent.isValid() && currentAgent.identifier() == resource.identifier()) {
        const int progress = resource.progress();
        const QString message = resource.statusMessage();

        if (resource.status() == AgentInstance::Broken
                || resource.status() == AgentInstance::NotConfigured
                || !resource.isOnline()) {
            mProgressBar->hide();
        } else {
            kDebug() << progress << message;
            mProgressBar->show();
            mProgressBar->setValue(progress);
            if (progress == 100) {
                mProgressBarHideTimer->start();
            } else {
                mProgressBarHideTimer->stop();
            }
        }
        if (!message.isEmpty()) {
            statusBar()->showMessage(message);
        }
    }
}

void SugarClient::slotShowDetails(bool on)
{
    mPages[ mUi.tabWidget->currentIndex() ]->showDetails(on);
}

void SugarClient::slotPageShowDetailsChanged()
{
    mShowDetails->setChecked(currentPage()->showsDetails());
}

void SugarClient::slotCurrentTabChanged(int index)
{
    mShowDetails->setChecked(mPages[ index ]->showsDetails());
}

void SugarClient::slotConfigure()
{
    ConfigurationDialog dlg;
    dlg.setFullUserName(ClientSettings::self()->fullUserName());
    dlg.setAssigneeFilters(ClientSettings::self()->assigneeFilters());
    dlg.setCountryFilters(ClientSettings::self()->countryFilters());
    if (dlg.exec()) {
        ClientSettings::self()->setFullUserName(dlg.fullUserName());
        ClientSettings::self()->setAssigneeFilters(dlg.assigneeFilters());
        ClientSettings::self()->setCountryFilters(dlg.countryFilters());
        ClientSettings::self()->sync();
    }
}

void SugarClient::slotPrintReport()
{
    currentPage()->printReport();
}

void SugarClient::slotCollectionResult(const QString &mimeType, const Collection &collection)
{
    foreach(Page *page, mPages) {
        if (page->mimeType() == mimeType) {
            page->setCollection(collection);
            return;
        }
    }
    if (mimeType == "application/x-vnd.kdab.crm.note") {
        mNotesRepository->setNotesCollection(collection);
    } else if (mimeType == "application/x-vnd.kdab.crm.email") {
        mNotesRepository->setEmailsCollection(collection);
    }

}

void SugarClient::slotIgnoreModifications(bool ignore)
{
    foreach(Page *page, mPages) {
        page->setModificationsIgnored(ignore);
    }
}

Page *SugarClient::currentPage() const
{
    return mPages[ mUi.tabWidget->currentIndex() ];
}

AgentInstance SugarClient::currentResource() const
{
    const int index = mResourceSelector->currentIndex();
    return mResourceSelector->itemData(index, AgentInstanceModel::InstanceRole).value<AgentInstance>();
}

void SugarClient::initialResourceSelection()
{
    const int selectors = mResourceSelector->count();
    if (selectors == 1) {
        slotResourceSelectionChanged(mResourceSelector->currentIndex());
    } else {
        mResourceSelector->setCurrentIndex(-1);
        mResourceDialog->show();
        mResourceDialog->raise();
    }
}

#include "sugarclient.moc"
