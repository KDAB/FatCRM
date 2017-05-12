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
#include "createentryjob.h"
#include "accountshandler.h"
#include "opportunitieshandler.h"


class TestCreateEntryJob : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void shouldAddAccount()
    {
        //GIVEN
        SugarAccount account;
        account.setName("newAccount");
        Akonadi::Item item;
        item.setId(0);
        item.setPayload<SugarAccount>(account);
        SugarMockProtocol *protocol = new SugarMockProtocol;
        SugarSession session(nullptr);
        session.setSessionParameters("user", "password", "hosttest");
        session.setProtocol(protocol);
        protocol->setSession(&session);
        CreateEntryJob job(item, &session);
        AccountsHandler handler(&session);
        protocol->setAccountsHandler(&handler);
        job.setModule(&handler);
        //WHEN
        QVERIFY(job.exec());
        //THEN
        SugarAccount newAccount = protocol->accounts().at(0);
        QCOMPARE(newAccount.name(), QString("newAccount"));
        QCOMPARE(newAccount.id(), QString("1000"));
        SugarAccount newAccountFromJob = job.item().payload<SugarAccount>();
        QCOMPARE(newAccountFromJob.name(), QString("newAccount"));
        QCOMPARE(newAccountFromJob.id(), QString("1000"));
    }

    void shouldAddOpportunity()
    {
        //GIVEN
        SugarOpportunity opp;
        opp.setName("newOpportunity");
        Akonadi::Item item;
        item.setId(0);
        item.setPayload<SugarOpportunity>(opp);
        SugarMockProtocol *protocol = new SugarMockProtocol;
        SugarSession session(nullptr);        
        session.setSessionParameters("user", "password", "hosttest");
        session.setProtocol(protocol);
        protocol->setSession(&session);
        CreateEntryJob job(item, &session);
        OpportunitiesHandler handler(&session);
        protocol->setOpportunitiesHandler(&handler);
        job.setModule(&handler);
        //WHEN
        QVERIFY(job.exec());
        //THEN
        SugarOpportunity newOpportunity = protocol->opportunities().at(0);
        QCOMPARE(newOpportunity.name(), QString("newOpportunity"));
        QCOMPARE(newOpportunity.id(), QString("1000"));
        SugarOpportunity newOpportunityFromJob = job.item().payload<SugarOpportunity>();
        QCOMPARE(newOpportunityFromJob.name(), QString("newOpportunity"));
        QCOMPARE(newOpportunityFromJob.id(), QString("1000"));
    }

    void shouldHandleInvalidContextError()
    {
        //GIVEN
        SugarAccount account;
        account.setName("newAccount");
        Akonadi::Item item;
        item.setId(0);
        SugarMockProtocol *protocol = new SugarMockProtocol;
        SugarSession session(nullptr);
        session.setSessionParameters("user", "password", "hosttest");
        session.setProtocol(protocol);
        protocol->setSession(&session);
        CreateEntryJob job(item, &session);
        AccountsHandler handler(&session);
        protocol->setAccountsHandler(&handler);
        job.setModule(&handler);
        //WHEN
        QVERIFY(!job.exec());
        //THEN
        QCOMPARE(job.error(), int(SugarJob::InvalidContextError));
    }

    void shouldHandleCouldNotConnectError()
    {
        //GIVEN
        SugarAccount account;
        account.setName("newAccount");
        Akonadi::Item item;
        item.setId(0);
        item.setPayload<SugarAccount>(account);
        SugarMockProtocol *protocol = new SugarMockProtocol;
        protocol->setServerNotFound(true);
        SugarSession session(nullptr);
        session.setSessionParameters("user", "password", "hosttest");
        session.setProtocol(protocol);
        protocol->setSession(&session);
        CreateEntryJob job(item, &session);
        AccountsHandler handler(&session);
        protocol->setAccountsHandler(&handler);
        job.setModule(&handler);
        //WHEN
        QVERIFY(!job.exec());
        //THEN
        QCOMPARE(job.error(), int(SugarJob::CouldNotConnectError));
    }
};

QTEST_MAIN(TestCreateEntryJob)
#include "test_createentryjob.moc"
