#ifndef ACCOUNTREPOSITORY_H
#define ACCOUNTREPOSITORY_H

#include <QMap>
#include <QVector>
#include <sugaraccount.h>

class AccountRepository
{
public:
    static AccountRepository *instance();
    ~AccountRepository();

    void addAccount(const SugarAccount &account);

    QList<SugarAccount> similarAccounts(const SugarAccount &account) const;
    QList<SugarAccount> accountsByKey(const QString &key) const;

private:
    AccountRepository();

    typedef QMap<QString, SugarAccount> Map;
    Map mKeyMap;
    Map mNameMap;
};

#endif // ACCOUNTREPOSITORY_H
