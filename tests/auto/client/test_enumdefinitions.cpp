/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QTest>
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
        QTest::addColumn<QString>("expectedIndexOfString");
        QTest::addColumn<int>("expectedIndexOfValue");

        QTest::newRow("empty") << EnumDefinitions() << "" << -1;
        QTest::newRow("emptyEnum") << (EnumDefinitions() << EnumDefinitions::Enum(QStringLiteral("e1"))) << "" << -1;
        QTest::newRow("emptyEnums") << (EnumDefinitions() << EnumDefinitions::Enum(QStringLiteral("e1")) << EnumDefinitions::Enum(QStringLiteral("e2"))) << "" << -1;

        EnumDefinitions::Enum leadSource(QStringLiteral("lead_source"));
        EnumDefinitions::KeyValue kv1 = {"key", "value"};
        leadSource.mEnumValues.append(kv1);
        QTest::newRow("oneValue") << (EnumDefinitions() << leadSource) << "lead_source" << 0;
        EnumDefinitions::KeyValue kv2 = {"key2", "value 2"};
        leadSource.mEnumValues.append(kv2);
        QTest::newRow("twoValues") << (EnumDefinitions() << leadSource) << "lead_source" << 0;

        EnumDefinitions::Enum salutation(QStringLiteral("salutation"));
        EnumDefinitions::KeyValue kv3 = {"another", "value"};
        salutation.mEnumValues.append(kv3);
        QTest::newRow("twoEnums") << (EnumDefinitions() << leadSource << salutation) << "salutation" << 1;
    }

    void testSerialization()
    {
        QFETCH(EnumDefinitions, enums);
        QFETCH(QString, expectedIndexOfString);
        QFETCH(int, expectedIndexOfValue);

        const QString str = enums.toString();
        EnumDefinitions reloaded = EnumDefinitions::fromString(str);
        QCOMPARE(reloaded.toString(), str);
        QCOMPARE(reloaded.count(), enums.count());

        if (expectedIndexOfValue > -1) {
            QCOMPARE(reloaded.indexOf(expectedIndexOfString), expectedIndexOfValue);
        }
    }

};

QTEST_MAIN(TestEnumDefinitions)
#include "test_enumdefinitions.moc"
