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

        QTest::newRow("accounts") << "account" << 3;
        QTest::newRow("opportunities") << "opportunity" << 2;
        QTest::newRow("leads") << "lead" << 0;
    }

    void shouldCountEntriesCorrectly()
    {
        QFETCH(QString, type);
        QFETCH(int, amount);
        //GIVEN
        SugarMockProtocol protocol;
        //WHEN
        int entriesCount = 0;
        ListEntriesScope scope;
        QString query;
        QString errorMessage;
        protocol.getEntriesCount(scope, type, query, entriesCount, errorMessage);
        //THEN
        QCOMPARE(entriesCount, amount);
    }
};

QTEST_MAIN(TestSugarMockProtocol)
#include "test_sugarmockprotocol.moc"
