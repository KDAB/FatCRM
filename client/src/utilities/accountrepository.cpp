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
