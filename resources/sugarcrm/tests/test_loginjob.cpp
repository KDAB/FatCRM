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
#include "loginjob.h"
#include "sugarsession.h"
#include "sugarmockprotocol.h"

class TestLoginJob : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void loginJobShouldGiveASessionId_data()
    {
        QTest::addColumn<QString>("username");
        QTest::addColumn<QString>("password");
        QTest::addColumn<QString>("expectedSessionId");

        QTest::newRow("correct") << "user" << "password" << "1";
        QTest::newRow("wrong_user") << "u" << "password" << QString();
        QTest::newRow("wrong_password") << "user" << "pass" << QString();
    }

    void loginJobShouldGiveASessionId()
    {
        QFETCH(QString, username);
        QFETCH(QString, password);
        QFETCH(QString, expectedSessionId);
        //GIVEN
        SugarSession session(nullptr);
        SugarProtocolBase *protocol = new SugarMockProtocol;
        session.setProtocol(protocol);
        session.setSessionParameters(username, password, "hosttest");
        LoginJob job(&session);
        //WHEN
        job.exec();
        //THEN
        QCOMPARE(session.sessionId(), expectedSessionId);
    }
};

QTEST_MAIN(TestLoginJob)
#include "test_loginjob.moc"
