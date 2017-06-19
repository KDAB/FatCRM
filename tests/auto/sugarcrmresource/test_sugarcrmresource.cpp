/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QDBusInterface>
#include <AkonadiCore/Item>
#include <AkonadiCore/Collection>
#include <AkonadiCore/Job>
#include <AkonadiCore/CollectionFetchJob>
#include <AkonadiCore/CollectionFetchScope>
#include <AkonadiCore/ItemFetchJob>
#include <AkonadiCore/ItemFetchScope>
#include <AkonadiCore/ItemCreateJob>
#include <AkonadiCore/ItemModifyJob>
#include <AkonadiCore/ItemDeleteJob>
#include <qtest_akonadi.h>
#include <AkonadiCore/servermanager.h>
#include <AkonadiCore/AttributeFactory>
#include <AkonadiCore/EntityHiddenAttribute>
#include <AkonadiCore/Monitor>
#include <QDBusReply>
#include <sugaraccount.h>
#include <sugaropportunity.h>
#include <unistd.h>

using namespace Akonadi;

static const char s_dbusInterfaceName[] = "com.kdab.SugarCRM.Mock";
static const char s_dbusObjectName[] = "/CRMMock";

static void configureItemFetchScope(Akonadi::ItemFetchScope &scope)
{
    scope.setFetchRemoteIdentification(false);
    scope.setIgnoreRetrievalErrors(true);
    scope.fetchFullPayload(true);
}

struct ItemData {
    QString name;
    QString id;
    ItemData(const QString &_name, const QString &_id) : name(_name), id(_id) { }
    bool operator==(const ItemData &other) const { return name == other.name && id == other.id; }
    bool operator<(const ItemData &other) const { return id < other.id; }
};

QString serviceName()
{
    QString serviceName = "org.freedesktop.Akonadi.Agent.akonadi_sugarcrm_resource_0.testrunner-";
    serviceName += QString::fromLocal8Bit(qgetenv("AKONADI_TESTRUNNER_PID"));
    return serviceName;
}

class TestSugarCRMResource : public QObject
{
    Q_OBJECT

private:
    Collection mAccountsCollection;
    Collection mOpportunitiesCollection;

    Item::Id mNewId;

    template<typename T>
    static void fetchItems(Collection &col, QList<ItemData> &result)
    {
        Q_ASSERT(result.isEmpty());
        ItemFetchJob *job = new ItemFetchJob(col);
        job->fetchScope().fetchFullPayload();
        AKVERIFYEXEC(job);
        for (const Item &item : job->items()) {
            QVERIFY(item.hasPayload<T>());
            T sugarItem = item.payload<T>();
            result << ItemData(sugarItem.name(), sugarItem.id());
        }
    }

    static void compareItemDataList(QList<ItemData> &&result, QList<ItemData> &&expected)
    {
        QCOMPARE(result.size(), expected.size());
        std::sort(expected.begin(), expected.end());
        std::sort(result.begin(), result.end());
        for (int i=0; i<result.count(); ++i) {
            QCOMPARE(result.at(i), expected.at(i));
        }
    }

    template<typename T>
    static void fetchAndCompareItems(Collection &col, QList<ItemData> &&expected)
    {
        QList<ItemData> result;
        fetchItems<T>(col, result);
        compareItemDataList(std::move(result), std::move(expected));
    }

    template<typename T>
    void compareToExpectedResult(const QList<QVariant> &arguments, const QString &expectedName, const QString &expectedId)
    {
        const Item item = arguments.at(0).value<Item>();
        QVERIFY(item.hasPayload<T>());
        T inst = item.payload<T>();

        qDebug() << "name:" << inst.name() << "id:" << inst.id();
        QCOMPARE(inst.name(), expectedName);
        QCOMPARE(inst.id(), expectedId);
    }

    void monitorInit(Monitor &monitor, const Collection &col)
    {
        monitor.setCollectionMonitored(col);
        configureItemFetchScope(monitor.itemFetchScope());
    }

    void externalOperation(Collection &collection, const QString &opperationName, const QString &name, const QString &id,
                           const char * signal, QList<QVariant> &arguments)
    {
        Monitor monitor;
        monitorInit(monitor, collection);
        QDBusInterface mock(serviceName(), s_dbusObjectName, s_dbusInterfaceName);
        QDBusReply<void> reply;
        if (name.isEmpty()) {
            reply = mock.call(opperationName, id);
        } else {
            reply = mock.call(opperationName, name, id);
        }
        QVERIFY2(reply.isValid(), reply.error().message().toLatin1());
        AgentManager::self()->synchronizeCollection(collection);

        QSignalSpy spy(&monitor, signal);
        QVERIFY(spy.wait());
        QCOMPARE(spy.count(), 1);
        arguments = spy.at(0);
    }

    template<typename T>
    void createSugarItem(const QString &name, const QString &id, Collection &collection, QList<ItemData> &&expected, const QString &operationName)
    {
        //GIVEN
        T sugarItem;
        sugarItem.setName(name);
        Item item;
        item.setPayload(sugarItem);
        item.setMimeType(T::mimeType());
        ItemCreateJob *job = new ItemCreateJob(item, collection);
        //WHEN
        AKVERIFYEXEC(job);
        //THEN
        Monitor monitor;
        monitorInit(monitor, collection);
        QSignalSpy spyAdded(&monitor, SIGNAL(itemAdded(Akonadi::Item,Akonadi::Collection)));
        QSignalSpy spyChanged(&monitor, SIGNAL(itemChanged(Akonadi::Item,QSet<QByteArray>)));

        QVERIFY(spyAdded.wait());
        QVERIFY(spyChanged.wait());

//        for (int i = 0; i < spyAdded.count(); i++) {
//            const QList<QVariant> arguments = spyAdded.at(i);
//            const Item item = arguments.at(0).value<Item>();
//            T inst = item.payload<T>();
//            qDebug() << "TEST ////" << "added" << i << "name:" << inst.name() << "id:" << inst.id() << "remoteId:" << item.remoteId();
//        }
//        for (int i = 0; i < spyChanged.count(); i++) {
//            const QList<QVariant> arguments = spyChanged.at(i);
//            const Item item = arguments.at(0).value<Item>();
//            T inst = item.payload<T>();
//            qDebug() << "TEST ////" << "changed" << i << "name:" << inst.name() << "id:" << inst.id() << "remoteId:" << item.remoteId();
//        }

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        QCOMPARE(spyAdded.count(), 1);

        const QList<QVariant> arguments = spyAdded.at(0);
        compareToExpectedResult<T>(arguments, name, "");

        QTRY_COMPARE(spyChanged.count(), 2);


        const QList<QVariant> argumentsUpdate = spyChanged.at(spyChanged.count()-1);
        compareToExpectedResult<T>(argumentsUpdate, name, id);
#endif
        fetchAndCompareItems<T>(collection, std::move(expected));

        mNewId = spyChanged.at(0).at(0).value<Item>().id();
        QVERIFY(mNewId != 0);

        QDBusInterface mock(serviceName(), s_dbusObjectName, s_dbusInterfaceName);
        QDBusReply<bool> reply = mock.call(operationName, name, id);
        QVERIFY2(reply.value(), qPrintable(QString(operationName + " failed")));
    }

    template<typename T>
    void updateSugarItem(const QString &name, const QString &id, Collection &collection, QList<ItemData> &&expected, const QString &operationName)
    {
        //GIVEN
        T sugarItem;
        sugarItem.setName(name);
        sugarItem.setId(id);
        Item item;
        item.setId(mNewId);
        item.setRemoteId(id);
        item.setPayload(sugarItem);
        item.setMimeType(T::mimeType());
        ItemModifyJob *job = new ItemModifyJob(item);
        //WHEN
        AKVERIFYEXEC(job);
        //THEN
        Monitor monitor;
        monitorInit(monitor, collection);
        QSignalSpy spy(&monitor, SIGNAL(itemChanged(Akonadi::Item,QSet<QByteArray>)));
        QVERIFY(spy.wait());
#if 0
        QTest::qWait(2000);
        for (int i = 0; i < spy.count(); i++) {
            const QList<QVariant> arguments = spy.at(i);
            const Item item = arguments.at(0).value<Item>();
            T inst = item.payload<T>();
            qDebug() << "UPDATE ////" << "changed" << i << "name:" << inst.name() << "id:" << inst.id() << "remoteId:" << item.remoteId();
        }
#endif

        const QList<QVariant> arguments = spy.at(0);
        compareToExpectedResult<T>(arguments, name, id);

        fetchAndCompareItems<T>(collection, std::move(expected));

        QDBusInterface mock(serviceName(), s_dbusObjectName, s_dbusInterfaceName);
        QDBusReply<bool> reply;
        reply = mock.call(operationName, name, id);
        QVERIFY(reply.value());

        QVERIFY2(reply.isValid(), reply.error().message().toLatin1());
    }

    template<typename T>
    void deleteSugarItem(const QString &id, Collection &collection, QList<ItemData> &&expected)
    {
        //GIVEN
        Item item;
        item.setId(mNewId);
        item.setRemoteId(id);
        ItemDeleteJob *job = new ItemDeleteJob(item);
        //WHEN
        AKVERIFYEXEC(job);
        //THEN
        Monitor monitor;
        monitor.setCollectionMonitored(collection);
        configureItemFetchScope(monitor.itemFetchScope());
        QSignalSpy spyRemoved(&monitor, SIGNAL(itemRemoved(Akonadi::Item)));
        QVERIFY(spyRemoved.wait());

        fetchAndCompareItems<T>(collection, std::move(expected));
    }

private Q_SLOTS:

    void initTestCase()
    {
        AkonadiTest::checkTestIsIsolated();
        qRegisterMetaType<Akonadi::Item>();
        qRegisterMetaType<Akonadi::Collection>();
    }

    void shouldFetchCollection()
    {
        //GIVEN
        CollectionFetchJob *fetchJob = new CollectionFetchJob(Collection::root(), CollectionFetchJob::Recursive);
        fetchJob->fetchScope().setResource("akonadi_sugarcrm_resource_0");
        //WHEN
        AKVERIFYEXEC(fetchJob);
        QStringList expected{{"Accounts"}, {"Opportunities"}, {"Contacts"}, {"Documents"}, {"Emails"}, {"akonadi_sugarcrm_resource_0"}};
        //THEN
        QCOMPARE(fetchJob->collections().size(), 6);
        for (const Collection &col : fetchJob->collections()) {
            QVERIFY(expected.contains(col.name()));
            QString name = col.name();
            if (name == "Accounts") {
                mAccountsCollection = col;
            } else if(name == "Opportunities") {
                mOpportunitiesCollection = col;
            }
        }
    }

    //ACCOUNTS

    void shouldListAccounts()
    {
        QList<ItemData> expected{{"accountZero", "0"}, {"accountOne", "1"}, {"accountTwo", "2"}};
        fetchAndCompareItems<SugarAccount>(mAccountsCollection, std::move(expected));
    }

    void shouldCorrectlySynchronizeExternalAccountCreation()
    {
        QList<QVariant> arguments;
        externalOperation(mAccountsCollection, "addAccount", "nameTest", "idTest", SIGNAL(itemAdded(Akonadi::Item,Akonadi::Collection)), arguments);

        QList<ItemData> expected{{"accountZero", "0"}, {"accountOne", "1"}, {"accountTwo", "2"}, {"nameTest", "idTest"}};
        fetchAndCompareItems<SugarAccount>(mAccountsCollection, std::move(expected));

        compareToExpectedResult<SugarAccount>(arguments, "nameTest", "idTest");
    }

    void shouldCorrectlySynchronizeExternalAccountUpdate()
    {        
        QList<QVariant> arguments;
        externalOperation(mAccountsCollection, "updateAccount", "nameTest2", "idTest", SIGNAL(itemChanged(Akonadi::Item,QSet<QByteArray>)), arguments);

        QList<ItemData> expected{{"accountZero", "0"}, {"accountOne", "1"}, {"accountTwo", "2"}, {"nameTest2", "idTest"}};
        fetchAndCompareItems<SugarAccount>(mAccountsCollection, std::move(expected));

        compareToExpectedResult<SugarAccount>(arguments, "nameTest2", "idTest");
    }

    void shouldCorrectlySynchronizeExternalAccountDelete()
    {
        QList<QVariant> arguments;
        externalOperation(mAccountsCollection, "deleteAccount", "", "idTest", SIGNAL(itemRemoved(Akonadi::Item)), arguments);

        QList<ItemData> expected{{"accountZero", "0"}, {"accountOne", "1"}, {"accountTwo", "2"}};
        fetchAndCompareItems<SugarAccount>(mAccountsCollection, std::move(expected));
    }

    void shouldCreateAccount()
    {
        createSugarItem<SugarAccount>("newAccount", "1000", mAccountsCollection, {{"accountZero", "0"}, {"accountOne", "1"}, {"accountTwo", "2"}, {"newAccount", "1000"}}, "accountExist");
    }

    void shouldUpdateAccount()
    {
        updateSugarItem<SugarAccount>("updateAccount", "1000", mAccountsCollection, {{"accountZero", "0"}, {"accountOne", "1"}, {"accountTwo", "2"}, {"updateAccount", "1000"}}, "accountExist");
    }

    void shouldDeleteAccount()
    {
        deleteSugarItem<SugarAccount>("1000", mAccountsCollection, {{"accountZero", "0"}, {"accountOne", "1"}, {"accountTwo", "2"}});
    }

    //OPPORTUNITIES

    void shouldListOpportunities()
    {
        QList<ItemData> expected{{"validOpp", "100"}, {"oppWithNonExistingAccount", "101"}};
        fetchAndCompareItems<SugarOpportunity>(mOpportunitiesCollection, std::move(expected));
    }

    void shouldCorrectlySynchronizeExternalOpportunitiesCreation()
    {
        QList<QVariant> arguments;
        externalOperation(mOpportunitiesCollection,"addOpportunity", "nameTest", "idTest",
                          SIGNAL(itemAdded(Akonadi::Item,Akonadi::Collection)), arguments);

        QList<ItemData> expected{{"validOpp", "100"}, {"oppWithNonExistingAccount", "101"}, {"nameTest", "idTest"}};
        fetchAndCompareItems<SugarOpportunity>(mOpportunitiesCollection, std::move(expected));

        compareToExpectedResult<SugarOpportunity>(arguments, "nameTest", "idTest");
    }

    void shouldCorrectlySynchronizeExternalOpportunitiesUpdate()
    {
        QList<QVariant> arguments;
        externalOperation(mOpportunitiesCollection, "updateOpportunity", "nameTest2", "idTest",
                          SIGNAL(itemChanged(Akonadi::Item,QSet<QByteArray>)), arguments);

        QList<ItemData> expected{{"validOpp", "100"}, {"oppWithNonExistingAccount", "101"}, {"nameTest2", "idTest"}};
        fetchAndCompareItems<SugarOpportunity>(mOpportunitiesCollection, std::move(expected));

        compareToExpectedResult<SugarOpportunity>(arguments, "nameTest2", "idTest");
    }

    void shouldCorrectlySynchronizeExternalOpportunitiesDelete()
    {
        QList<QVariant> arguments;
        externalOperation(mOpportunitiesCollection, "deleteOpportunity", "", "idTest",
                          SIGNAL(itemRemoved(Akonadi::Item)), arguments);

        QList<ItemData> expected{{"validOpp", "100"}, {"oppWithNonExistingAccount", "101"}};
        fetchAndCompareItems<SugarOpportunity>(mOpportunitiesCollection, std::move(expected));
    }

    void shouldCreateOpportunity()
    {
        createSugarItem<SugarOpportunity>("newOpportunity", "1001", mOpportunitiesCollection, {{"validOpp", "100"}, {"oppWithNonExistingAccount", "101"}, {"newOpportunity", "1001"}}, "opportunityExist");
    }

    void shouldUpdateOpportunity()
    {
        updateSugarItem<SugarOpportunity>("updateOpportunity", "1001", mOpportunitiesCollection, {{"validOpp", "100"}, {"oppWithNonExistingAccount", "101"}, {"updateOpportunity", "1001"}}, "opportunityExist");
    }

    void shouldDeleteOpportunity()
    {
        deleteSugarItem<SugarOpportunity>("1001", mOpportunitiesCollection, {{"validOpp", "100"}, {"oppWithNonExistingAccount", "101"}});
    }

};

QTEST_AKONADIMAIN(TestSugarCRMResource)
#include "test_sugarcrmresource.moc"
