#ifndef SUGARCLIENT_H
#define SUGARCLIENT_H

#include "ui_mainwindow.h"
#include <detailswidget.h>
#include <accountspage.h>
#include <opportunitiespage.h>
#include <leadspage.h>
#include <contactspage.h>
#include <campaignspage.h>
#include "enums.h"

#include <QMainWindow>

class QAction;
class QComboBox;
class QProgressBar;
class QTimer;
class QToolBar;
class ResourceConfigDialog;

namespace Akonadi {
    class AgentInstance;
}

class SugarClient : public QMainWindow
{
    Q_OBJECT
public:
    SugarClient();

    ~SugarClient();

    void displayDockWidgets( bool value = true );
    DetailsWidget *detailsWidget( DetailsType type );

Q_SIGNALS:
    void resourceSelected( const QByteArray &identifier );
    void displayDetails();

protected:
    void closeEvent( QCloseEvent *event );

private:
    void initialize();
    void createMenus();
    void createToolBar();
    void createDockWidgets();
    void setupActions();
    void createTabs();

    QMenu *mViewMenu;
    QToolBar *mDetailsToolBar;
    QDockWidget *mAccountDetailsDock;
    DetailsWidget *mAccountDetailsWidget;
    QAction *mViewAccountAction;
    QDockWidget *mOpportunityDetailsDock;
    DetailsWidget *mOpportunityDetailsWidget;
    QAction *mViewOpportunityAction;
    QDockWidget *mLeadDetailsDock;
    DetailsWidget *mLeadDetailsWidget;
    QAction *mViewLeadAction;
    QDockWidget *mContactDetailsDock;
    DetailsWidget *mContactDetailsWidget;
    QAction *mViewContactAction;
    QDockWidget *mCampaignDetailsDock;
    DetailsWidget *mCampaignDetailsWidget;
    QAction *mViewCampaignAction;
    QList<Page*> mPages;

    QComboBox * mResourceSelector; // Login;

    Ui_MainWindow mUi;

    QProgressBar *mProgressBar;
    QTimer *mProgressBarHideTimer;

    ResourceConfigDialog *mResourceDialog;

private Q_SLOTS:
    void slotDelayedInit();
    void slotReload();
    void slotSynchronize();
    void slotResourceSelectionChanged( int index );
    void slotResourceSelected( const Akonadi::AgentInstance &resource );
    void slotShowMessage( const QString& );
    void slotManageItemDetailsView( int currentTab );
    void slotManageDetailsDisplay( bool value );
    void slotDetailsDisplayDisabled( bool value );
    void slotConfigureResources();
    void slotResourceError( const Akonadi::AgentInstance &resource, const QString &message );
    void slotResourceOnline( const Akonadi::AgentInstance &resource, bool online );
    void slotResourceProgress( const Akonadi::AgentInstance &resource );

private:
    void detachDockViews( bool value );
    QComboBox* createResourcesCombo();
    Akonadi::AgentInstance currentResource() const;
    void initialResourceSelection();
};

#endif
