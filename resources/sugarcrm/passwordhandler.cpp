#include "passwordhandler.h"

#include <settings.h>

#if USE_KWALLET
#include <kwallet.h>
using KWallet::Wallet;

static const char s_walletFolderName[] = "SugarCRM";
#endif

// Implementation inspired from kdepim-runtime/resources/imap/settings.cpp branch KDE/4.14

PasswordHandler::PasswordHandler(const QString &resourceId, QObject *parent) :
    QObject(parent),
    mResourceId(resourceId)
{
#if USE_KWALLET
    // We have no GUI to use on startup
    // We could at least use the config dialog though.
    m_winId = 0;

    Wallet *wallet = Wallet::openWallet( Wallet::NetworkWallet(), m_winId, Wallet::Asynchronous );
    if ( wallet ) {
      connect( wallet, SIGNAL( walletOpened(bool) ),
               this, SLOT( onWalletOpened(bool) ) );
    }
#endif
}

bool PasswordHandler::isPasswordAvailable()
{
#if USE_KWALLET
    QScopedPointer<Wallet> wallet(Wallet::openWallet(Wallet::NetworkWallet(), m_winId));
    return wallet && wallet->isOpen();
#else
    return true;
#endif
}

QString PasswordHandler::password(bool *userRejected)
{
    if (userRejected != 0) {
        *userRejected = false;
    }

    if (!mPassword.isEmpty())
        return mPassword;
#if USE_KWALLET
    QScopedPointer<Wallet> wallet(Wallet::openWallet(Wallet::NetworkWallet(), m_winId));
    if (wallet && wallet->isOpen()) {
        if (wallet->hasFolder(QString(s_walletFolderName))) {
            wallet->setFolder(QString(s_walletFolderName));
            wallet->readPassword(mResourceId, mPassword);
        } else {
            wallet->createFolder(QString(s_walletFolderName));
        }
        // Initial migration: password not in wallet yet
        if (mPassword.isEmpty()) {
            mPassword = Settings::password();
            if (!mPassword.isEmpty()) {
                savePassword();
            }
        }
    } else if (userRejected != 0) {
        *userRejected = true;
    }
#else
    mPassword = Settings::password();
#endif
    return mPassword;

}

void PasswordHandler::setPassword(const QString &password)
{
    if (password == mPassword)
        return;

    mPassword = password;

#if USE_KWALLET
    savePassword();
#else
    Settings::setPassword(mPassword);
    Settings::self()->writeConfig();
#endif
}

void PasswordHandler::onWalletOpened(bool success)
{
    Wallet *wallet = qobject_cast<Wallet*>( sender() );
    if (wallet && success) {
        // read and store the password
        password();
        emit passwordAvailable();
    }
}

bool PasswordHandler::savePassword()
{
#if USE_KWALLET
    QScopedPointer<Wallet> wallet(Wallet::openWallet(Wallet::NetworkWallet(), m_winId));
    if (wallet && wallet->isOpen()) {
        if (!wallet->hasFolder(QString(s_walletFolderName)))
            wallet->createFolder(QString(s_walletFolderName));
        wallet->setFolder(QString(s_walletFolderName));
        wallet->writePassword(mResourceId, mPassword);
        wallet->sync();
        Settings::setPassword(QString()); // ensure no plain-text password from before in the config file
        Settings::self()->writeConfig();
        return true;
    }
    return false;
#endif
}
