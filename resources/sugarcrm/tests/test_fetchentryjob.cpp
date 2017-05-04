/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QSignalSpy>
#include <Akonadi/Item>
#include "sugaraccount.h"
#include "sugarsession.h"
#include "accountshandler.h"
#include "opportunitieshandler.h"
#include "sugarmockprotocol.h"
#include "fetchentryjob.h"


class TestFetchEntryJob : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void shouldFetchAccount_data()
    {
        QTest::addColumn<QString>("id");
        QTest::addColumn<QString>("expectedId");
        QTest::addColumn<QString>("expectedName");
        QTest::addColumn<bool>("expectedResult");

        QTest::newRow("existingAccount") << "0" << "0" << "accountZero" << true;
        QTest::newRow("nonexistingAccount") << "28" << "" << "" << false;
    }

    void shouldFetchAccount()
    {
        //GIVEN
        QFETCH(QString, id);
        QFETCH(QString, expectedId);
        QFETCH(QString, expectedName);
        QFETCH(bool, expectedResult);

        SugarAccount account;
        account.setId(id);
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(id);
        item.setPayload<SugarAccount>(account);
        SugarMockProtocol *protocol = new SugarMockProtocol;
        protocol->addAccounts();
        SugarSession session(nullptr);
        session.setSessionParameters("user", "password", "hosttest");
        session.setProtocol(protocol);
        protocol->setSession(&session);
        AccountsHandler handler(&session);
        protocol->setAccountsHandler(&handler);
        FetchEntryJob job(item, &session);
        job.setModule(&handler);
        //WHEN
        bool result = job.exec();
        QCOMPARE(result, expectedResult);
        //THEN
        if (result) {
            SugarAccount accountFind = job.item().payload<SugarAccount>();
            QCOMPARE(accountFind.id(), expectedId);
            QCOMPARE(accountFind.name(), expectedName);
        }
    }

    void shouldFetchOpportunity_data()
    {
        QTest::addColumn<QString>("id");
        QTest::addColumn<QString>("expectedId");
        QTest::addColumn<QString>("expectedName");
        QTest::addColumn<bool>("expectedResult");

        QTest::newRow("existingOpportunity") << "100" << "100" << "validOpp" << true;
        QTest::newRow("nonexistingOpportunity") << "28" << "" << "" << false;
    }

    void shouldFetchOpportunity()
    {
        //GIVEN
        QFETCH(QString, id);
        QFETCH(QString, expectedId);
        QFETCH(QString, expectedName);
        QFETCH(bool, expectedResult);

        SugarOpportunity opp;
        opp.setId(id);
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(id);
        item.setPayload<SugarOpportunity>(opp);
        SugarMockProtocol *protocol = new SugarMockProtocol;
        protocol->addOpportunities();
        SugarSession session(nullptr);
        session.setSessionParameters("user", "password", "hosttest");
        session.setProtocol(protocol);
        protocol->setSession(&session);
        OpportunitiesHandler handler(&session);
        protocol->setOpportunitiesHandler(&handler);
        FetchEntryJob job(item, &session);
        job.setModule(&handler);
        //WHEN
        bool result = job.exec();
        QCOMPARE(result, expectedResult);
        //THEN
        if (result) {
            SugarOpportunity oppFind = job.item().payload<SugarOpportunity>();
            QCOMPARE(oppFind.id(), expectedId);
            QCOMPARE(oppFind.name(), expectedName);
        }
    }
};

QTEST_MAIN(TestFetchEntryJob)
#include "test_fetchentryjob.moc"
