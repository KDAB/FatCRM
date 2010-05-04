#ifndef SUGARCLIENT_H
#define SUGARCLIENT_H

#include "ui_mainwindow.h"
#include <QMainWindow>

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
    Ui_MainWindow mUi;

private Q_SLOTS:
    void slotDelayedInit();
    void slotResourceSelectionChanged( int index );
};

#endif
