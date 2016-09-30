/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Akonadi/Collection>
#include "ui_mainwindow.h"
#include "enums.h"

#include <QMainWindow>

class ItemsTreeModel;
class KJob;
class QAction;
class QCheckBox;
class QComboBox;
class QProgressBar;
class QTimer;
class QToolBar;
class ResourceConfigDialog;
class CollectionManager;
class LinkedItemsRepository;
class Page;
class ReportPage;
class LoadingOverlay;
class ContactsPage;

namespace Akonadi
{
class AgentInstance;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(bool displayOverlay);

    ~MainWindow() override;

    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void resourceSelected(const QByteArray &identifier);
    void onlineStatusChanged(bool online);

protected:
    void closeEvent(QCloseEvent *event);

private Q_SLOTS:
    void slotDelayedInit();
    void slotAboutApp();
    void slotToggleOffline(bool offline);
    void slotSynchronize();
    void slotSynchronizeCollection(const Akonadi::Collection &collection);
    void slotFullReload();
    void slotResourceSelectionChanged(int index);
    void slotResourceSelected(const Akonadi::AgentInstance &resource);
    void slotResourceCountChanged();
    void slotServerStarted();
    void slotShowMessage(const QString &);
    void slotModelLoaded(DetailsType type);
    void slotNotesLoaded(int count);
    void slotEmailsLoaded(int count);
    void slotDocumentsLoaded(int count);
    void slotConfigureResources();
    void slotResourceError(const Akonadi::AgentInstance &resource, const QString &message);
    void slotResourceOnline(const Akonadi::AgentInstance &resource, bool online);
    void slotResourceProgress(const Akonadi::AgentInstance &resource);
    void slotImportContacts();
    void slotConfigure();
    void slotPrintReport();
    void slotCollectionResult(const QString &mimeType, const Akonadi::Collection& collection);
    void slotOppModelCreated(ItemsTreeModel *model);
    void slotContactsModelCreated(ItemsTreeModel *model);
    void slotOpenObject(DetailsType type, const QString &id);
    void slotClearTimestampResult(KJob*);
    void slotTryImportCsvFile(const QString &filePath);
    void slotImportCsvFile(const QString &filePath);
    void slotHideOverlay();

private:
    void initialize(bool displayOverlay);
    void createActions();
    void setupActions();
    void createTabs();
    void addPage(Page *page);
    void updateWindowTitle(bool online);

    Page *currentPage() const;
    Page *pageForType(DetailsType type) const;
    void setupResourcesCombo();
    Akonadi::AgentInstance currentResource() const;
    void initialResourceSelection();
    void initialLoadingDone();
    void processPendingImports();
    void showResourceDialog();

    QList<Page *> mPages;

    QComboBox *mResourceSelector;

    Ui_MainWindow mUi;
    QMenu *mViewMenu;

    QProgressBar *mProgressBar;
    QTimer *mProgressBarHideTimer;
    ReportPage *mReportPage;

    ResourceConfigDialog *mResourceDialog;
    CollectionManager *mCollectionManager;
    LinkedItemsRepository *mLinkedItemsRepository;

    Page *mAccountPage;
    ContactsPage *mContactsPage;
    ItemsTreeModel *mContactsModel;

    QToolBar *mMainToolBar;
    QAction *mResourceSelectorAction;
    QList<KJob *> mClearTimestampJobs;

    bool mInitialLoadingDone;
    bool mDisplayOverlay;
    QStringList mPendingImportPaths;
    LoadingOverlay *mLoadingOverlay;


};

#endif
