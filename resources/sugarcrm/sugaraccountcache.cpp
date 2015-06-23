#include "sugaraccountcache.h"

#include <KSharedConfig>
#include <KConfigGroup>

SugarAccountCache *SugarAccountCache::instance()
{
    static SugarAccountCache s_cache;
    return &s_cache;
}

SugarAccountCache::SugarAccountCache(QObject *parent) :
    QObject(parent)
{
    restore();
}

void SugarAccountCache::addAccount(const QString &name, const QString &id)
{
    mAccountIdForName.insert(name, id);
    if (mPendingAccountNames.contains(name)) {
        emit pendingAccountAdded(name, id);
        mPendingAccountNames.remove(name);
        save();
    }
}

QString SugarAccountCache::accountIdForName(const QString &name) const
{
    return mAccountIdForName.value(name);
}

void SugarAccountCache::addPendingAccountName(const QString &name)
{
    mPendingAccountNames.insert(name);
    save();
}

void SugarAccountCache::save()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group(config, "Cache");
    group.writeEntry("PendingAccountNames", mPendingAccountNames.toList());
}

void SugarAccountCache::restore()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group(config, "Cache");
    mPendingAccountNames = group.readEntry("PendingAccountNames", QStringList()).toSet();
}
