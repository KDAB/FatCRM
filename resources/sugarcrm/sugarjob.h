#ifndef SUGARJOB_H
#define SUGARJOB_H

#include <KJob>

class KDSoapMessage;
class SugarSession;
class TNS__Set_entry_result;

class SugarJob : public KJob
{
    Q_OBJECT

public:
    enum Errors {
        LoginError
    };

    explicit SugarJob( SugarSession* session, QObject* parent = 0);

    ~SugarJob();

    void start();

protected:
    virtual void startSugarTask() = 0;

    bool handleLoginError( const KDSoapMessage &fault );

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void startLogin() )
    Q_PRIVATE_SLOT( d, void startTask() )
    Q_PRIVATE_SLOT( d, void loginDone( const TNS__Set_entry_result &callResult ) )
    Q_PRIVATE_SLOT( d, void loginError( const KDSoapMessage &fault ) )
};

#endif
