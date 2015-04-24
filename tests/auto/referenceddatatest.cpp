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
        assignedToRefData->setReferencedData("e88a49f9", "David Faure");
        assignedToRefData->setReferencedData("14fc9aa7", "Sabine Faure");
        assignedToRefData->setReferencedData("3ab7e3b5", "Adam Faure");
        assignedToRefData->setReferencedData("6cdd11df", "Ernest Faure");
        // Add some duplicates
        assignedToRefData->setReferencedData("6cdd11df", "Ernest Faure");
        assignedToRefData->setReferencedData("e88a49f9", "David Faure");

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
