/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: David Faure <david.faure@kdab.com>

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

#include "sugaraccount.h"
#include "sugaropportunity.h"
#include "linkeditemsrepository.h"
#include "collectionmanager.h"

#include <QTest>
class TestLinkedItemsRepository : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void shouldFindOpportunitiesForAccount()
    {
        //GIVEN
        LinkedItemsRepository repository(&m_collectionManager);
        SugarOpportunity opp;
        opp.setId("opp1");
        opp.setAccountId("acc1");
        //WHEN
        repository.addOpportunity(opp);
        //THEN
        QCOMPARE(repository.opportunitiesForAccount("acc1").count(), 1);
        QCOMPARE(repository.opportunitiesForAccount("non-existing").count(), 0);
    }

    void shouldClearCorrectly()
    {
        //GIVEN
        LinkedItemsRepository repository(&m_collectionManager);
        SugarOpportunity opp;
        opp.setId("opp1");
        opp.setAccountId("acc1");
        repository.addOpportunity(opp);
        //WHEN
        repository.clear();
        //THEN
        QCOMPARE(repository.opportunitiesForAccount("acc1").count(), 0);
    }

    void shouldDisappearAfterRemove()
    {
        //GIVEN
        LinkedItemsRepository repository(&m_collectionManager);
        SugarOpportunity opp;
        opp.setId("opp1");
        opp.setAccountId("acc1");
        repository.addOpportunity(opp);
        //WHEN
        repository.removeOpportunity(opp);
        //THEN
        QCOMPARE(repository.opportunitiesForAccount("acc1").count(), 0);

        // Check that removing a non-existing doesn't crash or something
        repository.removeOpportunity(opp);
        QCOMPARE(repository.opportunitiesForAccount("acc1").count(), 0);
    }

    void shouldUpdateOpportunity()
    {
        //GIVEN
        LinkedItemsRepository repository(&m_collectionManager);
        SugarOpportunity opp;
        opp.setId("opp1");
        opp.setAccountId("acc1");
        repository.addOpportunity(opp);
        opp.setAccountId("acc2");
        //WHEN
        repository.updateOpportunity(opp);
        //THEN
        QCOMPARE(repository.opportunitiesForAccount("acc1").count(), 0);
        QCOMPARE(repository.opportunitiesForAccount("acc2").count(), 1);
    }

private:
    CollectionManager m_collectionManager;
};

QTEST_MAIN(TestLinkedItemsRepository)
#include "test_linkeditemsrepository.moc"
