#include <QtTest/QtTestGui>
#include <QComboBox>
#include <referenceddatamodel.h>
#include <referenceddata.h>
#include <enums.h>
#include <QDebug>

class ReferencedDataTest : public QObject
{
    Q_OBJECT
public:
private Q_SLOTS:
    void testSortedCombo()
    {
        ReferencedData *assignedToRefData = ReferencedData::instance(AssignedToRef);
        assignedToRefData->setReferencedData("dfaure", "David Faure");
        assignedToRefData->setReferencedData("sfaure", "Sabine Faure");
        assignedToRefData->setReferencedData("afaure", "Adam Faure");
        assignedToRefData->setReferencedData("efaure", "Ernest Faure");
        // Add some duplicates
        assignedToRefData->setReferencedData("efaure", "Ernest Faure");
        assignedToRefData->setReferencedData("dfaure", "David Faure");

        QComboBox *combo = new QComboBox;
        ReferencedDataModel::setModelForCombo(combo, AssignedToRef);
        QCOMPARE(combo->count(), 1 + 4); // 1 for the empty item at the top
        QStringList items;
        for (int i = 0; i < combo->count(); ++i) {
            items.append(combo->itemText(i));
        }
        QCOMPARE(items, QStringList() << QString()
                 << "Adam Faure" << "David Faure" << "Ernest Faure" << "Sabine Faure");
    }
};

QTEST_MAIN(ReferencedDataTest)
#include "referenceddatatest.moc"
