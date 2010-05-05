#ifndef SUGARCLIENT_H
#define SUGARCLIENT_H

#include "ui_mainwindow.h"
#include <contactdetails.h>

#include <QMainWindow>

#include <akonadi/item.h>

class SugarClient : public QMainWindow
{
    Q_OBJECT
public:
    SugarClient();

    ~SugarClient();

Q_SIGNALS:
    void resourceSelected( const QByteArray &identifier );

private:
    void initialize();
    void createMenus();
    void createDockWidgets();

    QMenu *mViewMenu;
    QDockWidget *mContactDetailsDock;
    ContactDetails *mContactDetailsWidget;
    Ui_MainWindow mUi;

private Q_SLOTS:
    void slotDelayedInit();
    void slotResourceSelectionChanged( int index );
    void slotContactItemChanged( const Akonadi::Item& item );
};

#endif
