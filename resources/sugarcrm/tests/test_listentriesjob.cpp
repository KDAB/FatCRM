/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2017-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Jeremy Entressangle <jeremy.entressangle@kdab.com>

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

#include <QTest>
#include <QDebug>
#include "sugarmockprotocol.h"
#include "listentriesjob.h"
#include "accountshandler.h"
#include "opportunitieshandler.h"
#include "campaignshandler.h"
#include "sugarsession.h"
#include <QSignalSpy>
#include <AkonadiCore/Item>

Q_DECLARE_METATYPE(Module)

class TestListEntriesJob : public QObject
{
    Q_OBJECT
private:

    void verifyOpportunities(const Akonadi::Item::List &lItems, const QList<QString> &id, const QList<QString> &name, const QList<QString> &accountId)
    {
        for (int i = 0; i < lItems.size(); i++)
        {
            SugarOpportunity so = lItems.at(i).payload<SugarOpportunity>();
            QCOMPARE(so.id(), id[i]);
            QCOMPARE(so.name(), name[i]);
            QCOMPARE(so.accountId(), accountId[i]);
        }
    }

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Akonadi::Item::List>("Akonadi::Item::List");
    }

    void shouldSetModule()
    {
        //GIVEN
        Akonadi::Collection collection;
        SugarSession session(nullptr);
        ListEntriesJob listEntries(collection, &session);
        AccountsHandler module(&session);
        //WHEN
        listEntries.setModule(&module);
        //THEN
        QCOMPARE(&module, listEntries.module());
    }

    void shouldReturnCollection()
    {
        //GIVEN
        Akonadi::Collection collection1;
        collection1.setId(0);
        Akonadi::Collection collection2;
        collection2.setId(1);
        SugarSession session(nullptr);
        //WHEN
        ListEntriesJob lej(collection1, &session);
        //THEN
        QVERIFY(lej.collection() == collection1);
        QVERIFY(lej.collection() != collection2);
    }

    void shouldReturnCorrectCount_data()
    {
        QTest::addColumn<Module>("moduleName");
        QTest::addColumn<int>("expectedCount");

        QTest::newRow("Accounts") << Module::Accounts << 3;
        QTest::newRow("Opportunities") << Module::Opportunities << 2;
        QTest::newRow("Campaigns") << Module::Campaigns << 1;
    }

    void shouldReturnCorrectCount()
    {
        QFETCH(Module, moduleName);
        QFETCH(int, expectedCount);
        //GIVEN
        Akonadi::Collection collection;
        collection.setId(1);
        auto *protocol = new SugarMockProtocol;
        protocol->addData();
        SugarSession session(nullptr);
        protocol->setSession(&session);
        session.setProtocol(protocol);
        session.setSessionParameters("user", "password", "hosttest");
        ModuleHandler *handler = 0;
        if (moduleName == Module::Accounts) {
            handler = new AccountsHandler(&session);
        } else if (moduleName == Module::Opportunities) {
            handler = new OpportunitiesHandler(&session);
        } else if (moduleName == Module::Campaigns) {
            handler = new CampaignsHandler(&session);
        }
        auto *job = new ListEntriesJob(collection, &session);
        job->setModule(handler);
        QSignalSpy spy(job, SIGNAL(totalItems(int)));
        //WHEN
        QVERIFY(job->exec());
        //THEN
        QCOMPARE(spy.count(), 1);
        const QList<QVariant> arguments = spy.at(0);
        QCOMPARE(arguments.at(0).toInt(), expectedCount);

        delete handler;
    }

    void shouldReturnCorrectAccountsList()
    {
        //GIVEN
        Akonadi::Collection collection;
        collection.setId(1);
        auto *protocol = new SugarMockProtocol;
        protocol->addAccounts();
        SugarSession session(nullptr);
        protocol->setSession(&session);
        session.setProtocol(protocol);
        session.setSessionParameters("user", "password", "hosttest");
        AccountsHandler handler(&session);
        auto *job = new ListEntriesJob(collection, &session);
        job->setModule(&handler);
        QSignalSpy spy(job, SIGNAL(itemsReceived(Akonadi::Item::List,bool)));
        //WHEN
        QVERIFY(job->exec());
        //THEN
        QCOMPARE(spy.count(), 1);
        const QList<QVariant> arguments = spy.at(0);
        const QString id[3] = {"0", "1", "2"};
        const QString name[3] = {"accountZero", "accountOne", "accountTwo"};
        const Akonadi::Item::List lItems = arguments.at(0).value<Akonadi::Item::List>();
        QCOMPARE(lItems.size(), 3);
        for(int i = 0; i < 3; i++)
        {
            SugarAccount sa = lItems[i].payload<SugarAccount>();
            QCOMPARE(sa.id(), id[i]);
            QCOMPARE(sa.name(), name[i]);
        }
        QCOMPARE(job->deletedItems().count(), 0);
    }

    // Depends on shouldReturnCorrectAccountsList()
    void shouldReturnCorrectOpportunitiesList()
    {
        //GIVEN
        Akonadi::Collection collection;
        collection.setId(1);
        auto *protocol = new SugarMockProtocol;
        protocol->addOpportunities();
        SugarSession session(nullptr);
        protocol->setSession(&session);
        session.setProtocol(protocol);
        session.setSessionParameters("user", "password", "hosttest");
        OpportunitiesHandler handler(&session);
        auto *job = new ListEntriesJob(collection, &session);
        job->setModule(&handler);
        QSignalSpy spy(job, SIGNAL(itemsReceived(Akonadi::Item::List,bool)));
        //WHEN
        QVERIFY(job->exec());
        //THEN
        QCOMPARE(spy.count(), 1);
        const QList<QVariant> arguments = spy.at(0);
        const QList<QString> id = {"100", "101"};
        const QList<QString> name = {"validOpp", "oppWithNonExistingAccount"};
        QList<QString> accountId = {"0", "doesNotExist"};
        Akonadi::Item::List lItems = arguments.at(0).value<Akonadi::Item::List>();
        QCOMPARE(lItems.size(), 2);
        verifyOpportunities(lItems, id, name, accountId);
        QCOMPARE(job->deletedItems().count(), 0);
    }

    void shouldReturnSoapError()
    {
        Akonadi::Collection collection;
        collection.setId(1);
        auto *protocol = new SugarMockProtocol;
        SugarSession session(nullptr);
        protocol->setSession(&session);
        session.setProtocol(protocol);
        session.setSessionParameters("user", "password", "hosttest");
        ModuleHandler *handler = new AccountsHandler(&session);
        auto *job = new ListEntriesJob(collection, &session);
        job->setModule(handler);
        const QString errStr = "The session ID is invalid";
        //WHEN
        protocol->setNextSoapError(errStr);
        QVERIFY(!job->exec());
        // THEN
        QCOMPARE(job->error(), int(SugarJob::SoapError));
        QCOMPARE(job->errorString(), errStr);
        QCOMPARE(job->deletedItems().count(), 0);
    }
};

QTEST_MAIN(TestListEntriesJob)
#include "test_listentriesjob.moc"
