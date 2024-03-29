/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "ui_mainwindow.h"

#include "contactspage.h"
#include "accountspage.h"
#include "opportunitiespage.h"

#include "aboutdialog.h"
#include "accountrepository.h"
#include "clientsettings.h"
#include "collectionmanager.h"
#include "configurationdialog.h"
#include "contactsimporter.h"
#include "contactsimportwizard.h"
#include "dbusinvokerinterface.h"
#include "dbuswinidprovider.h"
#include "enums.h"
#include "loadingoverlay.h"
#include "config-fatcrm-version.h"
#include "linkeditemsrepository.h"
#include "referenceddata.h"
#include "reportpage.h"
#include "resourceconfigdialog.h"
#include "fatcrm_client_debug.h"
#include "searchesdialog.h"
#include "itemstreemodel.h"

#include "kdcrmdata/enumdefinitionattribute.h"

#include <KDReportsReport.h>
#include <KDReportsPreviewDialog.h>
#include <KDReportsPreviewWidget.h>

#include <Akonadi/AgentFilterProxyModel>
#include <Akonadi/AgentInstance>
#include <Akonadi/AgentInstanceModel>
#include <Akonadi/AgentManager>
#include <Akonadi/AttributeFactory>
#include <Akonadi/ServerManager>
#include <Akonadi/ControlGui>
using namespace Akonadi;

#include <KAboutData>
#include <KLocalizedString>

#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QLabel>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QTimer>
#include <QToolBar>

MainWindow::MainWindow(bool displayOverlay, bool showGDPR)
    : mUi(new Ui_MainWindow),
      mProgressBar(nullptr),
      mProgressBarHideTimer(nullptr),
      mCollectionManager(new CollectionManager(this)),
      mLinkedItemsRepository(new LinkedItemsRepository(mCollectionManager, this)),
      mContactsModel(nullptr),
      mInitialLoadingDone(false),
      mDisplayOverlay(displayOverlay)
{
    mUi->setupUi(this);
    initialize(displayOverlay, showGDPR);

    /*
     * this creates an overlay in case Akonadi is not running,
     * allowing the user to restart it
     */
    Akonadi::ControlGui::widgetNeedsAkonadi(this);
    QMetaObject::invokeMethod(this, "slotDelayedInit", Qt::AutoConnection);

    (void)new DBusWinIdProvider(this);

    auto *iface = new DBusInvokerInterface(this);
    connect(iface, &DBusInvokerInterface::importCsvFileRequested, this, &MainWindow::slotTryImportCsvFile);

    ClientSettings::self()->restoreWindowSize(QStringLiteral("main"), this);

    qApp->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    ClientSettings::self()->saveWindowSize(QStringLiteral("main"), this);
    delete mResourceDialog;

    delete mUi;
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    // Intercept mouse wheel events on closed combo boxes, because
    // it causes accidentally data changes in the UI.
    if (event->type() == QEvent::Wheel) {
        auto *cb = qobject_cast<QComboBox*>(object);
        if (cb)
            return true;
    }

    return QMainWindow::eventFilter(object, event);
}

void MainWindow::slotDelayedInit()
{
    Q_FOREACH (const Page *page, mPages) {
        // Connect from this to pages - this is really just a way to avoid writing a loop at emit time...
        connect(this, &MainWindow::resourceSelected,
                page, &Page::slotResourceSelectionChanged);
        connect(this, &MainWindow::onlineStatusChanged,
                page, &Page::slotOnlineStatusChanged);
    }

    // initialize additional UI

    setupResourcesCombo();

    // no parent so it can have its own Akonadi-not-started overlay (bug in Akonadi::ErrorOverlay, fixed in f5f76cc, kdepimlibs >= 4.14.10)
    mResourceDialog = new ResourceConfigDialog;

    connect(mResourceDialog, &ResourceConfigDialog::resourceSelected,
            this, &MainWindow::slotResourceSelected);

    connect(mCollectionManager, &CollectionManager::collectionResult,
            this, &MainWindow::slotCollectionResult);

    initialResourceSelection();

    connect(Akonadi::ServerManager::self(), &ServerManager::started,
            this, &MainWindow::slotServerStarted);

    connect(AgentManager::self(), &AgentManager::instanceError,
            this, &MainWindow::slotResourceError);
    connect(AgentManager::self(), &AgentManager::instanceOnline,
            this, &MainWindow::slotResourceOnline);
    connect(AgentManager::self(), &AgentManager::instanceProgressChanged,
            this, &MainWindow::slotResourceProgress);
    connect(AgentManager::self(), &AgentManager::instanceStatusChanged,
            this, &MainWindow::slotResourceProgress);

    Akonadi::AttributeFactory::registerAttribute<EnumDefinitionAttribute>();
}

void MainWindow::slotAboutKDAB()
{
    AboutDialog dialog(this);
    dialog.setWindowTitle(tr("About KDAB"));
    dialog.setTitle(tr("Klarälvdalens Datakonsult AB (KDAB)"));
    dialog.setText(tr("<qt><p>This application is supported and maintained by KDAB</p>"
                      "<p>The KDAB Group is the global No.1 software consultancy for Qt, C++ and "
                      "OpenGL applications across desktop, embedded and mobile platforms. "
                      "<p>"
                      "The KDAB Group provides consulting and mentoring for developing Qt applications " "from scratch and in porting from all popular and legacy frameworks to Qt. "
                      "We continue to help develop parts of Qt and are one of the major contributors "
                      "the Qt Project. We can give advanced or standard trainings anywhere "
                      "around the globe on Qt as well as C++, OpenGL, 3D and more."
                      "<p>"
                      "Please visit <a href='https://www.kdab.com'>https://www.kdab.com</a> "
                      "to meet the people who write code like this."
                      "</p></qt>"));
    dialog.setLogo(QStringLiteral(":/images/kdablogo.png"));
    dialog.setWindowIcon(QPixmap(QStringLiteral(":/images/kdablogo.png")));
    dialog.adjustSize();
    dialog.exec();
}

void MainWindow::slotAboutApp()
{
    const auto aboutData = KAboutData::applicationData();

    AboutDialog dialog(this);
    dialog.setWindowTitle(tr("About FatCRM"));
    dialog.setTitle(tr("FatCRM - %1").arg(aboutData.shortDescription()));
    dialog.setText(tr("<qt><p>FatCRM is supported and maintained by KDAB</p>"
                      "<p>(C) 2010-2021 Klarälvdalens Datakonsult AB (KDAB)</p>"
                      "<p>Version %1</p>"
                      "<p>This project is a KDAB R&D effort to create a standalone GUI for SugarCRM.</p>"
                      "<p>FatCRM is an open source project:</p>"
                      "<ul>"
                      "<li><a href=\"https://github.com/KDAB/FatCRM\">GitHub project page</a></li>"
                      "<li><a href=\"https://github.com/KDAB/FatCRM/issues\">Issue Tracker</a></li>"
                      "<li><a href=\"https://github.com/KDAB/FatCRM/graphs/contributors\">Contributors</a></li>"
                      "</ul><p>Patches welcome!</p></qt>").arg(FATCRM_EXTENDED_VERSION));
    dialog.setLogo(QStringLiteral(":/images/fatcrmlogo.png"));
    dialog.setWindowIcon(QIcon::fromTheme(QStringLiteral("fatcrm")));
    dialog.adjustSize();
    dialog.exec();
}


void MainWindow::initialize(bool displayOverlay, bool showGDPR)
{
    Q_INIT_RESOURCE(resources);

    mLoadingOverlay = new LoadingOverlay(mUi->tabWidget);
    if (!displayOverlay)
        slotHideOverlay();

    resize(900, 900);
    createActions();
    createTabs(showGDPR);
    setupActions();
    // initialize view actions
    mUi->actionSynchronize->setEnabled(false);
    mUi->actionFullReload->setEnabled(false);
    mUi->actionOfflineMode->setEnabled(false);

    // Displays status of FatCRM initial loading,
    // and then displays status of the resource.
    // This is permanent (rather than using showMessage) because the statustip
    // feature, when e.g. leaving a toolbutton, clears any temporary message.
    // If the resource shows a severe error, we want to see it for sure.
    mStatusLabel = new QLabel(this);
    statusBar()->addWidget(mStatusLabel);

    mProgressBar = new QProgressBar(this);
    mProgressBar->setRange(0, 100);
    mProgressBar->setMaximumWidth(100);
    statusBar()->addPermanentWidget(mProgressBar);
    mProgressBar->hide();

    mProgressBarHideTimer = new QTimer(this);
    mProgressBarHideTimer->setInterval(1000);
    connect(mProgressBarHideTimer, &QTimer::timeout, mProgressBar, &QProgressBar::hide);

    mSyncWarningTimer = new QTimer(this);
    mSyncWarningTimer->setInterval(60 * 60 * 1000); // 1h
    connect(mSyncWarningTimer, &QTimer::timeout, this, [this]() { slotShowMessage(i18n("Warning: 1 hour without synchronization. Check internet connection, then try killing akonadi_sugarcrm_resource.")); });

    connect(mLinkedItemsRepository, SIGNAL(notesLoaded(int)),
            this, SLOT(slotNotesLoaded(int)));
    connect(mLinkedItemsRepository, SIGNAL(emailsLoaded(int)),
            this, SLOT(slotEmailsLoaded(int)));
    connect(mLinkedItemsRepository, SIGNAL(documentsLoaded(int)),
            this, SLOT(slotDocumentsLoaded(int)));
}

void MainWindow::createActions()
{
    // the other menus are handled in Qt Designer

    mViewMenu = new QMenu(i18n("&View"), this);
    menuBar()->insertMenu(mUi->menuSettings->menuAction(), mViewMenu);
    QAction *printAction = new QAction(i18n("Print Report..."), this);
    printAction->setShortcut(QKeySequence::Print);
    printAction->setIcon(QIcon(":/icons/document-print-preview.png"));
    connect(printAction, &QAction::triggered, this, &MainWindow::slotPrintReport);
    mViewMenu->addAction(printAction);

    QAction *exportAction = new QAction(i18n("Export as CSV..."), this);
    connect(exportAction, &QAction::triggered, this, &MainWindow::slotExport);
    mViewMenu->addAction(exportAction);

    mViewMenu->addSeparator();

    mSavedSearchesMenu = new QMenu(i18n("Saved Searches"), mViewMenu);
    mViewMenu->addMenu(mSavedSearchesMenu);

    populateSavedSearchesMenu();
    connect(ClientSettings::self(), &ClientSettings::recentSearchesUpdated, this, &MainWindow::populateSavedSearchesMenu);
    connect(mSavedSearchesMenu, &QMenu::triggered, this, &MainWindow::slotLoadSearchFromRecent);

    mMainToolBar = addToolBar(i18n("Main ToolBar"));
    mResourceSelector = new QComboBox(this);
    mResourceSelectorAction = mMainToolBar->addWidget(mResourceSelector);
    mMainToolBar->addAction(mUi->actionSynchronize);

    mMainToolBar->addAction(printAction);
}

void MainWindow::setupActions()
{
    const QIcon reloadIcon =
        (style() != nullptr ? style()->standardIcon(QStyle::SP_BrowserReload, nullptr, nullptr)
         : QIcon());
    if (!reloadIcon.isNull()) {
        mUi->actionSynchronize->setIcon(reloadIcon);
    }

    connect(mUi->actionCRMAccounts, &QAction::triggered, this, &MainWindow::slotConfigureResources);
    connect(mUi->actionImportContacts, &QAction::triggered, this, &MainWindow::slotImportContacts);
    connect(mUi->actionOfflineMode, &QAction::toggled, this, &MainWindow::slotToggleOffline);
    connect(mUi->actionSynchronize, &QAction::triggered, this, &MainWindow::slotSynchronize);
    connect(mUi->actionFullReload, &QAction::triggered, this, &MainWindow::slotFullReload);
    connect(mUi->actionQuit, &QAction::triggered, this, &QWidget::close);

    connect(mUi->actionAboutFatCRM, &QAction::triggered, this, &MainWindow::slotAboutApp);
    connect(mUi->actionAboutKDAB, &QAction::triggered, this, &MainWindow::slotAboutKDAB);

    connect(mUi->actionConfigureFatCRM, &QAction::triggered, this, &MainWindow::slotConfigure);

    auto *activateNextTabAction = new QAction(tr("Activate Next Tab"), this);
    activateNextTabAction->setShortcut(Qt::CTRL + Qt::Key_PageDown);
    connect(activateNextTabAction, &QAction::triggered, this, &MainWindow::slotActivateNextTab);
    addAction(activateNextTabAction);

    auto *activatePreviousTabAction = new QAction(tr("Activate Previous Tab"), this);
    activatePreviousTabAction->setShortcut(Qt::CTRL + Qt::Key_PageUp);
    connect(activatePreviousTabAction, &QAction::triggered, this, &MainWindow::slotActivatePreviousTab);
    addAction(activatePreviousTabAction);

    Q_FOREACH (const Page *page, mPages) {
        connect(page, &Page::statusMessage,
                this, &MainWindow::slotShowMessage);
        connect(page, &Page::modelLoaded,
                this, &MainWindow::slotModelLoaded);
        connect(page, &Page::synchronizeCollection,
                this, &MainWindow::slotSynchronizeCollection);
        connect(page, &Page::syncRequired, this, &MainWindow::slotSynchronize);
        connect(page, &Page::openObject,
                this, &MainWindow::slotOpenObject);
    }
}

void MainWindow::slotResourceSelectionChanged(int index)
{
    if (mDisplayOverlay) {
        mLoadingOverlay->show();
        mLoadingOverlay->setMessage(i18n("Loading..."));
    }
    AgentInstance agent = mResourceSelector->itemData(index, AgentInstanceModel::InstanceRole).value<AgentInstance>();
    if (agent.isValid()) {
        const QByteArray identifier = agent.identifier().toLatin1();
        ClientSettings::self()->setDefaultResourceId(identifier);
        emit resourceSelected(identifier);
        slotResourceOnline(agent, agent.isOnline());
        mUi->actionSynchronize->setEnabled(true);
        mUi->actionFullReload->setEnabled(true);
        mUi->actionOfflineMode->setEnabled(true);
        mResourceDialog->resourceSelectionChanged(agent);
        slotResourceProgress(agent);
        ReferencedData::clearAll();
        AccountRepository::instance()->clear();
        mLinkedItemsRepository->clear();
        mCollectionManager->setResource(identifier);
        slotShowMessage(i18n("(0/6) Listing folders..."));
    } else {
        mUi->actionSynchronize->setEnabled(false);
        mUi->actionFullReload->setEnabled(false);
        mUi->actionOfflineMode->setEnabled(false);
    }
}

void MainWindow::slotResourceSelected(const Akonadi::AgentInstance &resource)
{
    for (int index = 0; index < mResourceSelector->count(); ++index) {
        const AgentInstance agent = mResourceSelector->itemData(index, AgentInstanceModel::InstanceRole).value<AgentInstance>();
        if (agent.isValid() && agent == resource) {
            mResourceSelector->setCurrentIndex(index);
            slotResourceSelectionChanged(index);
            return;
        }
    }
}

void MainWindow::slotServerStarted()
{
    qCDebug(FATCRM_CLIENT_LOG) << "Akonadi server started. Resource selector has" << mResourceSelector->count() << "items";
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
        qCDebug(FATCRM_CLIENT_LOG) << "setting agent" << currentAgent.identifier() << "to online=" << online;
        currentAgent.setIsOnline(online);
    }
}

void MainWindow::slotSynchronize()
{
    AgentInstance currentAgent = currentResource();
    if (currentAgent.isValid()) {
        if (!currentAgent.isOnline())
            currentAgent.setIsOnline(true);
        slotResourceProgress(currentAgent); // show initial status, very important if it's e.g. Broken
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
    mClearTimestampJobs = mCollectionManager->clearTimestamps();
    Q_FOREACH (KJob *modJob, mClearTimestampJobs) {
        connect(modJob, &KJob::result, this, &MainWindow::slotClearTimestampResult);
    }
}

void MainWindow::slotShowMessage(const QString &message)
{
    qCDebug(FATCRM_CLIENT_LOG) << message;
    mStatusLabel->setText(message);
}

void MainWindow::slotModelLoaded(DetailsType type)
{
    // We load Opps, Accounts, Contacts, Notes and Emails in this order (see CollectionManager)
    //qCDebug(FATCRM_CLIENT_LOG) << typeToString(type) << "loaded";
    switch (type)
    {
    case DetailsType::Account:
        slotShowMessage(i18n("(2/6) Loading opportunities..."));
        break;
    case DetailsType::Opportunity:
        slotShowMessage(i18n("(3/6) Loading contacts..."));
        break;
    case DetailsType::Contact:
        slotShowMessage(i18n("(4/6) Loading notes..."));
        ReferencedData::emitInitialLoadingDoneForAll(); // fill combos
        slotHideOverlay();
        mLinkedItemsRepository->loadNotes();
        break;
    case DetailsType::Lead:
    case DetailsType::Campaign:
        // currently unused
        break;
    }
}

void MainWindow::slotNotesLoaded(int count)
{
    Q_UNUSED(count);
    slotShowMessage(i18n("(5/6) Loading emails..."));
    mLinkedItemsRepository->loadEmails();
}

void MainWindow::slotEmailsLoaded(int count)
{
    Q_UNUSED(count);
    slotShowMessage(i18n("(6/6) Loading documents..."));
    mLinkedItemsRepository->loadDocuments();
}

void MainWindow::slotDocumentsLoaded(int count)
{
    Q_UNUSED(count);
    slotInitialLoadingDone();
}

void MainWindow::slotInitialLoadingDone()
{
    mInitialLoadingDone = true;

    mLinkedItemsRepository->monitorChanges();
    slotShowMessage(i18n("Ready"));

    // Now is a good time to synchronize data from the server,
    // so that users don't start working on outdated data for the next 30 minutes.
    slotSynchronize();

    processPendingImports();

    emit initialLoadingDone();
}

void MainWindow::addPage(Page *page)
{
    page->setCollectionManager(mCollectionManager);
    page->setLinkedItemsRepository(mLinkedItemsRepository);
    mPages << page;
}

void MainWindow::createTabs(bool showGDPR)
{
    mAccountPage = new AccountsPage(this);
    addPage(mAccountPage);
    mUi->tabWidget->addTab(mAccountPage, i18n("&Accounts"));

    mOpportunitiesPage = new OpportunitiesPage(this);
    addPage(mOpportunitiesPage);
    mUi->tabWidget->addTab(mOpportunitiesPage, i18n("&Opportunities"));

    connect(mAccountPage, &AccountsPage::requestNewOpportunity, mOpportunitiesPage, &OpportunitiesPage::createOpportunity);
    connect(mOpportunitiesPage, &Page::modelCreated, this, &MainWindow::slotOppModelCreated);

#if 0
    Page *page = new LeadsPage(this);
    addPage(page);
    mUi->tabWidget->addTab(page, i18n("&Leads"));
#endif

    mContactsPage = new ContactsPage(showGDPR, this);
    addPage(mContactsPage);
    mUi->tabWidget->addTab(mContactsPage, i18n("&Contacts"));

    connect(mContactsPage, &Page::modelCreated, this, &MainWindow::slotContactsModelCreated);

#if 0
    page = new CampaignsPage(this);
    addPage(page);
    mUi->tabWidget->addTab(page, i18n("&Campaigns"));
#endif

    mReportPage = new ReportPage(this);
    mUi->tabWidget->addTab(mReportPage, i18n("&Reports"));

    //set Opportunities page as current
    mUi->tabWidget->setCurrentIndex(1);
}

void MainWindow::slotConfigureResources()
{
    mResourceDialog->show();
    mResourceDialog->raise();
}

void MainWindow::setupResourcesCombo()
{
    // monitor Akonadi agents so we can check for KDCRM specific resources
    auto *agentModel = new AgentInstanceModel(this);
    auto *agentFilterModel = new AgentFilterProxyModel(this);
    agentFilterModel->addCapabilityFilter(QStringLiteral("KDCRM").toLatin1());
    agentFilterModel->setSourceModel(agentModel);

    mResourceSelector->setModel(agentFilterModel);

    connect(mResourceSelector, SIGNAL(activated(int)),
            this, SLOT(slotResourceSelectionChanged(int)));
    connect(mResourceSelector->model(), &QAbstractItemModel::rowsInserted,
            this, &MainWindow::slotResourceCountChanged);
    connect(mResourceSelector->model(), &QAbstractItemModel::rowsRemoved,
            this, &MainWindow::slotResourceCountChanged);
    slotResourceCountChanged();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();

    Q_FOREACH (Page *page, mPages) {
        if(!page->queryClose()) {
            return;
        }
    }

    event->accept();
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
        mUi->actionOfflineMode->setChecked(!online);
        mUi->actionImportContacts->setEnabled(online);
        emit onlineStatusChanged(online); // update details dialog
        if (online)
            mSyncWarningTimer->start();
        else
            mSyncWarningTimer->stop();
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
            mSyncWarningTimer->stop();
            mProgressBar->hide();
            if (!mUi->actionOfflineMode->isChecked()) {
                slotShowMessage(message);
                return;
            }
        } else if (status == AgentInstance::Idle) {
            statusBar()->clearMessage();
            mProgressBarHideTimer->start();
        } else {
            qCDebug(FATCRM_CLIENT_LOG) << "Resource:" << message << "- progress:" << progress;
            mProgressBar->show();
            mProgressBar->setValue(progress);
            if (progress == 100) {
                mProgressBarHideTimer->start();
            } else {
                mProgressBarHideTimer->stop();
            }
            mSyncWarningTimer->start();
        }
        if (!message.isEmpty()) {
            slotShowMessage(message);
        }
    }
}

void MainWindow::slotImportContacts()
{
    const QString csvFile = QFileDialog::getOpenFileName(this, i18n("Select contacts file"), QString(), QStringLiteral("*.csv"));
    if (!csvFile.isEmpty()) {
        slotImportCsvFile(csvFile);
    }
}

void MainWindow::slotConfigure()
{
    ConfigurationDialog dlg;
    dlg.exec();
}

void MainWindow::slotPrintReport()
{
    Page *page = currentPage();
    if (!page) {
        return;
    }

    auto report = page->generateReport();
    if (!report)
        return;

    KDReports::PreviewDialog preview(report.get(), this);
    preview.setWindowTitle(i18n("Print Preview"));
    preview.previewWidget()->setShowPageListWidget(false);
    preview.resize(1167, 906);
    preview.exec();
}

void MainWindow::slotExport()
{
    Page *page = currentPage();
    if (!page) {
        return;
    }
    const QString fileName = QFileDialog::getSaveFileName(this, i18n("Export as CSV"), QString(), "*.csv");
    if (!fileName.isEmpty()) {
        page->exportToCSV(fileName);
    }
}

void MainWindow::slotCollectionResult(const QString &mimeType, const Collection &collection)
{
    if (mimeType == SugarAccount::mimeType()) {
        slotShowMessage(i18n("(1/6) Loading accounts..."));
    }
    foreach(Page *page, mPages) {
        if (page->mimeType() == mimeType) {
            page->setCollection(collection);
            return;
        }
    }
    if (mimeType == SugarNote::mimeType()) {
        mLinkedItemsRepository->setNotesCollection(collection);
    } else if (mimeType == SugarEmail::mimeType()) {
        mLinkedItemsRepository->setEmailsCollection(collection);
    } else if (mimeType == SugarDocument::mimeType()) {
        mLinkedItemsRepository->setDocumentsCollection(collection);
    }
}

void MainWindow::slotOppModelCreated(ItemsTreeModel *model)
{
    mReportPage->setOppModel(model);
}

void MainWindow::slotContactsModelCreated(ItemsTreeModel *model)
{
    mContactsModel = model;
}

void MainWindow::slotOpenObject(DetailsType type, const QString &id)
{
    Page *page = pageForType(type);
    if (page) {
        page->openWidget(id);
    } else {
        qCDebug(FATCRM_CLIENT_LOG) << "No page for type" << int(type);
    }
}

void MainWindow::slotClearTimestampResult(KJob *job)
{
    mClearTimestampJobs.removeAll(job);
    if (mClearTimestampJobs.isEmpty()) {
        slotSynchronize();
    }
}

void MainWindow::slotTryImportCsvFile(const QString &filePath)
{
    if (mInitialLoadingDone)
        slotImportCsvFile(filePath);
    else
        mPendingImportPaths.append(filePath);
}

void MainWindow::slotImportCsvFile(const QString &filePath)
{
    ContactsImporter importer;
    if (importer.importFile(filePath)) {
        const QVector<ContactsSet> contacts = importer.contacts();
        // non modal so that we can use FatCRM to search for accounts/contacts.
        // no parent widget so it can be minimized separately, shows up separate in the taskbar, etc.
        auto *importWizard = new ContactsImportWizard(nullptr);
        importWizard->setAccountCollection(mAccountPage->collection());
        importWizard->setContactsCollection(mContactsPage->collection());
        importWizard->setImportedContacts(contacts);
        importWizard->setContactsModel(mContactsModel);
        connect(importWizard, &ContactsImportWizard::openFutureContact,
                mContactsPage, &ContactsPage::slotOpenFutureContact);
        importWizard->show();
        raiseMainWindowAndDialog(importWizard);
    } else {
        QMessageBox::warning(this, i18nc("@title:window", "Failed to import CSV file"),
                             i18n("Error importing %1", filePath));
    }
}

void MainWindow::slotOpenSearchesDialog()
{
    SearchesDialog dlg;
    dlg.setWindowTitle(i18n("Load Saved Search"));
    if (dlg.exec() == QDialog::Rejected) {
        return;
    }

    loadSavedSearch(dlg.selectedItemName());
    ClientSettings::self()->addRecentlyUsedSearch(dlg.selectedItemName());
}

Page *MainWindow::currentPage() const
{
    return mPages.value(mUi->tabWidget->currentIndex());
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
        mResourceDialog->hide();
    } else if (selectors == 0) {
        mLoadingOverlay->setMessage(i18n("Configure a SugarCRM resource in order to use FatCRM."));
        showResourceDialog();
    } else {
        const int resourceIndex = resourceIndexFor(ClientSettings::self()->defaultResourceId());
        if (resourceIndex != -1) {
            mResourceSelector->setCurrentIndex(resourceIndex);
            slotResourceSelectionChanged(resourceIndex);
            mResourceDialog->hide();
            return;
        }

        mResourceSelector->setCurrentIndex(-1);
        mLoadingOverlay->setMessage(i18n("Choose a SugarCRM resource."));
        showResourceDialog();
    }
}

void MainWindow::showResourceDialog()
{
    mResourceDialog->show();
    // delay mResourceDialog->raise() so it happens after MainWindow::show() (from main.cpp)
    // This is part of the "mResourceDialog has no parent" workaround
    QMetaObject::invokeMethod(mResourceDialog, "raise", Qt::QueuedConnection);
}

int MainWindow::resourceIndexFor(const QString &id) const
{
    if (id.isEmpty())
        return -1;

    for (int index = 0; index < mResourceSelector->count(); ++index) {
        const AgentInstance agent = mResourceSelector->itemData(index, AgentInstanceModel::InstanceRole).value<AgentInstance>();
        if (agent.isValid() && (agent.identifier() == id)) {
            return index;
        }
    }

    return -1;
}

void MainWindow::raiseMainWindowAndDialog(QWidget *dialog)
{
    setWindowState(windowState() & ~Qt::WindowMinimized);
    raise();

    dialog->raise();
    dialog->activateWindow();
}

Page *MainWindow::pageForType(DetailsType type) const
{
    foreach(Page *page, mPages) {
        if (page->detailsType() == type) {
            return page;
        }
    }
    return nullptr;
}

void MainWindow::processPendingImports()
{
    foreach (const QString &filePath, mPendingImportPaths)
        slotImportCsvFile(filePath);

    mPendingImportPaths.clear();
}

void MainWindow::slotHideOverlay()
{
    mLoadingOverlay->hide();
}

bool MainWindow::loadSavedSearch(const QString &searchName)
{
    qCDebug(FATCRM_CLIENT_LOG) << "Loading saved search:" << searchName;

    // Make sure user is on the Opportunities Page
    mUi->tabWidget->setCurrentIndex(1);

    mLoadedSearchPrefix = ClientSettings::self()->searchPrefixFromName(searchName);
    mOpportunitiesPage->loadSearch(mLoadedSearchPrefix);

    QString searchText = ClientSettings::self()->searchText(searchName);
    currentPage()->setSearchText(searchText);

    mLoadedSearchName = searchName;
    return !mLoadedSearchPrefix.isEmpty();
}

void MainWindow::populateSavedSearchesMenu()
{
    const QStringList recentSearches = ClientSettings::self()->recentlyUsedSearches();
    const int count = qMin(recentSearches.count(), 5);

    mSavedSearchesMenu->clear();
    for (int x = 0; x < count; ++x) {
        const QString &searchName = recentSearches.at(x);
        auto *searchAlternative = new QAction(searchName, this);
        mSavedSearchesMenu->addAction(searchAlternative);
    }

    mSavedSearchesMenu->addSeparator();

    mLoadedSearchName = ClientSettings::self()->searchNameFromPrefix(mLoadedSearchPrefix);
    QString saveActionName = mLoadedSearchPrefix.isEmpty() ? i18n("Save...") : i18n("Save \"%1\"", mLoadedSearchName);
    auto *saveCurrentSearch = new QAction(QIcon::fromTheme("document-save"), saveActionName, this);
    mSavedSearchesMenu->addAction(saveCurrentSearch);
    connect(saveCurrentSearch, &QAction::triggered, this, &MainWindow::slotSaveSearch);
    if (mLoadedSearchPrefix.isEmpty()) {
        saveCurrentSearch->setDisabled(true);
    }

    QAction *saveSearchAs = new QAction(QIcon::fromTheme("document-save-as"), i18n("Save Search As..."), this);
    mSavedSearchesMenu->addAction(saveSearchAs);
    connect(saveSearchAs, &QAction::triggered, this, &MainWindow::slotSaveSearchAs);

    QAction *manageSearches = new QAction(QIcon::fromTheme("document-import"), i18n("Load Saved Search..."), this);
    mSavedSearchesMenu->addAction(manageSearches);
    connect(manageSearches, &QAction::triggered, this, &MainWindow::slotOpenSearchesDialog);
}

void MainWindow::slotLoadSearchFromRecent(QAction *searchAction)
{
    const QStringList recentSearches = ClientSettings::self()->recentlyUsedSearches();
    const QString selectedSearchName = searchAction->iconText();
    auto it = std::find(recentSearches.begin(),
                        recentSearches.end(),
                        selectedSearchName);

    if (it != recentSearches.end()) {
        loadSavedSearch(selectedSearchName);
    }
}

void MainWindow::slotSaveSearch()
{
    if (!mLoadedSearchPrefix.isEmpty()) {
        mOpportunitiesPage->setSearchPrefix(mLoadedSearchPrefix);
        mOpportunitiesPage->setSearchName(mLoadedSearchName);
        mOpportunitiesPage->setSearchText(currentPage()->searchText());
        mOpportunitiesPage->saveSearch();
    } else {
        slotSaveSearchAs();
    }
}

void MainWindow::slotSaveSearchAs()
{
    AddSearchDialog dialog(this, false, mLoadedSearchName, currentPage()->searchText());
    dialog.setWindowTitle(i18n("Save Search As"));
    dialog.exec();
}

void MainWindow::slotActivateNextTab()
{
    mUi->tabWidget->setCurrentIndex((mUi->tabWidget->currentIndex() + 1) % mUi->tabWidget->count());
}

void MainWindow::slotActivatePreviousTab()
{
    mUi->tabWidget->setCurrentIndex((mUi->tabWidget->currentIndex() - 1) % mUi->tabWidget->count());
}
