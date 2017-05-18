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
#include <QSettings>
#include <Akonadi/Item>
#include "sugaraccount.h"
#include "sugarsession.h"
#include "sugarsoapprotocol.h"
#include "createentryjob.h"
#include "updateentryjob.h"
#include "deleteentryjob.h"
#include "fetchentryjob.h"
#include "accountshandler.h"
#include "opportunitieshandler.h"

class TestJobWithSugarMockProtocol : public QObject
{
    Q_OBJECT

public:
    TestJobWithSugarMockProtocol()
        : mSession(nullptr)
    {
    }

private:
    SugarSession mSession;
    SugarAccount mNewAccount;
    SugarOpportunity mNewOpp;
    QString mRemoteRevision;

private Q_SLOTS:

    void initTestCase()
    {
        qApp->setOrganizationName("KDAB");
        QSettings settings;
        settings.beginGroup("SugarAuthentication");
        QString userName = settings.value("username", QString()).toString();
        QString mPassword = settings.value("password", QString()).toString();
        QString mHost = settings.value("host", QString()).toString();

        if (userName.isEmpty() || mPassword.isEmpty() || mHost.isEmpty()) { // Create a template file
            if (userName.isEmpty()) {
                settings.setValue("username", QString());
            }
            if (mPassword.isEmpty()) {
                settings.setValue("password", QString());
            }
            if (mHost.isEmpty()) {
                settings.setValue("host", QString());
            }
            QSKIP(qPrintable(QString("Missing authentication information for a test SugarCRM server, please edit %1 to write your username, password, and host. Do NOT use a production server!").arg(settings.fileName())),SkipAll);
        }
        mSession.setSessionParameters(userName, mPassword, mHost);
        mSession.createSoapInterface();
    }

    void shouldAddAccount()
    {
        //GIVEN
        SugarAccount account;
        account.setName("newAccount");
        Akonadi::Item item;
        item.setId(0);
        item.setPayload<SugarAccount>(account);
        SugarSoapProtocol *protocol = new SugarSoapProtocol;
        mSession.setProtocol(protocol);
        protocol->setSession(&mSession);
        CreateEntryJob job(item, &mSession);
        AccountsHandler handler(&mSession);
        job.setModule(&handler);
        //WHEN
        QVERIFY(job.exec());
        mNewAccount = job.item().payload<SugarAccount>();
        mRemoteRevision = job.item().remoteRevision();
    }

    void shouldFetchAccount()
    {
        //GIVEN
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewAccount.id());
        SugarSoapProtocol *protocol = new SugarSoapProtocol;
        mSession.setProtocol(protocol);
        protocol->setSession(&mSession);
        AccountsHandler handler(&mSession);
        FetchEntryJob job(item, &mSession);
        job.setModule(&handler);
        //WHEN
        QVERIFY(job.exec());
        //THEN
        SugarAccount foundAccount = job.item().payload<SugarAccount>();
        QCOMPARE(foundAccount.id(), mNewAccount.id());
        QCOMPARE(foundAccount.name(), mNewAccount.name());
    }

    void shouldUpdateAccount()
    {
        //GIVEN
        SugarAccount account;
        account.setName("updateAccount");
        account.setId(mNewAccount.id());
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewAccount.id());
        item.setRemoteRevision(mRemoteRevision);
        item.setPayload<SugarAccount>(account);
        SugarSoapProtocol *protocol = new SugarSoapProtocol;
        mSession.setProtocol(protocol);
        protocol->setSession(&mSession);
        UpdateEntryJob job(item, &mSession);
        AccountsHandler handler(&mSession);
        job.setModule(&handler);
        //WHEN
        QVERIFY(job.exec());
        //THEN
        SugarAccount accountUpdate = job.item().payload<SugarAccount>();
        QCOMPARE(accountUpdate.name(), QString("updateAccount"));
        QCOMPARE(accountUpdate.id(), mNewAccount.id());
    }

    void shouldDeleteAccount()
    {
        //GIVEN
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewAccount.id());
        SugarSoapProtocol *protocol = new SugarSoapProtocol;
        mSession.setProtocol(protocol);
        protocol->setSession(&mSession);
        DeleteEntryJob job(item, &mSession, Module::Accounts);
        //WHEN
        job.exec(); // Triggers a soap error but the account is removed, the fetch after shows that.
    }

    void shouldNotFetchAccount()
    {
        //GIVEN
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewAccount.id());
        SugarSoapProtocol *protocol = new SugarSoapProtocol;
        mSession.setProtocol(protocol);
        protocol->setSession(&mSession);
        AccountsHandler handler(&mSession);
        FetchEntryJob job(item, &mSession);
        job.setModule(&handler);
        //WHEN
        QVERIFY(job.exec());
        //THEN
        SugarAccount accountFind = job.item().payload<SugarAccount>();
        QCOMPARE(accountFind.id(), mNewAccount.id());
        QCOMPARE(accountFind.name(), QString());
    }

    void shouldCreateOpportunity()
    {
        //GIVEN
        SugarOpportunity opp;
        opp.setName("newOpportunity");
        Akonadi::Item item;
        item.setId(0);
        item.setPayload<SugarOpportunity>(opp);
        SugarSoapProtocol *protocol = new SugarSoapProtocol;
        mSession.setProtocol(protocol);
        protocol->setSession(&mSession);
        CreateEntryJob job(item, &mSession);
        OpportunitiesHandler handler(&mSession);
        job.setModule(&handler);
        //WHEN
        QVERIFY(job.exec());
        mNewOpp = job.item().payload<SugarOpportunity>();
        mRemoteRevision = job.item().remoteRevision();
    }

    void shouldFetchOpportunity()
    {
        //GIVEN
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewOpp.id());
        SugarSoapProtocol *protocol = new SugarSoapProtocol;
        mSession.setProtocol(protocol);
        protocol->setSession(&mSession);
        OpportunitiesHandler handler(&mSession);
        FetchEntryJob job(item, &mSession);
        job.setModule(&handler);
        //WHEN
        QVERIFY(job.exec());
        //THEN
        SugarOpportunity oppFind = job.item().payload<SugarOpportunity>();
        QCOMPARE(oppFind.id(), mNewOpp.id());
        QCOMPARE(oppFind.name(), mNewOpp.name());
    }

    void shouldUpdateOpportunity()
    {
        //GIVEN
        SugarOpportunity opp;
        opp.setName("updateOpportunity");
        opp.setId(mNewOpp.id());
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewOpp.id());
        item.setRemoteRevision(mRemoteRevision);
        item.setPayload<SugarOpportunity>(opp);
        SugarSoapProtocol *protocol = new SugarSoapProtocol;
        mSession.setProtocol(protocol);
        protocol->setSession(&mSession);
        UpdateEntryJob job(item, &mSession);
        OpportunitiesHandler handler(&mSession);
        job.setModule(&handler);
        //WHEN
        QVERIFY(job.exec());
        //THEN
        SugarOpportunity oppUpdate = job.item().payload<SugarOpportunity>();
        QCOMPARE(oppUpdate.name(), QString("updateOpportunity"));
        QCOMPARE(oppUpdate.id(), mNewOpp.id());
    }

    void shouldDeleteOpportunity()
    {
        //GIVEN
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewOpp.id());
        SugarSoapProtocol *protocol = new SugarSoapProtocol;
        mSession.setProtocol(protocol);
        protocol->setSession(&mSession);
        DeleteEntryJob job(item, &mSession, Module::Opportunities);
        //WHEN
        job.exec(); // Triggers a soap error but the opportunity is removed, the fetch after shows that.
    }

    void shouldNotFetchOpportunity()
    {
        //GIVEN
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewOpp.id());
        SugarSoapProtocol *protocol = new SugarSoapProtocol;
        mSession.setProtocol(protocol);
        protocol->setSession(&mSession);
        OpportunitiesHandler handler(&mSession);
        FetchEntryJob job(item, &mSession);
        job.setModule(&handler);
        //WHEN
        QVERIFY(job.exec());
        //THEN
        SugarOpportunity oppFind = job.item().payload<SugarOpportunity>();
        QCOMPARE(oppFind.id(), mNewOpp.id());
        QCOMPARE(oppFind.name(), QString());
    }
};

QTEST_MAIN(TestJobWithSugarMockProtocol)
#include "test_jobwithsugarmockprotocol.moc"
