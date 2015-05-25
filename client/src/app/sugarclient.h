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
    void displayDetails();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void initialize();
    void createActions();
    void setupActions();
    void createTabs();

    QList<Page *> mPages;
    QCheckBox *mShowDetails;

    QComboBox *mResourceSelector;  // Login;

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

private:
    Page *currentPage() const;
    QComboBox *createResourcesCombo();
    Akonadi::AgentInstance currentResource() const;
    void initialResourceSelection();
};

#endif
