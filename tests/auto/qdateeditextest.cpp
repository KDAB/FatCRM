#include <QtTest/QtTestGui>
#include <qdateeditex.h>
#include <QDebug>
#include <QAbstractProxyModel>
#include <QSignalSpy>

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
        QCOMPARE(w.text(), QString(""));
        QCOMPARE(spy.count(), 0);
        QTest::qWait(50);
        QCOMPARE(spy.count(), 0);
    }

private:
};

QTEST_MAIN(QDateEditExTest)
#include "qdateeditextest.moc"
