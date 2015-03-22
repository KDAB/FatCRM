#ifndef LOGINERRORDIALOG_H
#define LOGINERRORDIALOG_H

#include <KDialog>

class KJob;
class SugarSession;
namespace Akonadi {
    class ResourceBase;
}

class LoginErrorDialog : public KDialog
{
    Q_OBJECT

public:
    LoginErrorDialog( KJob *job, SugarSession *session, Akonadi::ResourceBase *resource, QWidget *parent = 0 );

    ~LoginErrorDialog();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void changeConfig() )
    Q_PRIVATE_SLOT( d, void cancel() )
};

#endif
