#ifndef SUGARCLIENT_H
#define SUGARCLIENT_H

#include "ui_mainwindow.h"
#include <contactdetails.h>

#include <QMainWindow>


class SugarClient : public QMainWindow
{
    Q_OBJECT
public:
    SugarClient();

    ~SugarClient();

    inline ContactDetails * contactDetailsWidget() {
        return mContactDetailsWidget;
    }

Q_SIGNALS:
    void resourceSelected( const QByteArray &identifier );

private:
    void initialize();
    void createMenus();
    void createDockWidgets();
    void setupActions();

    QMenu *mViewMenu;
    QDockWidget *mContactDetailsDock;
    ContactDetails *mContactDetailsWidget;
    Ui_MainWindow mUi;

private Q_SLOTS:
    void slotDelayedInit();
    void slotResourceSelectionChanged( int index );
    void slotContactItemChanged();
    void slotShowMessage( const QString& );
};

#endif
