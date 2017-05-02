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
#include "sugarsession.h"

Q_DECLARE_METATYPE(SugarSession::RequiredAction);

class TestSugarSession : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void shouldSetSessionParametersCorrectly()
    {
        //GIVEN
        SugarSession session(nullptr);
        //WHEN
        session.setSessionParameters("usertest", "passwordtest", "hosttest");
        //THEN
        QCOMPARE(session.userName(), QString("usertest"));
        QCOMPARE(session.password(), QString("passwordtest"));
        QCOMPARE(session.host(), QString("hosttest"));
    }

    void shouldReturnCorrectRequiredAction_data()
    {
        QTest::addColumn<QString>("username");
        QTest::addColumn<QString>("password");
        QTest::addColumn<QString>("host");
        QTest::addColumn<SugarSession::RequiredAction>("correctReturn");

        QTest::newRow("nochange") << "usertest" << "passwordtest" << "hosttest" << SugarSession::RequiredAction::None;
        QTest::newRow("newpassword") << "usertest" << "pwtest" << "hosttest" << SugarSession::RequiredAction::ReLogin;
        QTest::newRow("newuser") << "user" << "passwordtest" << "hosttest" << SugarSession::RequiredAction::NewLogin;
        QTest::newRow("newhost") << "usertest" << "passwordtest" << "host" << SugarSession::RequiredAction::NewLogin;
    }

    void shouldReturnCorrectRequiredAction()
    {
        //GIVEN
        QFETCH(QString, username);
        QFETCH(QString, password);
        QFETCH(QString, host);
        QFETCH(SugarSession::RequiredAction, correctReturn);
        SugarSession session(nullptr);
        session.setSessionParameters("usertest", "passwordtest", "hosttest");
        //WHEN
        SugarSession::RequiredAction rA = session.setSessionParameters(username, password, host);
        //THEN
        QCOMPARE(rA, correctReturn);

    }

    void shouldGetSessionIdCorrectly()
    {
        //GIVEN
        SugarSession session(nullptr);
        session.setSessionId("22");
        //WHEN
        const QString id = session.sessionId();
        //THEN
        QCOMPARE(id, QString("22"));
    }

    void shouldForgetSessionCorrectly()
    {
        //GIVEN
        SugarSession session(nullptr);
        session.setSessionId("28");
        //WHEN
        session.forgetSession();
        const QString id = session.sessionId();
        //THEN
        QCOMPARE(id, QString());
    }

};

QTEST_MAIN(TestSugarSession)
#include "test_sugarsession.moc"
