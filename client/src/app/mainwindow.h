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

#include "ui_mainwindow.h"
#include "accountspage.h"
#include "campaignspage.h"
#include "contactspage.h"
#include "enums.h"
#include "leadspage.h"
#include "opportunitiespage.h"
#include "fatcrmprivate_export.h"

#include <QMainWindow>

class QAction;
class QCheckBox;
class QComboBox;
class QProgressBar;
class QTimer;
class QToolBar;
class ResourceConfigDialog;
class CollectionManager;
class NotesRepository;
class ReportPage;

namespace Akonadi
{
class AgentInstance;
}

class FATCRMPRIVATE_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

    ~MainWindow();

Q_SIGNALS:
    void resourceSelected(const QByteArray &identifier);
    void onlineStatusChanged(bool online);

protected:
    void closeEvent(QCloseEvent *event);

private:
    void initialize();
    void createActions();
    void setupActions();
    void createTabs();
    void updateWindowTitle(bool online);

    QList<Page *> mPages;
    QCheckBox *mShowDetails;

    QComboBox *mResourceSelector;

    Ui_MainWindow mUi;
    QMenu *mViewMenu;

    QProgressBar *mProgressBar;
    QTimer *mProgressBarHideTimer;
    ReportPage *mReportPage;

    ResourceConfigDialog *mResourceDialog;
    CollectionManager *mCollectionManager;
    NotesRepository *mNotesRepository;

    Page *mAccountPage;

    QToolBar *mMainToolBar;
    QAction *mResourceSelectorAction;
    QList<KJob *> mClearTimestampJobs;

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
    void slotConfigureResources();
    void slotResourceError(const Akonadi::AgentInstance &resource, const QString &message);
    void slotResourceOnline(const Akonadi::AgentInstance &resource, bool online);
    void slotResourceProgress(const Akonadi::AgentInstance &resource);
    void slotShowDetails(bool on);
    void slotPageShowDetailsChanged();
    void slotCurrentTabChanged(int index);
    void slotImportContacts();
    void slotConfigure();
    void slotPrintReport();
    void slotCollectionResult(const QString &mimeType, const Akonadi::Collection& collection);
    void slotIgnoreModifications(bool ignore);
    void slotOppModelCreated(ItemsTreeModel *model);
    void slotOpenObject(DetailsType type, const QString &id);
    void slotClearTimestampResult(KJob*);


private:
    Page *currentPage() const;
    Page *pageForType(DetailsType type) const;
    void setupResourcesCombo();
    Akonadi::AgentInstance currentResource() const;
    void initialResourceSelection();
};

#endif
