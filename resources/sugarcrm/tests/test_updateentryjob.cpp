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

#include "accountshandler.h"
#include "opportunitieshandler.h"
#include "sugaraccount.h"
#include "sugarmockprotocol.h"
#include "sugarsession.h"
#include "updateentryjob.h"
#include "kdcrmdata/kdcrmutils.h"

#include <AkonadiCore/Item>

#include <QDebug>
#include <QSignalSpy>

#include <QTest>

class TestUpdateEntryJob : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void shouldUpdateAccount()
    {
        //GIVEN
        auto *protocol = new SugarMockProtocol;
        SugarSession session(nullptr);
        session.setSessionParameters("user", "password", "hosttest");
        protocol->addData();
        session.setProtocol(protocol);
        protocol->setSession(&session);
        SugarAccount account;
        account.setName("updateAccount");
        account.setId("1");
        account.setDateModified(QDateTime::currentDateTime());
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId("1");
        item.setPayload<SugarAccount>(account);
        item.setRemoteRevision(account.dateModifiedRaw());
        UpdateEntryJob job(item, &session);
        AccountsHandler handler(&session);
        job.setModule(&handler);
        //WHEN
        QVERIFY(job.exec());
        //THEN
        SugarAccount newAccount = protocol->accounts().at(1);
        QCOMPARE(newAccount.name(), QString("updateAccount"));
        QCOMPARE(newAccount.id(), QString("1"));
        SugarAccount newAccountFromJob = job.item().payload<SugarAccount>();
        QCOMPARE(newAccountFromJob.name(), QString("updateAccount"));
        QCOMPARE(newAccountFromJob.id(), QString("1"));
    }

    void shouldUpdateOpportunity()
    {
        //GIVEN
        auto *protocol = new SugarMockProtocol;
        SugarSession session(nullptr);
        session.setSessionParameters("user", "password", "hosttest");
        protocol->addData();
        session.setProtocol(protocol);
        protocol->setSession(&session);
        SugarOpportunity opp;
        opp.setName("updateOpp");
        opp.setId("100");
        opp.setDateModified(QDateTime::currentDateTime());
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId("100");
        item.setPayload<SugarOpportunity>(opp);
        item.setRemoteRevision(opp.dateModifiedRaw());
        UpdateEntryJob job(item, &session);
        OpportunitiesHandler handler(&session);
        job.setModule(&handler);
        //WHEN
        QVERIFY(job.exec());
        //THEN
        SugarOpportunity newOpp = protocol->opportunities().at(0);
        QCOMPARE(newOpp.name(), QString("updateOpp"));
        QCOMPARE(newOpp.id(), QString("100"));
        SugarOpportunity newOppFromJob = job.item().payload<SugarOpportunity>();
        QCOMPARE(newOppFromJob.name(), QString("updateOpp"));
        QCOMPARE(newOppFromJob.id(), QString("100"));
    }

    void shouldHandleInvalidContextError()
    {
        //GIVEN
        SugarAccount account;
        account.setName("updateAccount");
        account.setId("1");
        Akonadi::Item item;
        item.setId(0);
        item.setPayload<SugarAccount>(account);
        auto *protocol = new SugarMockProtocol;
        SugarSession session(nullptr);
        session.setSessionParameters("user", "password", "hosttest");
        protocol->addData();
        session.setProtocol(protocol);
        protocol->setSession(&session);
        UpdateEntryJob job(item, &session);
        AccountsHandler handler(&session);
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
        account.setName("updateAccount");
        account.setId("1");
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId("1");
        item.setPayload<SugarAccount>(account);
        auto *protocol = new SugarMockProtocol;
        protocol->setServerNotFound(true);
        SugarSession session(nullptr);
        session.setSessionParameters("user", "password", "hosttest");
        protocol->addData();
        session.setProtocol(protocol);
        protocol->setSession(&session);
        UpdateEntryJob job(item, &session);
        AccountsHandler handler(&session);
        job.setModule(&handler);
        //WHEN
        QVERIFY(!job.exec());
        //THEN
        QCOMPARE(job.error(), int(SugarJob::CouldNotConnectError));
    }
};

QTEST_MAIN(TestUpdateEntryJob)
#include "test_updateentryjob.moc"
