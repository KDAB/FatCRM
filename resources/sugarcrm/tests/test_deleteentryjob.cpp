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
#include "sugarmockprotocol.h"
#include "accountshandler.h"
#include "opportunitieshandler.h"
#include "deleteentryjob.h"


class TestDeleteEntryJob : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void shouldCorectlyDeleteAccount_data()
    {
        QTest::addColumn<QString>("remoteId");
        QTest::addColumn<bool>("exist");

        QTest::newRow("exitingAccount") << "1" << true;
        QTest::newRow("nonexistingAccount") << "5" << false;
    }

    void shouldCorectlyDeleteAccount()
    {
        QFETCH(QString, remoteId);
        QFETCH(bool, exist);
        //GIVEN
        SugarMockProtocol *protocol = new SugarMockProtocol;
        SugarSession session(nullptr);
        session.setSessionParameters("user", "password", "hosttest");
        protocol->addAccounts();
        session.setProtocol(protocol);
        protocol->setSession(&session);

        SugarAccount account;
        account.setId(remoteId);
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(remoteId);
        item.setPayload<SugarAccount>(account);
        DeleteEntryJob job(item, &session, "Accounts");
        AccountsHandler handler(&session);
        protocol->setAccountsHandler(&handler);
        //WHEN
        bool result = job.exec();
        //THEN
        QCOMPARE(result, exist);
        if (exist) {
            QCOMPARE(protocol->accounts().size(), 2);
            QCOMPARE(protocol->accounts().at(0).id(), QString("0"));
            QCOMPARE(protocol->accounts().at(1).id(), QString("2"));
        } else {
            QCOMPARE(protocol->accounts().size(), 3);
        }
    }


    void shouldCorectlyDeleteOpportunity_data()
    {
        QTest::addColumn<QString>("remoteId");
        QTest::addColumn<bool>("exist");

        QTest::newRow("exitingOpportunity") << "101" << true;
        QTest::newRow("nonexistingOpportunity") << "5" << false;
    }

    void shouldCorectlyDeleteOpportunity()
    {
        QFETCH(QString, remoteId);
        QFETCH(bool, exist);
        //GIVEN
        SugarMockProtocol *protocol = new SugarMockProtocol;
        SugarSession session(nullptr);
        session.setSessionParameters("user", "password", "hosttest");
        protocol->addOpportunities();
        session.setProtocol(protocol);
        protocol->setSession(&session);

        SugarOpportunity opp;
        opp.setId(remoteId);
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(remoteId);
        item.setPayload<SugarOpportunity>(opp);
        DeleteEntryJob job(item, &session, "Opportunities");
        OpportunitiesHandler handler(&session);
        protocol->setOpportunitiesHandler(&handler);
        //WHEN
        bool result = job.exec();
        //THEN
        QCOMPARE(result, exist);
        if (exist) {
            QCOMPARE(protocol->opportunities().size(), 1);
            QCOMPARE(protocol->opportunities().at(0).id(), QString("100"));
        } else {
            QCOMPARE(protocol->opportunities().size(), 2);
        }
    }
};

QTEST_MAIN(TestDeleteEntryJob)
#include "test_deleteentryjob.moc"
