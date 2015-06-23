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

#include "qdateeditex.h"

#include <QtTest/QtTestGui>
#include <QDebug>
#include <QAbstractProxyModel>
#include <QSignalSpy>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>

class QDateEditExTest : public QObject
{
    Q_OBJECT
public:
private Q_SLOTS:
    void testSpecialValue()
    {
        QDateEdit w;
        w.setSpecialValueText(" ");
        w.setDate(w.minimumDate());
        QSignalSpy spy(&w, SIGNAL(dateChanged(QDate)));
        QVERIFY(spy.isValid());
        QCOMPARE(w.text(), QString(" "));
        QCOMPARE(spy.count(), 0);
        w.show();
        QCOMPARE(w.text(), QString(" "));
        QCOMPARE(spy.count(), 0);
    }

    void testSpecialValueInvalid()
    {
        QDateEdit w;
        w.setMinimumDate(QDate());
        //qDebug() << w.minimumDate(); // still 1752
        w.setSpecialValueText(" ");
        w.setDate(QDate());
        //QCOMPARE(w.text(), QString(" ")); // fails
    }

    void testNullDate()
    {
        QDateEditEx w;
        w.setNullable(true);
        w.setDate(QDate());
        QSignalSpy spy(&w, SIGNAL(dateChanged(QDate)));
        QVERIFY(spy.isValid());
        QCOMPARE(w.text(), QString(""));
        QCOMPARE(spy.count(), 0);
        w.show();
        QPushButton *clearButton = w.findChild<QPushButton *>();
        QVERIFY(clearButton);
        QVERIFY(!clearButton->isVisible());
        QCOMPARE(w.text(), QString(""));
        QCOMPARE(spy.count(), 0);
        QTest::qWait(500);
        QCOMPARE(w.text(), QString(""));
        QCOMPARE(spy.count(), 0);

        w.setDate(QDate::currentDate());
        QVERIFY(clearButton->isVisible());
    }

    void focusOutFromNullDateShouldNotShowDate()
    {
        QWidget top;
        QVBoxLayout *layout = new QVBoxLayout(&top);
        QDateEditEx *w = new QDateEditEx(&top);
        layout->addWidget(w);
        QLineEdit *other = new QLineEdit(&top);
        layout->addWidget(other);

        w->setNullable(true);
        w->setDate(QDate());
        QCOMPARE(w->text(), QString());
        w->setFocus();
        top.show();
        QCOMPARE(w->text(), QString());
        QTest::qWait(100);
        QCOMPARE(w->text(), QString());

        // click elsewhere
        other->setFocus();
        QCOMPARE(w->text(), QString());
        QTest::qWait(100);
        QCOMPARE(w->text(), QString());
    }

private:
};

QTEST_MAIN(QDateEditExTest)
#include "qdateeditextest.moc"
