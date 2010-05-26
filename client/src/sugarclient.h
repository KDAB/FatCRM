#ifndef SUGARCLIENT_H
#define SUGARCLIENT_H

#include "ui_mainwindow.h"
#include <accountdetails.h>
#include <opportunitydetails.h>
#include <contactdetails.h>
#include <accountspage.h>
#include <opportunitiespage.h>
#include <contactspage.h>

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

    inline ContactDetails *contactDetailsWidget() {
        return mContactDetailsWidget;
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
    QDockWidget *mContactDetailsDock;
    ContactDetails *mContactDetailsWidget;
    QAction *mViewContactAction;
    AccountsPage *mAccountsPage;
    OpportunitiesPage *mOpportunitiesPage;
    ContactsPage *mContactsPage;
    Ui_MainWindow mUi;

private Q_SLOTS:
    void slotDelayedInit();
    void slotResourceSelectionChanged( int index );
    void slotAccountItemChanged();
    void slotOpportunityItemChanged();
    void slotContactItemChanged();
    void slotShowMessage( const QString& );
    void slotShowAccountDetailsDock();
    void slotShowOpportunityDetailsDock();
    void slotShowContactDetailsDock();
    void slotManageItemDetailsView( int currentTab );
};

#endif
