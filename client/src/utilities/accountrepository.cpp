/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

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

#include "accountrepository.h"

#include "fatcrm_client_debug.h"

#include <QStringList>

AccountRepository *AccountRepository::instance()
{
    static AccountRepository repo;
    return &repo;
}

AccountRepository::~AccountRepository()
{
}

void AccountRepository::clear()
{
    mIdMap.clear();
    mKeyMap.clear();
    mNameMap.clear();
    mCountries.clear();
}

QStringList AccountRepository::countries() const
{
    return mCountries.toList();
}

void AccountRepository::addAccount(const SugarAccount &account, Akonadi::Item::Id akonadiId)
{
    const QString accountId = account.id();

    Q_ASSERT(!accountId.isEmpty());
    if (mIdMap.contains(accountId)) // can this happen?
        qWarning() << "AccountRepository: already have" << accountId << mIdMap.value(accountId).name() << account.name();
    mIdMap.insert(accountId, account);
    // ## This does not handle the case of renaming accounts later on
    mKeyMap.insertMulti(account.key(), account);
    mNameMap.insertMulti(account.cleanAccountName(), account);
    if (!account.billingAddressCountry().isEmpty()) {
        mCountries.insert(account.billingAddressCountry());
    }
    if (!account.shippingAddressCountry().isEmpty()) {
        mCountries.insert(account.shippingAddressCountry());
    }
    emit accountAdded(accountId, akonadiId);
}

QVector<AccountRepository::Field> AccountRepository::modifyAccount(const SugarAccount &account)
{
    QVector<Field> changedFields;
    const QString accountId = account.id();
    Q_ASSERT(!accountId.isEmpty());
    Map::iterator it = mIdMap.find(accountId);
    if (it != mIdMap.end()) {
        // Existing account modified
        const SugarAccount &oldAccount = *it;
        if (oldAccount.name() != account.name()) {
            qCDebug(FATCRM_CLIENT_LOG) << "account renamed from" << oldAccount.name() << "to" << account.name();
            changedFields.append(Name);
        }
        if (oldAccount.countryForGui() != account.countryForGui()) {
            qCDebug(FATCRM_CLIENT_LOG) << account.name() << ": country modified";
            changedFields.append(Country);
        }

        *it = account;
        if (!changedFields.isEmpty()) {
            emit accountModified(accountId, changedFields);
        }
        // We don't handle changes in mKeyMap and mNameMap, hoping that this doesn't matter
        // since they are only used by the import dialog.
    } else {
        qWarning() << "Account not found " << accountId << "name=" << account.name();
    }
    return changedFields;
}

void AccountRepository::removeAccount(const SugarAccount &account)
{
    const QString id = account.id();
    if (id.isEmpty()) // not yet created on server
        return;
    const QString key = account.key();
    const QString cleanAccountName = account.cleanAccountName();

    mIdMap.remove(id);

    // Be careful not to remove the wrong one if there are duplicates...

    Map::iterator i = mKeyMap.find(key);
    while (i != mKeyMap.end() && i.key() == key) {
        if (i.value().id() == id) {
            mKeyMap.erase(i);
            break;
        }
        ++i;
    }

    i = mNameMap.find(cleanAccountName);
    while (i != mNameMap.end() && i.key() == cleanAccountName) {
        if (i.value().id() == id) {
            mNameMap.erase(i);
            break;
        }
        ++i;
    }

    emit accountRemoved(id);
}

SugarAccount AccountRepository::accountById(const QString &id) const
{
    return mIdMap.value(id);
}

bool AccountRepository::hasId(const QString &id) const
{
    return mIdMap.contains(id);
}

QList<SugarAccount> AccountRepository::similarAccounts(const SugarAccount &account) const
{
    return mNameMap.values(account.cleanAccountName());
}

QList<SugarAccount> AccountRepository::accountsByKey(const QString &key) const
{
    return mKeyMap.values(key);
}

void AccountRepository::emitInitialLoadingDone()
{
    qCDebug(FATCRM_CLIENT_LOG);
    emit initialLoadingDone();
}

AccountRepository::AccountRepository()
{
}
