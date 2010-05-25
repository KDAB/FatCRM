#ifndef SUGARCLIENT_H
#define SUGARCLIENT_H

#include "ui_mainwindow.h"
#include <contactdetails.h>
#include <accountdetails.h>
#include <accountspage.h>
#include <contactspage.h>

#include <QMainWindow>

class QAction;

class SugarClient : public QMainWindow
{
    Q_OBJECT
public:
    SugarClient();

    ~SugarClient();

    inline ContactDetails * contactDetailsWidget() {
        return mContactDetailsWidget;
    }

    inline AccountDetails * accountDetailsWidget() {
        return mAccountDetailsWidget;
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
    QDockWidget *mContactDetailsDock;
    ContactDetails *mContactDetailsWidget;
    QAction *mViewContactAction;
    QDockWidget *mAccountDetailsDock;
    AccountDetails *mAccountDetailsWidget;
    QAction *mViewAccountAction;
    ContactsPage *mContactsPage;
    AccountsPage *mAccountsPage;
    Ui_MainWindow mUi;

private Q_SLOTS:
    void slotDelayedInit();
    void slotResourceSelectionChanged( int index );
    void slotContactItemChanged();
    void slotAccountItemChanged();
    void slotShowMessage( const QString& );
    void slotShowContactDetailsDock();
    void slotShowAccountDetailsDock();
    void slotManageItemDetailsView( int currentTab );
};

#endif
