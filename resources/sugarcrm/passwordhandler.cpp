/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "passwordhandler.h"
#include "sugarcrmresource_debug.h"
#include <settings.h>

#include <qt5keychain/keychain.h>

static const char s_keychainFolderName[] = "SugarCRM";

// Implementation inspired from kdepim-runtime/resources/imap/settings.cpp branch KDE/4.14

PasswordHandler::PasswordHandler(const QString &resourceId, QObject *parent) :
    QObject(parent),
    mResourceId(resourceId)
{
    auto *job = new QKeychain::ReadPasswordJob(s_keychainFolderName);

    job->setKey(resourceId);

    connect(job, &QKeychain::Job::finished, this, [this, job] {
        if (job->error()) {
            qWarning() << "Error reading password for" << mResourceId << job->errorString();

            if (job->error() == QKeychain::AccessDeniedByUser) {
                mDeniedByUser = true;
            }
        } else {
            mPassword = job->textData();
            mKeychainOpened = true;
            Q_EMIT passwordAvailable();
        }
    });

    job->start();
}

bool PasswordHandler::isPasswordAvailable()
{
    return mKeychainOpened;
}

QString PasswordHandler::password(bool *userRejected)
{
    if (userRejected != nullptr) {
        *userRejected = mDeniedByUser;

        if (mDeniedByUser) {
            return QString();
        }
    }

    if (!mKeychainOpened) {
        return QString();
    }

    // Initial migration: password not in keychain yet
    if (mPassword.isEmpty()) {
        mPassword = Settings::password();
        if (!mPassword.isEmpty()) {
            savePassword();
        }
    }

    return mPassword;
}

void PasswordHandler::setPassword(const QString &password)
{
    if (password == mPassword)
        return;

    mPassword = password;
    savePassword();
}

bool PasswordHandler::savePassword()
{
    auto *job = new QKeychain::WritePasswordJob(s_keychainFolderName);
    job->setKey(mResourceId);
    job->setTextData(mPassword);

    connect(job, &QKeychain::Job::finished, this, [this, job] {
        if (job->error()) {
            qWarning() << "password save error" << job->errorString();
        }
    });

    job->start();

    Settings::setPassword(QString()); // ensure no plain-text password from before in the config file
    Settings::self()->save();

    return true;
}
