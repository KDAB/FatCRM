/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "kdcrmutils.h"

#include <QTest>
#include <QDebug>

class KDCRMUtilsTest : public QObject
{
    Q_OBJECT
public:
private Q_SLOTS:
    void testIncrementTimestamp_data()
    {
        QTest::addColumn<QString>("input");
        QTest::addColumn<QString>("output");

        QTest::newRow("28") << "2015-06-26 21:39:28" << "2015-06-26 21:39:29";
        QTest::newRow("59") << "2015-06-26 21:39:59" << "2015-06-26 21:40:00";
        QTest::newRow("midnight") << "2015-06-26 23:59:59" << "2015-06-27 00:00:00";
        QTest::newRow("empty") << "" << "";
    }

    void testIncrementTimestamp()
    {
        QFETCH(QString, input);
        QFETCH(QString, output);

        QString str = input;
        KDCRMUtils::incrementTimeStamp(str);
        QCOMPARE(str, output);
        KDCRMUtils::decrementTimeStamp(str);
        QCOMPARE(str, input);
    }
};

QTEST_MAIN(KDCRMUtilsTest)
#include "kdcrmutilstest.moc"
