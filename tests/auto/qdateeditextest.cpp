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
        QCOMPARE(w.text(), QString(" "));
        w.show();
        QCOMPARE(w.text(), QString(" "));
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
        QCOMPARE(w.text(), QString(""));
        w.show();
        QEXPECT_FAIL("", "Show puts the default date back in", Continue);
        QCOMPARE(w.text(), QString(""));
    }

private:
};

QTEST_MAIN(QDateEditExTest)
#include "qdateeditextest.moc"
