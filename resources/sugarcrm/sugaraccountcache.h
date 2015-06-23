#ifndef SUGARACCOUNTCACHE_H
#define SUGARACCOUNTCACHE_H

#include <QObject>
#include <QHash>
#include <QSet>

class SugarAccountCache : public QObject
{
    Q_OBJECT
public:
    static SugarAccountCache *instance();

    void addAccount(const QString &name, const QString &id);

    QString accountIdForName(const QString &name) const;

    // Remember that some opportunities are waiting for this account name to appear
    void addPendingAccountName(const QString &name);

    void save();
    void restore();

signals:
    void pendingAccountAdded(const QString &accountName, const QString &accountId);

private:
    explicit SugarAccountCache(QObject *parent = 0);

    QHash<QString /*id*/, QString /*name*/> mAccountIdForName;

    QSet<QString> mPendingAccountNames;

};

#endif // SUGARACCOUNTCACHE_H
