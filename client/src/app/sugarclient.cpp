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

#include "sugarclient.h"

#include "dbuswinidprovider.h"
#include "enums.h"
#include "resourceconfigdialog.h"
#include "clientsettings.h"
#include "configurationdialog.h"
#include "collectionmanager.h"
#include "notesrepository.h"
#include "reportpage.h"
#include "fatcrm_version.h"

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
#include <QFileDialog>
#include <contactsimporter.h>
#include <QMessageBox>
#include <accountimportdialog.h>

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

void SugarClient::slotAboutApp()
{
    QMessageBox::about(this, i18n("About FatCRM"), i18n("A desktop application for SugarCRM\n\nVersion %1\n\n(C) 2010-2015 Klarälvdalens Datakonsult AB (KDAB)", QString(s_version)));
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
    // the other menus are handled in Qt Designer

    mViewMenu = new QMenu(tr("&View"), this);
    menuBar()->insertMenu(mUi.menuSettings->menuAction(), mViewMenu);
    QAction *printAction = new QAction(tr("Print Report..."), this);
    printAction->setShortcut(QKeySequence::Print);
    printAction->setIcon(QIcon(":/icons/document-print-preview.png"));
    connect(printAction, SIGNAL(triggered()), this, SLOT(slotPrintReport()));
    mViewMenu->addAction(printAction);
    mViewMenu->addSeparator();

    QToolBar *detailsToolBar = addToolBar(tr("Details Toolbar"));
    mShowDetails = new QCheckBox(tr("Show Details"));
    detailsToolBar->addWidget(mShowDetails);
    connect(mShowDetails, SIGNAL(toggled(bool)), SLOT(slotShowDetails(bool)));
    detailsToolBar->addAction(printAction);
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
    connect(mUi.actionImportContacts, SIGNAL(triggered()), this, SLOT(slotImportContacts()));
    connect(mUi.actionOfflineMode, SIGNAL(toggled(bool)), this, SLOT(slotToggleOffline(bool)));
    connect(mUi.actionSynchronize, SIGNAL(triggered()), this, SLOT(slotSynchronize()));
    connect(mUi.actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    connect(mUi.actionAboutFatCRM, SIGNAL(triggered()), this, SLOT(slotAboutApp()));

    // NOT FINISHED YET
    mUi.actionImportContacts->setVisible(false);

    connect(mUi.actionConfigureFatCRM, SIGNAL(triggered()), this, SLOT(slotConfigure()));

    Q_FOREACH (const Page *page, mPages) {
        connect(page, SIGNAL(statusMessage(QString)), this, SLOT(slotShowMessage(QString)));
        connect(page, SIGNAL(showDetailsChanged(bool)), this, SLOT(slotPageShowDetailsChanged()));
        connect(page, SIGNAL(synchronizeCollection(Akonadi::Collection)), this, SLOT(slotSynchronizeCollection(Akonadi::Collection)));
        connect(page, SIGNAL(openObject(DetailsType,QString)),
                this, SLOT(slotOpenObject(DetailsType,QString)));
    }
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
        // TODO store current resource, and on change, clear caches (account repo, reference data...)
    } else {
        mUi.actionSynchronize->setEnabled(false);
        mUi.actionOfflineMode->setEnabled(false);
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
        if (online) {
            Q_FOREACH (Page *page, mPages) {
                page->retrieveResourceUrl();
            }
        }
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
    Page *page = currentPage();
    if (page)
        mShowDetails->setChecked(page->showsDetails());
}

void SugarClient::slotCurrentTabChanged(int index)
{
    if (index < mPages.count()) {
        mShowDetails->setChecked(mPages[ index ]->showsDetails());
    }
}

void SugarClient::slotImportContacts()
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
    Page *page = currentPage();
    if (page)
        page->printReport();
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

void SugarClient::slotOppModelCreated(ItemsTreeModel *model)
{
    mReportPage->setOppModel(model);
}

void SugarClient::slotOpenObject(DetailsType type, const QString &id)
{
    Page *page = pageForType(type);
    if (page) {
        page->openDialog(id);
    }
}

Page *SugarClient::currentPage() const
{
    const int index = mUi.tabWidget->currentIndex();
    if (index <= mPages.count())
        return mPages[ index ];
    return 0;
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

Page *SugarClient::pageForType(DetailsType type) const
{
    foreach(Page *page, mPages) {
        if (page->detailsType() == type) {
            return page;
        }
    }
    return 0;
}

#include "sugarclient.moc"
