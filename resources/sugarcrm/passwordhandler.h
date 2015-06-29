#ifndef PASSWORDHANDLER_H
#define PASSWORDHANDLER_H

#include <QObject>
#include <QWidget>

#include <config-kwallet.h>

class PasswordHandler : public QObject
{
    Q_OBJECT
public:
    explicit PasswordHandler(const QString &resourceId, QObject *parent = 0);

    bool isPasswordAvailable();
    QString password( bool *userRejected = 0 );
    void setPassword( const QString &password );

Q_SIGNALS:
    void passwordAvailable();

private Q_SLOTS:
    void onWalletOpened(bool success);

private:
#if USE_KWALLET
    WId m_winId;
    bool savePassword();
#endif
    QString mPassword;
    const QString mResourceId;
};

#endif // PASSWORDHANDLER_H
