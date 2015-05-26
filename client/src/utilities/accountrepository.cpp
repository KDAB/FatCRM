/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

AccountRepository *AccountRepository::instance()
{
    static AccountRepository repo;
    return &repo;
}

AccountRepository::~AccountRepository()
{

}

void AccountRepository::addAccount(const SugarAccount &account)
{
    mKeyMap.insertMulti(account.key(), account);
    mNameMap.insertMulti(account.cleanAccountName(), account);
}

QList<SugarAccount> AccountRepository::similarAccounts(const SugarAccount &account) const
{
    return mNameMap.values(account.name());
}

QList<SugarAccount> AccountRepository::accountsByKey(const QString &key) const
{
    return mKeyMap.values(key);
}

AccountRepository::AccountRepository()
{
}
