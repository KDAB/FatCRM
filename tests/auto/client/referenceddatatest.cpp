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

#include "referenceddata.h"
#include "referenceddatamodel.h"
#include "enums.h"

#include <QtTest/QtTestGui>
#include <QComboBox>
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
        auto *combo = new QComboBox;
        ReferencedDataModel::setModelForCombo(combo, AssignedToRef);
        auto *proxy = qobject_cast<QAbstractProxyModel *>(combo->model());
        QVERIFY(proxy);
        QAbstractItemModel *sourceModel = proxy->sourceModel();
        QCOMPARE(sourceModel->rowCount(), 1); // +1 for empty item at the top
        QCOMPARE(combo->count(), 1); // +1 for empty item at the top
        QSignalSpy spyRowsInserted(sourceModel, SIGNAL(rowsInserted(QModelIndex,int,int)));
        QSignalSpy spyRowsATBI(sourceModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
        QSignalSpy spyProxyRowsInserted(sourceModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

        // And then loading data into the model
        ReferencedData *assignedToRefData = ReferencedData::instance(AssignedToRef);
        assignedToRefData->setReferencedData(QStringLiteral("e88a49f9"), QStringLiteral("David Faure"));

        QCOMPARE(spyRowsATBI.count(), 1);
        QCOMPARE(spyRowsInserted.count(), 1);
        QCOMPARE(sourceModel->rowCount(), 2); // +1 for empty item at the top
        QCOMPARE(spyProxyRowsInserted.count(), 1);
        QCOMPARE(proxy->rowCount(), 2); // +1 for empty item at the top
        QCOMPARE(combo->count(), 2); // +1 for empty item at the top
        QCOMPARE(comboTexts(combo), QStringList() << QString() << "David Faure");

        assignedToRefData->setReferencedData(QStringLiteral("14fc9aa7"), QStringLiteral("Sabine Faure"));

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

        assignedToRefData->setReferencedData(QStringLiteral("3ab7e3b5"), QStringLiteral("Adam Faure"));
        assignedToRefData->setReferencedData(QStringLiteral("0cdd11df"), QStringLiteral("Ernest Faure"));
        assignedToRefData->setReferencedData(QStringLiteral("f0129ggs"), QStringLiteral("Charles Faure"));
        // Add some duplicates
        assignedToRefData->setReferencedData(QStringLiteral("0cdd11df"), QStringLiteral("Ernest Faure"));
        assignedToRefData->setReferencedData(QStringLiteral("e88a49f9"), QStringLiteral("David Faure"));

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
