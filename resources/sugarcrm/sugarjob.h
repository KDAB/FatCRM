#ifndef SUGARJOB_H
#define SUGARJOB_H

#include <KJob>

class KDSoapMessage;
class SugarSession;
namespace KDSoapGenerated
{
class Sugarsoap;
class TNS__Set_entry_result;
class TNS__Error_value;
}

class SugarJob : public KJob
{
    Q_OBJECT

public:
    enum Errors {
        LoginError = KJob::UserDefinedError + 1,
        CouldNotConnectError,
        SoapError,
        InvalidContextError,
        TaskError
    };

    explicit SugarJob(SugarSession *session, QObject *parent = 0);

    ~SugarJob();

    void start();

public Q_SLOTS:
    void restart();

protected:
    virtual bool doKill();
    virtual void startSugarTask() = 0;

    bool handleError(const KDSoapGenerated::TNS__Error_value &errorValue);
    bool handleLoginError(const KDSoapMessage &fault);

    QString sessionId() const;
    KDSoapGenerated::Sugarsoap *soap();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void startLogin())
    Q_PRIVATE_SLOT(d, void startTask())
    Q_PRIVATE_SLOT(d, void loginDone(const KDSoapGenerated::TNS__Set_entry_result &callResult))
    Q_PRIVATE_SLOT(d, void loginError(const KDSoapMessage &fault))
};

#endif
