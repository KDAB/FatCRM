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

#ifndef SUGARCLIENT_H
#define SUGARCLIENT_H

#include "ui_mainwindow.h"
#include <accountspage.h>
#include <opportunitiespage.h>
#include <leadspage.h>
#include <contactspage.h>
#include <campaignspage.h>
#include "enums.h"

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

class SugarClient : public QMainWindow
{
    Q_OBJECT
public:
    SugarClient();

    ~SugarClient();

Q_SIGNALS:
    void resourceSelected(const QByteArray &identifier);

protected:
    void closeEvent(QCloseEvent *event);

private:
    void initialize();
    void createActions();
    void setupActions();
    void createTabs();

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

private Q_SLOTS:
    void slotDelayedInit();
    void slotAboutApp();
    void slotToggleOffline(bool offline);
    void slotSynchronize();
    void slotSynchronizeCollection(const Akonadi::Collection &collection);
    void slotResourceSelectionChanged(int index);
    void slotResourceSelected(const Akonadi::AgentInstance &resource);
    void slotShowMessage(const QString &);
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


private:
    Page *currentPage() const;
    Page *pageForType(DetailsType type) const;
    QComboBox *createResourcesCombo();
    Akonadi::AgentInstance currentResource() const;
    void initialResourceSelection();
};

#endif
