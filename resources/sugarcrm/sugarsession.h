#ifndef SUGARSESSION_H
#define SUGARSESSION_H

#include <QObject>

class Sugarsoap;

class SugarSession : public QObject
{
    Q_OBJECT

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

    void setSessionId( const QString &sessionId );
    QString sessionId() const;

    QString userName() const;
    QString password() const;
    QString host() const;

    Sugarsoap *soap();

private:
    class Private;
    Private *const d;
};

#endif
