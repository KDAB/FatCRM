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
#include "sugarmockprotocol.h"
#include "sugarjob.h"
#include "listentriesscope.h"
#include "accountshandler.h"
#include "opportunitieshandler.h"
#include "campaignshandler.h"
#include "leadshandler.h"
#include "contactshandler.h"

#include "sugarsession.h"

class TestSugarMockProtocol : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void shouldLoginCorrectly_data()
    {
        QTest::addColumn<QString>("username");
        QTest::addColumn<QString>("password");
        QTest::addColumn<int>("correctError");

        QTest::newRow("correct") << "user" << "password" << 0;
        QTest::newRow("wrong_user") << "u" << "password" << static_cast<int>(SugarJob::Errors::LoginError);
        QTest::newRow("wrong_password") << "user" << "pass" << static_cast<int>(SugarJob::Errors::LoginError);
    }

    void shouldLoginCorrectly()
    {
        //GIVEN
        QFETCH(QString, username);
        QFETCH(QString, password);
        QFETCH(int, correctError);
        SugarMockProtocol protocol;
        //WHEN
        QString sessionId, errorMessage;
        int error = protocol.login(username, password, sessionId, errorMessage);
        //THEN
        QCOMPARE(error, correctError);
        if (error == KJob::NoError) {
            QCOMPARE(sessionId, QString("1"));
        }
    }

    void serverNotFound()
    {
        //GIVEN
        SugarMockProtocol protocol;
        protocol.setServerNotFound(true);
        //WHEN
        QString sessionId, errorMessage;
        int error = protocol.login("user", "password", sessionId, errorMessage);
        //THEN
        QCOMPARE(error, static_cast<int>(SugarJob::Errors::CouldNotConnectError));
    }

    void shouldCountEntriesCorrectly_data()
    {
        QTest::addColumn<QString>("type");
        QTest::addColumn<int>("amount");

        QTest::newRow("accounts") << "Accounts" << 3;
        QTest::newRow("opportunities") << "Opportunities" << 2;
        QTest::newRow("leads") << "Leads" << 1;
        QTest::newRow("campaign") << "Campaigns" << 1;
        QTest::newRow("contact") << "Contacts" << 1;
    }

    void shouldCountEntriesCorrectly()
    {
        QFETCH(QString, type);
        QFETCH(int, amount);
        //GIVEN
        SugarMockProtocol protocol;
        protocol.addData();
        //WHEN
        int entriesCount = 0;
        ListEntriesScope scope;
        QString query;
        QString errorMessage;
        protocol.getEntriesCount(scope, type, query, entriesCount, errorMessage);
        //THEN
        QCOMPARE(entriesCount, amount);
    }

    void ListEntriesWork_data()
    {
        QTest::addColumn<QString>("moduleName");
        QTest::addColumn<int>("amount");

        QTest::newRow("accounts") << "Accounts" << 3;
        QTest::newRow("opportunities") << "Opportunities" << 2;
        QTest::newRow("leads") << "Leads" << 1;
        QTest::newRow("campaigns") << "Campaigns" << 1;
        QTest::newRow("contacts") << "Contacts" << 1;
    }

    void ListEntriesWork()
    {
        QFETCH(QString, moduleName);
        QFETCH(int, amount);
        //GIVEN
        SugarSession session(nullptr);
        SugarMockProtocol protocol;
        protocol.addData();
        AccountsHandler accountsHandler(&session);
        protocol.setAccountsHandler(&accountsHandler);
        OpportunitiesHandler opportunitiesHandler(&session);
        protocol.setOpportunitiesHandler(&opportunitiesHandler);
        CampaignsHandler campaignHandler(&session);
        protocol.setCampaignsHandler(&campaignHandler);
        LeadsHandler leadsHandler(&session);
        protocol.setLeadsHandler(&leadsHandler);
        ContactsHandler contactHandler(&session);
        protocol.setContactsHandler(&contactHandler);

        //WHEN
        ListEntriesScope scope;
        EntriesListResult result;
        QStringList selectedFields;
        QString query, orderBy, errorMessage;
        protocol.listEntries(scope, moduleName, query, orderBy, selectedFields, result, errorMessage);
        //THEN
        QCOMPARE(result.resultCount, amount);
        QCOMPARE(result.entryList.items().size(), amount);
    }
};

QTEST_MAIN(TestSugarMockProtocol)
#include "test_sugarmockprotocol.moc"
