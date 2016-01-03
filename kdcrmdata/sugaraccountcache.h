/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef SUGARACCOUNTCACHE_H
#define SUGARACCOUNTCACHE_H

#include "kdcrmdata_export.h"

#include <QObject>
#include <QHash>
#include <QSet>

class KDCRMDATA_EXPORT SugarAccountCache : public QObject
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

    void clear(); // for the unittest

signals:
    void pendingAccountAdded(const QString &accountName, const QString &accountId);

private:
    explicit SugarAccountCache(QObject *parent = 0);

    QHash<QString /*id*/, QString /*name*/> mAccountIdForName;

    QSet<QString> mPendingAccountNames;

};

#endif // SUGARACCOUNTCACHE_H
