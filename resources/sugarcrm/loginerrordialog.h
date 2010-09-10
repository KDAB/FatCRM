#ifndef LOGINERRORDIALOG_H
#define LOGINERRORDIALOG_H

#include <KDialog>

class KJob;
class SugarSession;

class LoginErrorDialog : public KDialog
{
    Q_OBJECT

public:
    LoginErrorDialog( KJob *job, SugarSession *session, QWidget *parent = 0 );

    ~LoginErrorDialog();

Q_SIGNALS:
    void userOrHostChanged( const QString &user, const QString &host );

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void changeConfig() )
    Q_PRIVATE_SLOT( d, void cancel() )
};

#endif
