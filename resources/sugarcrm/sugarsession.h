#ifndef SUGARSESSION_H
#define SUGARSESSION_H

#include <QObject>

namespace KDSoapGenerated {
    class Sugarsoap;
}

class SugarSession : public QObject
{
    Q_OBJECT

    friend class SugarJob;
    friend class ModuleDebugInterface;
    friend class ModuleHandler;

public:
    enum RequiredAction {
        None,
        ReLogin,
        NewLogin
    };

    explicit SugarSession( QObject* parent = 0 );

    ~SugarSession();

    RequiredAction setSessionParameters( const QString &username, const QString &password,
                                         const QString &host );

    void createSoapInterface();

    QString sessionId() const;

    QString userName() const;
    QString password() const;
    QString host() const;

    void logout();

private:
    void setSessionId( const QString &sessionId );
    KDSoapGenerated::Sugarsoap *soap();

private:
    class Private;
    Private *const d;
};

#endif
