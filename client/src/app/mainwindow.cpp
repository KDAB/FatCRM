/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"

#include "accountimportdialog.h"
#include "accountrepository.h"
#include "clientsettings.h"
#include "collectionmanager.h"
#include "configurationdialog.h"
#include "contactsimporter.h"
#include "dbuswinidprovider.h"
#include "enums.h"
#include "fatcrm_version.h"
#include "notesrepository.h"
#include "referenceddata.h"
#include "reportpage.h"
#include "resourceconfigdialog.h"

#include "kdcrmdata/enumdefinitionattribute.h"

#include <AkonadiCore/AgentFilterProxyModel>
#include <AkonadiCore/AgentInstance>
#include <AkonadiCore/AgentInstanceModel>
#include <AkonadiCore/AgentManager>
#include <AkonadiCore/AttributeFactory>
#include <AkonadiWidgets/ControlGui>
#include <AkonadiCore/ServerManager>
using namespace Akonadi;

#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QTimer>
#include <QToolBar>

#include <KDebug>

MainWindow::MainWindow()
    : QMainWindow(),
      mShowDetails(0),
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
    Akonadi::ControlGui::widgetNeedsAkonadi(this);
    QMetaObject::invokeMethod(this, "slotDelayedInit", Qt::AutoConnection);

    (void)new DBusWinIdProvider(this);

     ClientSettings::self()->restoreWindowSize("main", this);
}

MainWindow::~MainWindow()
{
    ClientSettings::self()->saveWindowSize("main", this);
}

void MainWindow::slotDelayedInit()
{
    Q_FOREACH (const Page *page, mPages) {
        connect(this, SIGNAL(resourceSelected(QByteArray)),
                page, SLOT(slotResourceSelectionChanged(QByteArray)));
        connect(page, SIGNAL(ignoreModifications(bool)),
                this, SLOT(slotIgnoreModifications(bool)));
    }

    // initialize additional UI

    setupResourcesCombo();

    mResourceDialog = new ResourceConfigDialog(this);
    connect(mResourceDialog, SIGNAL(resourceSelected(Akonadi::AgentInstance)),
            this, SLOT(slotResourceSelected(Akonadi::AgentInstance)));

    connect(mCollectionManager, SIGNAL(collectionResult(QString,Akonadi::Collection)),
            this, SLOT(slotCollectionResult(QString,Akonadi::Collection)));

    initialResourceSelection();

    connect(Akonadi::ServerManager::self(), SIGNAL(started()),
            SLOT(slotServerStarted()));

    connect(AgentManager::self(), SIGNAL(instanceError(Akonadi::AgentInstance,QString)),
            this, SLOT(slotResourceError(Akonadi::AgentInstance,QString)));
    connect(AgentManager::self(), SIGNAL(instanceOnline(Akonadi::AgentInstance,bool)),
            this, SLOT(slotResourceOnline(Akonadi::AgentInstance,bool)));
    connect(AgentManager::self(), SIGNAL(instanceProgressChanged(Akonadi::AgentInstance)),
            this, SLOT(slotResourceProgress(Akonadi::AgentInstance)));
    connect(AgentManager::self(), SIGNAL(instanceStatusChanged(Akonadi::AgentInstance)),
            this, SLOT(slotResourceProgress(Akonadi::AgentInstance)));

    Akonadi::AttributeFactory::registerAttribute<EnumDefinitionAttribute>();
}

void MainWindow::slotAboutApp()
{
    QMessageBox::about(this, i18n("About FatCRM"), i18n("A desktop application for SugarCRM\n\nVersion %1\n\n(C) 2010-2015 Klarälvdalens Datakonsult AB (KDAB)", QString(s_version)));
}

void MainWindow::initialize()
{
    Q_INIT_RESOURCE(icons);

    resize(900, 900);
    createActions();
    createTabs();
    setupActions();
    // initialize view actions
    mUi.actionSynchronize->setEnabled(false);
    mUi.actionFullReload->setEnabled(false);
    mUi.actionOfflineMode->setEnabled(false);

    mProgressBar = new QProgressBar(this);
    mProgressBar->setRange(0, 100);
    mProgressBar->setMaximumWidth(100);
    statusBar()->addPermanentWidget(mProgressBar);
    mProgressBar->hide();

    mProgressBarHideTimer = new QTimer(this);
    mProgressBarHideTimer->setInterval(1000);
    connect(mProgressBarHideTimer, SIGNAL(timeout()), mProgressBar, SLOT(hide()));

    connect(mNotesRepository, SIGNAL(notesLoaded(int)),
            this, SLOT(slotNotesLoaded(int)));
    connect(mNotesRepository, SIGNAL(emailsLoaded(int)),
            this, SLOT(slotEmailsLoaded(int)));
}

void MainWindow::createActions()
{
    // the other menus are handled in Qt Designer

    mViewMenu = new QMenu(tr("&View"), this);
    menuBar()->insertMenu(mUi.menuSettings->menuAction(), mViewMenu);
    QAction *printAction = new QAction(tr("Print Report..."), this);
    printAction->setShortcut(QKeySequence::Print);
    printAction->setIcon(QIcon(":/icons/document-print-preview.png"));
    connect(printAction, SIGNAL(triggered()), this, SLOT(slotPrintReport()));
    mViewMenu->addAction(printAction);
    mViewMenu->addSeparator();

    mMainToolBar = addToolBar(tr("Main ToolBar"));
    mResourceSelector = new QComboBox(this);
    mResourceSelectorAction = mMainToolBar->addWidget(mResourceSelector);
    mMainToolBar->addAction(mUi.actionSynchronize);

    mShowDetails = new QCheckBox(tr("Show Details"));
    // A checkbox in a toolbar looks weird; and there are menu items for this already
    //mMainToolBar->addWidget(mShowDetails);
    //connect(mShowDetails, SIGNAL(toggled(bool)), SLOT(slotShowDetails(bool)));

    mMainToolBar->addAction(printAction);
}

void MainWindow::setupActions()
{
    const QIcon reloadIcon =
        (style() != 0 ? style()->standardIcon(QStyle::SP_BrowserReload, 0, 0)
         : QIcon());
    if (!reloadIcon.isNull()) {
        mUi.actionSynchronize->setIcon(reloadIcon);
    }

    connect(mUi.actionCRMAccounts, SIGNAL(triggered()), SLOT(slotConfigureResources()));
    connect(mUi.actionImportContacts, SIGNAL(triggered()), this, SLOT(slotImportContacts()));
    connect(mUi.actionOfflineMode, SIGNAL(toggled(bool)), this, SLOT(slotToggleOffline(bool)));
    connect(mUi.actionSynchronize, SIGNAL(triggered()), this, SLOT(slotSynchronize()));
    connect(mUi.actionFullReload, SIGNAL(triggered()), this, SLOT(slotFullReload()));
    connect(mUi.actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    connect(mUi.actionAboutFatCRM, SIGNAL(triggered()), this, SLOT(slotAboutApp()));

    // NOT FINISHED YET
    mUi.actionImportContacts->setVisible(false);

    connect(mUi.actionConfigureFatCRM, SIGNAL(triggered()), this, SLOT(slotConfigure()));

    Q_FOREACH (const Page *page, mPages) {
        connect(page, SIGNAL(statusMessage(QString)),
                this, SLOT(slotShowMessage(QString)));
        connect(page, SIGNAL(modelLoaded(DetailsType)),
                this, SLOT(slotModelLoaded(DetailsType)));
        connect(page, SIGNAL(showDetailsChanged(bool)),
                this, SLOT(slotPageShowDetailsChanged()));
        connect(page, SIGNAL(synchronizeCollection(Akonadi::Collection)),
                this, SLOT(slotSynchronizeCollection(Akonadi::Collection)));
        connect(page, SIGNAL(openObject(DetailsType,QString)),
                this, SLOT(slotOpenObject(DetailsType,QString)));
    }
}

void MainWindow::slotResourceSelectionChanged(int index)
{
    AgentInstance agent = mResourceSelector->itemData(index, AgentInstanceModel::InstanceRole).value<AgentInstance>();
    if (agent.isValid()) {
        const QByteArray identifier = agent.identifier().toLatin1();
        emit resourceSelected(identifier);
        updateWindowTitle(agent.isOnline());
        mUi.actionSynchronize->setEnabled(true);
        mUi.actionFullReload->setEnabled(true);
        mUi.actionOfflineMode->setEnabled(true);
        mUi.actionOfflineMode->setChecked(!agent.isOnline());
        mResourceDialog->resourceSelectionChanged(agent);
        slotResourceProgress(agent);
        ReferencedData::clearAll();
        AccountRepository::instance()->clear();
        mNotesRepository->clear();
        mCollectionManager->setResource(identifier);
        slotShowMessage(i18n("(0/5) Listing folders..."));
    } else {
        mUi.actionSynchronize->setEnabled(false);
        mUi.actionFullReload->setEnabled(false);
        mUi.actionOfflineMode->setEnabled(false);
    }
}

void MainWindow::slotResourceSelected(const Akonadi::AgentInstance &resource)
{
    for (int index = 0; index < mResourceSelector->count(); ++index) {
        const AgentInstance agent = mResourceSelector->itemData(index, AgentInstanceModel::InstanceRole).value<AgentInstance>();
        if (agent.isValid() && agent == resource) {
            mResourceSelector->setCurrentIndex(index);
            return;
        }
    }
}

void MainWindow::slotServerStarted()
{
    qDebug() << "Akonadi server started. Resource selector has" << mResourceSelector->count() << "items";
    if (mResourceSelector->count() > 0) {
        initialResourceSelection();
    }
}

void MainWindow::slotResourceCountChanged()
{
    // an empty combo or a combo with one item just looks stupid
    mResourceSelectorAction->setVisible(mResourceSelector->count() > 1);
}

void MainWindow::slotToggleOffline(bool offline)
{
    AgentInstance currentAgent = currentResource();
    const bool online = !offline;
    if (currentAgent.isValid() && currentAgent.isOnline() != online) {
        qDebug() << "setting agent" << currentAgent.identifier() << "to online=" << online;
        currentAgent.setIsOnline(online);
    }
}

void MainWindow::slotSynchronize()
{
    AgentInstance currentAgent = currentResource();
    if (currentAgent.isValid()) {
        if (!currentAgent.isOnline())
            currentAgent.setIsOnline(true);
        currentAgent.synchronize();
    }
}

void MainWindow::slotSynchronizeCollection(const Collection &collection)
{
    AgentInstance currentAgent = currentResource();
    if (currentAgent.isValid()) {
        if (!currentAgent.isOnline())
            currentAgent.setIsOnline(true);
        AgentManager::self()->synchronizeCollection(collection);
    }
}

void MainWindow::slotFullReload()
{
    // Kick off one collection-modify job per collection, to clear the timestamp attribute
    // Once all these jobs are done, we'll trigger a resource synchronization
    mClearTimestampJobs.clear();
    Q_FOREACH (Page *page, mPages) {
        KJob *modJob = page->clearTimestamp();
        connect(modJob, SIGNAL(result(KJob*)), this, SLOT(slotClearTimestampResult(KJob*)));
        mClearTimestampJobs.append(modJob);
    }
}

void MainWindow::slotShowMessage(const QString &message)
{
    qDebug() << message;
    statusBar()->showMessage(message);
}

void MainWindow::slotModelLoaded(DetailsType type)
{
    // We load Opps, Accounts, Contacts, Notes and Emails in this order (see CollectionManager)
    //qDebug() << typeToString(type) << "loaded";
    switch (type)
    {
    case Account:
        slotShowMessage(i18n("(2/5) Loading opportunities..."));
        break;
    case Opportunity:
        slotShowMessage(i18n("(3/5) Loading contacts..."));
        break;
    case Contact:
        slotShowMessage(i18n("(4/5) Loading notes..."));
        ReferencedData::emitInitialLoadingDoneForAll(); // fill combos
        Q_FOREACH (Page *page, mPages) {
            page->initialLoadingDone(); // select correct item in newly filled combos
        }
        mNotesRepository->loadNotes();
        break;
    case Lead:
    case Campaign:
        // currently unused
        break;
    }
}

void MainWindow::slotNotesLoaded(int count)
{
    Q_UNUSED(count);
    slotShowMessage(i18n("(5/5) Loading emails..."));
    mNotesRepository->loadEmails();
}

void MainWindow::slotEmailsLoaded(int count)
{
    Q_UNUSED(count);
    mNotesRepository->monitorChanges();
    slotShowMessage(i18n("Ready"));
}

void MainWindow::createTabs()
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

    connect(page, SIGNAL(modelCreated(ItemsTreeModel*)), this, SLOT(slotOppModelCreated(ItemsTreeModel*)));

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

    mReportPage = new ReportPage(this);
    mUi.tabWidget->addTab(mReportPage, tr("&Reports"));

    //set Opportunities page as current
    mShowDetails->setChecked(mPages[ 1 ]->showsDetails());
    mUi.tabWidget->setCurrentIndex(1);
}

void MainWindow::slotConfigureResources()
{
    mResourceDialog->show();
    mResourceDialog->raise();
}

void MainWindow::setupResourcesCombo()
{
    // monitor Akonadi agents so we can check for KDCRM specific resources
    AgentInstanceModel *agentModel = new AgentInstanceModel(this);
    AgentFilterProxyModel *agentFilterModel = new AgentFilterProxyModel(this);
    agentFilterModel->addCapabilityFilter(QString("KDCRM").toLatin1());
    agentFilterModel->setSourceModel(agentModel);

    // Remove this and use agentFilterModel on the last line when everyone has kdepimlibs >= 4.14.7
#if 1
    WorkaroundFilterProxyModel *workaround = new WorkaroundFilterProxyModel(this);
    workaround->setSourceModel(agentFilterModel);
    mResourceSelector->setModel(workaround);
#endif

    connect(mResourceSelector, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotResourceSelectionChanged(int)));
    connect(mResourceSelector->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(slotResourceCountChanged()));
    connect(mResourceSelector->model(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(slotResourceCountChanged()));
    slotResourceCountChanged();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
}

void MainWindow::slotResourceError(const AgentInstance &resource, const QString &message)
{
    const AgentInstance currentAgent = currentResource();
    if (currentAgent.isValid() && currentAgent.identifier() == resource.identifier()) {
        slotShowMessage(message);
    }
}

void MainWindow::updateWindowTitle(bool online)
{
    if (mResourceSelector->count() == 1) {
        setWindowTitle(online ? i18n("FatCRM") : i18n("FatCRM (offline)"));
    } else {
        const int index = mResourceSelector->currentIndex();
        const QString context = mResourceSelector->itemText(index);
        const QString contextTitle =
                online ? i18n("FatCRM: %1", context)
                       : i18n("FatCRM: %1 (offline)", context);
        setWindowTitle(contextTitle);
    }
}

void MainWindow::slotResourceOnline(const AgentInstance &resource, bool online)
{
    const AgentInstance currentAgent = currentResource();
    if (currentAgent.isValid() && currentAgent.identifier() == resource.identifier()) {
        updateWindowTitle(online);
        mUi.actionOfflineMode->setChecked(!online);
        if (online) {
            Q_FOREACH (Page *page, mPages) {
                page->retrieveResourceUrl();
            }
        }
    }
}

void MainWindow::slotResourceProgress(const AgentInstance &resource)
{
    const AgentInstance currentAgent = currentResource();
    if (currentAgent.isValid() && currentAgent.identifier() == resource.identifier()) {
        const int progress = resource.progress();
        const QString message = resource.statusMessage();
        AgentInstance::Status status = resource.status();

        if (status == AgentInstance::Broken
                || status == AgentInstance::NotConfigured
                || !resource.isOnline()) {
            mProgressBar->hide();
        } else if (status == AgentInstance::Idle) {
            statusBar()->clearMessage();
            mProgressBarHideTimer->start();
        } else {
            qDebug() << progress << message;
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

void MainWindow::slotShowDetails(bool on)
{
    mPages[ mUi.tabWidget->currentIndex() ]->showDetails(on);
}

void MainWindow::slotPageShowDetailsChanged()
{
    Page *page = currentPage();
    if (page) {
        mShowDetails->setChecked(page->showsDetails());
    }
}

void MainWindow::slotCurrentTabChanged(int index)
{
    if (index < mPages.count()) {
        mShowDetails->setChecked(mPages[ index ]->showsDetails());
    }
}

void MainWindow::slotImportContacts()
{
    const QString csvFile = QFileDialog::getOpenFileName(this, tr("Select contacts file"), QString(), "*.csv");
    if (!csvFile.isEmpty()) {
        ContactsImporter importer;
        if (importer.importFile(csvFile)) {
            const QVector<SugarAccount> accounts = importer.accounts();
            AccountImportDialog importDialog(this);
            importDialog.setImportedAccounts(accounts);
            importDialog.exec();
        } else {
            QMessageBox::warning(this, i18nc("@title:window", "Failed to import CSV file"),
                                 i18n("Error importing %1", csvFile));
        }
    }
}

void MainWindow::slotConfigure()
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

void MainWindow::slotPrintReport()
{
    Page *page = currentPage();
    if (page)
        page->printReport();
}

void MainWindow::slotCollectionResult(const QString &mimeType, const Collection &collection)
{
    if (mimeType == "application/x-vnd.kdab.crm.account") {
        slotShowMessage(i18n("(1/5) Loading accounts..."));
    }
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

void MainWindow::slotIgnoreModifications(bool ignore)
{
    foreach(Page *page, mPages) {
        page->setModificationsIgnored(ignore);
    }
}

void MainWindow::slotOppModelCreated(ItemsTreeModel *model)
{
    mReportPage->setOppModel(model);
}

void MainWindow::slotOpenObject(DetailsType type, const QString &id)
{
    Page *page = pageForType(type);
    if (page) {
        page->openDialog(id);
    }
}

void MainWindow::slotClearTimestampResult(KJob *job)
{
    mClearTimestampJobs.removeAll(job);
    if (mClearTimestampJobs.isEmpty()) {
        slotSynchronize();
    }
}

Page *MainWindow::currentPage() const
{
    const int index = mUi.tabWidget->currentIndex();
    if (index >= 0 && index <= mPages.count())
        return mPages[ index ];
    return 0;
}

AgentInstance MainWindow::currentResource() const
{
    const int index = mResourceSelector->currentIndex();
    return mResourceSelector->itemData(index, AgentInstanceModel::InstanceRole).value<AgentInstance>();
}

void MainWindow::initialResourceSelection()
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

Page *MainWindow::pageForType(DetailsType type) const
{
    foreach(Page *page, mPages) {
        if (page->detailsType() == type) {
            return page;
        }
    }
    return 0;
}

