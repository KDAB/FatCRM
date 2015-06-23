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
