/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

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

#include <QtTest/QtTest>
#include <QDebug>
#include "enumdefinitions.h"
#include <QSignalSpy>

class TestEnumDefinitions : public QObject
{
    Q_OBJECT
public:
private Q_SLOTS:

    void testSerialization_data()
    {
        QTest::addColumn<EnumDefinitions>("enums");

        QTest::newRow("empty") << EnumDefinitions();
        QTest::newRow("emptyEnum") << (EnumDefinitions() << EnumDefinitions::Enum("e1"));
        QTest::newRow("emptyEnums") << (EnumDefinitions() << EnumDefinitions::Enum("e1") << EnumDefinitions::Enum("e2"));

        EnumDefinitions::Enum leadSource("lead_source");
        leadSource.mEnumValues.insert("key", "value");
        QTest::newRow("oneValue") << (EnumDefinitions() << leadSource);
        leadSource.mEnumValues.insert("key2", "value 2");
        QTest::newRow("twoValues") << (EnumDefinitions() << leadSource);

        EnumDefinitions::Enum salutation("salutation");
        salutation.mEnumValues.insert("another", "value");
        QTest::newRow("twoEnums") << (EnumDefinitions() << leadSource << salutation);
    }

    void testSerialization()
    {
        QFETCH(EnumDefinitions, enums);

        const QString str = enums.toString();
        EnumDefinitions reloaded = EnumDefinitions::fromString(str);
        QCOMPARE(reloaded.toString(), str);
        QCOMPARE(reloaded.count(), enums.count());

        if (reloaded.count() > 0) {
            QCOMPARE(reloaded.indexOf("lead_source"), 0);
        }
        if (reloaded.count() > 1) {
            QCOMPARE(reloaded.indexOf("salutation"), 1);
        }
    }

};

QTEST_MAIN(TestEnumDefinitions)
#include "test_enumdefinitions.moc"
