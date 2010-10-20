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
    void setupActions();
    void createTabs();

    DetailsWidget *mAccountDetailsWidget;
    DetailsWidget *mOpportunityDetailsWidget;
    DetailsWidget *mLeadDetailsWidget;
    DetailsWidget *mContactDetailsWidget;
    DetailsWidget *mCampaignDetailsWidget;
    QList<Page*> mPages;
    QCheckBox *mShowDetails;

    QComboBox * mResourceSelector; // Login;

    Ui_MainWindow mUi;
    QMenu *mViewMenu;

    QProgressBar *mProgressBar;
    QTimer *mProgressBarHideTimer;

    ResourceConfigDialog *mResourceDialog;

private Q_SLOTS:
    void slotDelayedInit();
    void slotToggleOffline( bool offline );
    void slotSynchronize();
    void slotResourceSelectionChanged( int index );
    void slotResourceSelected( const Akonadi::AgentInstance &resource );
    void slotShowMessage( const QString& );
    void slotConfigureResources();
    void slotResourceError( const Akonadi::AgentInstance &resource, const QString &message );
    void slotResourceOnline( const Akonadi::AgentInstance &resource, bool online );
    void slotResourceProgress( const Akonadi::AgentInstance &resource );
    void slotShowDetails( bool on );
    void slotPageShowDetailsChanged();
    void slotCurrentTabChanged( int index );

private:
    QComboBox* createResourcesCombo();
    Akonadi::AgentInstance currentResource() const;
    void initialResourceSelection();
};

#endif
