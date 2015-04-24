#include <QtTest/QtTestGui>
#include <QComboBox>
#include <referenceddatamodel.h>
#include <referenceddata.h>
#include <enums.h>
#include <QDebug>
#include <QAbstractProxyModel>
#include <QSignalSpy>

class ReferencedDataTest : public QObject
{
    Q_OBJECT
public:
private Q_SLOTS:
    void testSortedCombo()
    {
        // Given GUI first
        QComboBox *combo = new QComboBox;
        ReferencedDataModel::setModelForCombo(combo, AssignedToRef);
        QAbstractProxyModel *proxy = qobject_cast<QAbstractProxyModel *>(combo->model());
        QVERIFY(proxy);
        QAbstractItemModel *sourceModel = proxy->sourceModel();
        QCOMPARE(sourceModel->rowCount(), 1); // +1 for empty item at the top
        QCOMPARE(combo->count(), 1); // +1 for empty item at the top
        QSignalSpy spyRowsInserted(sourceModel, SIGNAL(rowsInserted(QModelIndex, int, int)));
        QSignalSpy spyRowsATBI(sourceModel, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
        QSignalSpy spyProxyRowsInserted(sourceModel, SIGNAL(rowsInserted(QModelIndex, int, int)));

        // And then loading data into the model
        ReferencedData *assignedToRefData = ReferencedData::instance(AssignedToRef);
        assignedToRefData->setReferencedData("e88a49f9", "David Faure");

        QCOMPARE(spyRowsATBI.count(), 1);
        QCOMPARE(spyRowsInserted.count(), 1);
        QCOMPARE(sourceModel->rowCount(), 2); // +1 for empty item at the top
        QCOMPARE(spyProxyRowsInserted.count(), 1);
        QCOMPARE(proxy->rowCount(), 2); // +1 for empty item at the top
        QCOMPARE(combo->count(), 2); // +1 for empty item at the top
        QCOMPARE(comboTexts(combo), QStringList() << QString() << "David Faure");

        assignedToRefData->setReferencedData("14fc9aa7", "Sabine Faure");

        QCOMPARE(spyRowsATBI.count(), 2);
        QCOMPARE(spyRowsATBI.at(0).at(1).toInt(), 1);
        QCOMPARE(spyRowsInserted.count(), 2);
        QCOMPARE(spyRowsInserted.at(0).at(1).toInt(), 1);
        QCOMPARE(sourceModel->rowCount(), 3);
        QCOMPARE(modelTexts(sourceModel), QStringList() << QString() << "Sabine Faure" << "David Faure");
        QCOMPARE(spyProxyRowsInserted.count(), 2);
        QCOMPARE(proxy->rowCount(), 3);
        QCOMPARE(modelTexts(proxy), QStringList() << QString() << "David Faure" << "Sabine Faure");
        QCOMPARE(combo->count(), 3);
        QCOMPARE(comboTexts(combo), QStringList() << QString() << "David Faure" << "Sabine Faure");

        assignedToRefData->setReferencedData("3ab7e3b5", "Adam Faure");
        assignedToRefData->setReferencedData("0cdd11df", "Ernest Faure");
        assignedToRefData->setReferencedData("f0129ggs", "Charles Faure");
        // Add some duplicates
        assignedToRefData->setReferencedData("0cdd11df", "Ernest Faure");
        assignedToRefData->setReferencedData("e88a49f9", "David Faure");

        // The combo should show the 5 people, sorted
        QCOMPARE(spyRowsInserted.count(), 5);
        QCOMPARE(combo->count(), 1 + 5); // 1 for the empty item at the top
        qDebug() << comboTexts(combo);
        QCOMPARE(comboTexts(combo), QStringList() << QString()
                 << "Adam Faure" << "Charles Faure" << "David Faure" << "Ernest Faure" << "Sabine Faure");
    }

private:
    static QStringList comboTexts(QComboBox *combo) {
        QStringList items;
        for (int i = 0; i < combo->count(); ++i) {
            items.append(combo->itemText(i));
        }
        return items;
    }
    static QStringList modelTexts(QAbstractItemModel *model) {
        QStringList texts;
        for (int i = 0 ; i < model->rowCount() ; ++i )
            texts.append(model->index(i, 0).data().toString());
        return texts;
    }
};

QTEST_MAIN(ReferencedDataTest)
#include "referenceddatatest.moc"
