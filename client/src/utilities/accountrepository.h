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

#ifndef ACCOUNTREPOSITORY_H
#define ACCOUNTREPOSITORY_H

#include "sugaraccount.h"
#include "fatcrmprivate_export.h"

#include <QMap>
#include <QVector>

#include <AkonadiCore/item.h>

class FATCRMPRIVATE_EXPORT AccountRepository : public QObject
{
    Q_OBJECT
public:
    static AccountRepository *instance();
    ~AccountRepository() override;

    // Not a full list of fields, just the ones for which we care for modifications
    enum Field
    {
        Name,
        Country
    };

    void clear();
    void addAccount(const SugarAccount &account, Akonadi::Item::Id akonadiId);
    void removeAccount(const SugarAccount &account);
    /**
     * Called when account has been modified.
     */
    QVector<AccountRepository::Field> modifyAccount(const SugarAccount &account);

    SugarAccount accountById(const QString &id) const;
    QStringList countries() const;
    bool hasId(const QString &id) const;

    QList<SugarAccount> similarAccounts(const SugarAccount &account) const;
    QList<SugarAccount> accountsByKey(const QString &key) const;

    void emitInitialLoadingDone();

signals:
    void initialLoadingDone();

    // warning this is called a lot on startup; only use for later creations
    void accountAdded(const QString &id, Akonadi::Item::Id akonadiId);

    void accountModified(const QString &id, const QVector<AccountRepository::Field> &changedFields);

private:
    AccountRepository();

    typedef QMap<QString, SugarAccount> Map;
    Map mIdMap;
    Map mKeyMap;
    Map mNameMap;
    QSet<QString> mCountries;
};

#endif // ACCOUNTREPOSITORY_H
