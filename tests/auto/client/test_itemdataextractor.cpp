/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Jeremy Entressangle <jeremy.entressangle@kdab.com>

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
#include <QSignalSpy>
#include <KContacts/Addressee>
#include <kcontacts_version.h>
#include "sugaraccount.h"
#include "sugaropportunity.h"
#include "sugarlead.h"
#include "sugarcampaign.h"
#include "itemdataextractor.h"

#if KContacts_VERSION < QT_VERSION_CHECK(5, 11, 42)
Q_DECLARE_METATYPE(KContacts::Address);
#endif

template<typename T>
static Akonadi::Item createAkonadiItemfromType()
{
    T payload;
    payload.setId("24");
    Akonadi::Item item;
    item.setPayload<T>(payload);
    return item;
}

template<>
Akonadi::Item createAkonadiItemfromType<KContacts::Addressee>()
{
    KContacts::Addressee contact;
    contact.insertCustom("FATCRM", "X-ContactId", "24");
    Akonadi::Item item;
    item.setPayload<KContacts::Addressee>(contact);
    return item;
}

class TestItemDataExtractor : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void shouldReturnCorrectUrlFromId_data()
    {
        QTest::addColumn<QString>("resourceBaseUrl");
        QTest::addColumn<QString>("itemId");
        QTest::addColumn<DetailsType>("type");
        QTest::addColumn<QUrl>("expectedUrl");

        QTest::newRow("simple") << "test" << "879" << DetailsType::Account << QUrl("test?action=DetailView&module=Accounts&record=879");
        static const char *sDetailType[] = {"Accounts", "Opportunities", "Leads", "Contacts", "Campaigns"};
        for(int i = 0; i <= int(DetailsType::Campaign); i++) {
               QTest::newRow(sDetailType[i]) << "KDAB" << "1"
                                             << static_cast<DetailsType>(i) << QUrl("KDAB?action=DetailView&module=" + QString(sDetailType[i]) + "&record=1");
        }
        QTest::newRow("without_resourceBaseUrl") << "" << "1" << DetailsType::Account << QUrl();
        QTest::newRow("without_itemId") << "test" << "" << DetailsType::Account << QUrl();
    }

    void shouldReturnCorrectUrlFromId()
    {
        //GIVEN
        QFETCH(QString, resourceBaseUrl);
        QFETCH(QString, itemId);
        QFETCH(DetailsType, type);
        QFETCH(QUrl, expectedUrl);
        auto itemDE = ItemDataExtractor::createDataExtractor(type);
        //WHEN
        const QUrl url = itemDE->itemUrl(resourceBaseUrl, itemId);
        //THEN
        QCOMPARE(url, expectedUrl);

    }

    void shouldReturnCorrectUrlFromAkonadiItem()
    {
        //GIVEN
        auto itemDE = ItemDataExtractor::createDataExtractor(DetailsType::Account);
        SugarAccount account;
        account.setId("1");
        Akonadi::Item item;
        item.setPayload<SugarAccount>(account);
        //WHEN
        const QUrl url = itemDE->itemUrl("KDAB", item);
        //THEN
        QCOMPARE(url, QUrl("KDAB?action=DetailView&module=Accounts&record=1"));
    }

    void shouldReturnCorrectId_data()
    {
        QTest::addColumn<DetailsType>("type");
        QTest::addColumn<Akonadi::Item>("item");

        QTest::newRow("account") << DetailsType::Account << createAkonadiItemfromType<SugarAccount>();
        QTest::newRow("opportunity") << DetailsType::Opportunity << createAkonadiItemfromType<SugarOpportunity>();
        QTest::newRow("lead") << DetailsType::Lead << createAkonadiItemfromType<SugarLead>();
        QTest::newRow("contact") << DetailsType::Contact << createAkonadiItemfromType<KContacts::Addressee>();
        QTest::newRow("campaign") << DetailsType::Campaign << createAkonadiItemfromType<SugarCampaign>();
    }

    void shouldReturnCorrectId()
    {
        //GIVEN
        QFETCH(DetailsType, type);
        QFETCH(Akonadi::Item, item);
        auto itemDE = ItemDataExtractor::createDataExtractor(type);
        //WHEN
        const QString id = itemDE->idForItem(item);
        //THEN
        QCOMPARE(id, QString("24"));
    }
};

QTEST_MAIN(TestItemDataExtractor)
#include "test_itemdataextractor.moc"
