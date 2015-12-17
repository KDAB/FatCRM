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

#include <QtTest/QtTest>
#include <QDebug>
#include "sugaraccountcache.h"
#include <QSignalSpy>

class TestAccountCache : public QObject
{
    Q_OBJECT
public:
private Q_SLOTS:
    void emptyCacheShouldBeEmpty()
    {
        SugarAccountCache *cache = SugarAccountCache::instance();
        QCOMPARE(cache->accountIdForName(""), QString());
        QCOMPARE(cache->accountIdForName("KDAB"), QString());
    }

    void shouldNotHaveIdImmediately()
    {
        SugarAccountCache *cache = SugarAccountCache::instance();
        cache->addPendingAccountName("KDAB");
        QCOMPARE(cache->accountIdForName("KDAB"), QString());
    }

    void shouldHaveIdWhenAdding()
    {
        SugarAccountCache *cache = SugarAccountCache::instance();
        cache->addPendingAccountName("KDAB");
        QSignalSpy spy(cache, SIGNAL(pendingAccountAdded(QString,QString)));
        cache->addAccount("KDAB", "id_kdab");
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), QString("KDAB"));
        QCOMPARE(spy.at(0).at(1).toString(), QString("id_kdab"));
        QCOMPARE(cache->accountIdForName("KDAB"), QString("id_kdab"));
    }

    void shouldSaveAndRestore()
    {
        SugarAccountCache *cache = SugarAccountCache::instance();
        cache->addPendingAccountName("QTC");
        QSignalSpy spy(cache, SIGNAL(pendingAccountAdded(QString,QString)));
        cache->save();
        cache->clear();
        cache->restore();
        cache->addAccount("QTC", "id_qtc");
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), QString("QTC"));
        QCOMPARE(spy.at(0).at(1).toString(), QString("id_qtc"));
        QCOMPARE(cache->accountIdForName("QTC"), QString("id_qtc"));
    }
};

QTEST_MAIN(TestAccountCache)
#include "test_accountcache.moc"
