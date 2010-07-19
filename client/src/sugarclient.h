#ifndef SUGARCLIENT_H
#define SUGARCLIENT_H

#include "ui_mainwindow.h"
#include <accountdetails.h>
#include <opportunitydetails.h>
#include <leaddetails.h>
#include <campaigndetails.h>
#include <contactdetails.h>
#include <accountspage.h>
#include <opportunitiespage.h>
#include <leadspage.h>
#include <contactspage.h>
#include <campaignspage.h>

#include <QMainWindow>

class QAction;

class SugarClient : public QMainWindow
{
    Q_OBJECT
public:
    SugarClient();

    ~SugarClient();

    inline AccountDetails *accountDetailsWidget() {
        return mAccountDetailsWidget;
    }

    inline OpportunityDetails *opportunityDetailsWidget() {
        return mOpportunityDetailsWidget;
    }

    inline LeadDetails *leadDetailsWidget() {
        return mLeadDetailsWidget;
    }

    inline ContactDetails *contactDetailsWidget() {
        return mContactDetailsWidget;
    }

    inline CampaignDetails *campaignDetailsWidget() {
        return mCampaignDetailsWidget;
    }

Q_SIGNALS:
    void resourceSelected( const QByteArray &identifier );

private:
    void initialize();
    void createMenus();
    void createToolBars();
    void createDockWidgets();
    void setupActions();
    void createTabs();

    QMenu *mViewMenu;
    QDockWidget *mAccountDetailsDock;
    AccountDetails *mAccountDetailsWidget;
    QAction *mViewAccountAction;
    QDockWidget *mOpportunityDetailsDock;
    OpportunityDetails *mOpportunityDetailsWidget;
    QAction *mViewOpportunityAction;
    QDockWidget *mLeadDetailsDock;
    LeadDetails *mLeadDetailsWidget;
    QAction *mViewLeadAction;
    QDockWidget *mContactDetailsDock;
    ContactDetails *mContactDetailsWidget;
    QAction *mViewContactAction;
    QDockWidget *mCampaignDetailsDock;
    CampaignDetails *mCampaignDetailsWidget;
    QAction *mViewCampaignAction;
    AccountsPage *mAccountsPage;
    OpportunitiesPage *mOpportunitiesPage;
    LeadsPage *mLeadsPage;
    ContactsPage *mContactsPage;
    CampaignsPage *mCampaignsPage;

    QComboBox * mResourceSelector; // Login;

    Ui_MainWindow mUi;

private Q_SLOTS:
    void slotDelayedInit();
    void slotReload();
    void slotResourceSelectionChanged( int index );
    void slotAccountItemChanged();
    void slotOpportunityItemChanged();
    void slotContactItemChanged();
    void slotLeadItemChanged();
    void slotCampaignItemChanged();
    void slotShowMessage( const QString& );
    void slotShowAccountDetailsDock();
    void slotShowOpportunityDetailsDock();
    void slotShowLeadDetailsDock();
    void slotShowContactDetailsDock();
    void slotShowCampaignDetailsDock();
    void slotManageItemDetailsView( int currentTab );
    void slotLogin();

private:
    QComboBox* getResourcesCombo();
};

#endif
