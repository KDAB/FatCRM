#ifndef SHUTDOWNDIALOG_H
#define SHUTDOWNDIALOG_H

#include <QDialog>

class ShutdownDialog : public QDialog
{
    Q_OBJECT
public:
    enum ShutdownOptions {
      CancelShutdown,
      JustQuit,
      QuitAndAgentsOffline,
      QuitAndStopAkonadi
    };

    ~ShutdownDialog();

    static ShutdownOptions getShutdownOption( QWidget *parent = 0 );

private:
    class Private;
    Private *const d;

private:
    explicit ShutdownDialog( QWidget *parent = 0 );
};

#endif
