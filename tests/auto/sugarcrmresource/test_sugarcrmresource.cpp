/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2017-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sugaraccount.h"
#include "sugaropportunity.h"

#include <akonadi_version.h>
#include <AkonadiCore/AttributeFactory>
#include <AkonadiCore/Collection>
#include <AkonadiCore/CollectionFetchJob>
#include <AkonadiCore/CollectionFetchScope>
#include <AkonadiCore/EntityAnnotationsAttribute>
#include <AkonadiCore/EntityDisplayAttribute>
#include <AkonadiCore/Item>
#include <AkonadiCore/ItemCreateJob>
#include <AkonadiCore/ItemDeleteJob>
#include <AkonadiCore/ItemFetchJob>
#include <AkonadiCore/ItemFetchScope>
#include <AkonadiCore/ItemModifyJob>
#include <AkonadiCore/Job>
#include <AkonadiCore/Monitor>
#include <AkonadiCore/servermanager.h>
#include <AkonadiCore/qtest_akonadi.h>

#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QSignalSpy>

#include <QTest>

Q_DECLARE_METATYPE(QSet<QByteArray>)

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

static QString serviceName()
{
    QString serviceName = "org.freedesktop.Akonadi.Agent.akonadi_sugarcrm_resource_0.testrunner-";
    serviceName += QString::fromLocal8Bit(qgetenv("AKONADI_TESTRUNNER_PID"));
    return serviceName;
}

/**
 * This test uses the Akonadi API to talk to the full SugarCRM resource, *but*
 * it uses the Mock protocol implementation behind it, so no SOAP communication happens
 * and no SugarCRM server is necessary.
 */
class TestSugarCRMResource : public QObject
{
    Q_OBJECT

private:
    Collection mAccountsCollection;
    Collection mOpportunitiesCollection;
    Item mAccountItem;
    Item mOpportunityItem;

    static QString sessionId()
    {
        QDBusInterface mock(serviceName(), s_dbusObjectName, s_dbusInterfaceName);
        QDBusReply<QString> reply = mock.call("sessionId");
        if (!reply.isValid()) {
            return "ERROR : " + reply.error().message().toLatin1();
        }
        return reply.value();
    }

    // Fetch from the DB
    template<typename T>
    static void fetchItems(Collection &col, QList<ItemData> &result)
    {
        Q_ASSERT(result.isEmpty());
        auto *job = new ItemFetchJob(col);
        job->fetchScope().fetchFullPayload();
        AKVERIFYEXEC(job);
        const Item::List items = job->items();
        for (const Item &item : items) {
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
            QCOMPARE(result.at(i).name, expected.at(i).name);
            QCOMPARE(result.at(i).id, expected.at(i).id);
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
        QVERIFY(!expectedId.isEmpty());
        const Item item = arguments.at(0).value<Item>();
        QVERIFY(item.hasPayload<T>());
        T inst = item.payload<T>();

        qDebug() << "name:" << inst.name() << "id:" << inst.id() << "expectedId" << expectedId;
        QCOMPARE(inst.name(), expectedName);
        QCOMPARE(inst.id(), expectedId);
    }

    void monitorInit(Monitor &monitor, const Collection &col)
    {
        monitor.setCollectionMonitored(col);
        configureItemFetchScope(monitor.itemFetchScope());
    }

    void externalOperation(Collection &collection, const QString &operationName, const QString &name, const QString &id,
                           const char * signal, QList<QVariant> &arguments)
    {
        Monitor monitor;
        monitorInit(monitor, collection);
        QDBusInterface mock(serviceName(), s_dbusObjectName, s_dbusInterfaceName);
        QDBusReply<void> reply;
        if (name.isEmpty()) {
            reply = mock.call(operationName, id);
        } else {
            reply = mock.call(operationName, name, id);
        }
        QVERIFY2(reply.isValid(), reply.error().message().toLatin1());
        AgentManager::self()->synchronizeCollection(collection);

        QSignalSpy spy(&monitor, signal);
        QVERIFY2(spy.wait(), (QByteArray("Signal not emitted: ") + signal).constData());
        QCOMPARE(spy.count(), 1);
        arguments = spy.at(0);
    }

    QDateTime nextTimeStamp()
    {
        QDBusInterface mock(serviceName(), s_dbusObjectName, s_dbusInterfaceName);
        QDBusReply<QDateTime> reply = mock.call("nextTimeStamp");
        Q_ASSERT(reply.isValid());
        return reply.value();
    }

    static QString collectionTimestamp(const Collection &collection)
    {
        auto *annotationsAttribute = collection.attribute<EntityAnnotationsAttribute>();
        if (annotationsAttribute) {
            return annotationsAttribute->value("timestamp");
        }
        return QString();
    }

    template<typename T>
    void createSugarItem(const QString &name, const QString &id, Collection &collection, QList<ItemData> &&expected, const QString &operationName, Item &createdItem)
    {
        //GIVEN
        T sugarItem;
        sugarItem.setId(id);
        sugarItem.setName(name);
        sugarItem.setDateModified(nextTimeStamp());
        Item item;
        item.setPayload(sugarItem);
        item.setMimeType(T::mimeType());
        auto *job = new ItemCreateJob(item, collection);

        const QString oldTimestamp = collectionTimestamp(collection);
        QVERIFY(!oldTimestamp.isEmpty());

        //WHEN
        AKVERIFYEXEC(job);
        //THEN
        Monitor monitor;
        monitorInit(monitor, collection);
        QSignalSpy spyAdded(&monitor, SIGNAL(itemAdded(Akonadi::Item,Akonadi::Collection)));
        QSignalSpy spyChanged(&monitor, SIGNAL(itemChanged(Akonadi::Item,QSet<QByteArray>)));

        QVERIFY(spyAdded.wait());

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

        QCOMPARE(spyAdded.count(), 1);

        const QList<QVariant> arguments = spyAdded.at(0);
        compareToExpectedResult<T>(arguments, name, id);

        // The resource changes the item again, to set ID, modification time etc.
        QVERIFY(spyChanged.wait());
        QTRY_COMPARE(spyChanged.count(), 2);

        const QList<QVariant> argumentsUpdate = spyChanged.at(spyChanged.count()-1);
        compareToExpectedResult<T>(argumentsUpdate, name, id);

        fetchAndCompareItems<T>(collection, std::move(expected));

        createdItem = spyChanged.at(spyChanged.count()-1).at(0).value<Item>();
        QVERIFY(createdItem.hasPayload<T>());
        QVERIFY(createdItem.id() != 0);

        QDBusInterface mock(serviceName(), s_dbusObjectName, s_dbusInterfaceName);
        QDBusReply<bool> reply = mock.call(operationName, name, id);
        QVERIFY2(reply.isValid(), reply.error().message().toLatin1());
        QVERIFY2(reply.value(), qPrintable(QString(operationName + " failed")));

        // Check that the timestamp was updated
        auto *fetchJob = new CollectionFetchJob(collection, CollectionFetchJob::Base);
        AKVERIFYEXEC(fetchJob);
        QCOMPARE(fetchJob->collections().count(), 1);
        collection = fetchJob->collections().at(0);
        const QString newTimestamp = collectionTimestamp(collection);
        QVERIFY(!newTimestamp.isEmpty());
        QVERIFY2(newTimestamp != oldTimestamp, qPrintable(oldTimestamp));
    }

    // Modify the item defined by @p item to set the name @p name, in @p collection
    // Then fetch from that collection and compare with @p expected
    // Finally call @p operationName (e.g. "accountExists")
    template<typename T>
    void updateSugarItem(Item &item, const QString &name, Collection &collection, QList<ItemData> &&expected, const QString &operationName)
    {
        //GIVEN
        T sugarItem = item.payload<T>();
        const QString id = sugarItem.id();
        Q_ASSERT(!id.isEmpty());
        sugarItem.setName(name);
        item.setPayload(sugarItem);
        const int oldRevision = item.revision();
        QCOMPARE(item.mimeType(), T::mimeType());
        auto *job = new ItemModifyJob(item);
        //WHEN
        AKVERIFYEXEC(job);
        //THEN
        Monitor monitor;
        monitorInit(monitor, collection);

        // We should receive two itemChanged notifications.
        // 1) from the modification made by us just above
        // 2) when the resource updates date_modified (see end of SugarCRMResource::updateEntryResult)
        QSignalSpy spy(&monitor, SIGNAL(itemChanged(Akonadi::Item,QSet<QByteArray>)));
        QVERIFY(spy.wait());
        auto firstItem = spy.at(0).at(0).value<Akonadi::Item>();
        QCOMPARE(firstItem.revision(), oldRevision + 1);
        QCOMPARE(spy.at(0).at(1).value<QSet<QByteArray>>(), QSet<QByteArray>{QByteArray("PLD:RFC822")});
        QVERIFY(spy.wait());
        QCOMPARE(spy.count(), 2);
#if 0
        QTest::qWait(2000);
        for (int i = 0; i < spy.count(); i++) {
            const QList<QVariant> arguments = spy.at(i);
            const Item item = arguments.at(0).value<Item>();
            T inst = item.payload<T>();
            qDebug() << "UPDATE ////" << "changed" << i << "name:" << inst.name() << "id:" << inst.id() << "remoteId:" << item.remoteId();
        }
#endif

        const QList<QVariant> arguments = spy.at(1);
        compareToExpectedResult<T>(arguments, name, id);
        item = spy.at(1).at(0).value<Akonadi::Item>(); // update revision, to avoid a conflict

        fetchAndCompareItems<T>(collection, std::move(expected));

        QDBusInterface mock(serviceName(), s_dbusObjectName, s_dbusInterfaceName);
        QDBusReply<bool> reply = mock.call(operationName, name, id);
        QVERIFY2(reply.isValid(), reply.error().message().toLatin1());
        QVERIFY(reply.value());
    }

    template<typename T>
    void deleteSugarItem(const Item &item, Collection &collection, QList<ItemData> &&expected)
    {
        //GIVEN
        Monitor monitor;
        monitor.setCollectionMonitored(collection);
        configureItemFetchScope(monitor.itemFetchScope());
        QSignalSpy spyRemoved(&monitor, SIGNAL(itemRemoved(Akonadi::Item)));
        auto *job = new ItemDeleteJob(item);
        //WHEN
        AKVERIFYEXEC(job);
        //THEN
        if (spyRemoved.isEmpty()) {
            QVERIFY(spyRemoved.wait());
        }

        fetchAndCompareItems<T>(collection, std::move(expected));
    }

    void setInvalidSessionNextError()
    {
        QDBusInterface mock(serviceName(), s_dbusObjectName, s_dbusInterfaceName);
        QDBusReply<void> reply = mock.call("setNextSoapError", "Fault code 11: Invalid Session ID");
        QVERIFY2(reply.isValid(), reply.error().message().toLatin1());
    }

private Q_SLOTS:

    void initTestCase()
    {
        AkonadiTest::checkTestIsIsolated();
        qRegisterMetaType<Akonadi::Item>();
        qRegisterMetaType<Akonadi::Collection>();
        qRegisterMetaType<QSet<QByteArray> >();
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
        const Collection::List collections = fetchJob->collections();
        QCOMPARE(collections.size(), 6);
        for (const Collection &col : collections) {
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

    // When an account created "on the server" (which we do via a DBus call to the mock object)
    // then synchronizing should make the new account appear.
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
        createSugarItem<SugarAccount>("newAccount", "1000", mAccountsCollection, {{"accountZero", "0"}, {"accountOne", "1"}, {"accountTwo", "2"}, {"newAccount", "1000"}}, "accountExists", mAccountItem);
    }

    void shouldUpdateAccount()
    {
        updateSugarItem<SugarAccount>(mAccountItem, "updateAccount", mAccountsCollection, {{"accountZero", "0"}, {"accountOne", "1"}, {"accountTwo", "2"}, {"updateAccount", "1000"}}, "accountExists");
    }

    void shouldDeleteAccount()
    {
        deleteSugarItem<SugarAccount>(mAccountItem, mAccountsCollection, {{"accountZero", "0"}, {"accountOne", "1"}, {"accountTwo", "2"}});
    }

    void shouldForgetSessionWhenInvalid()
    {
        setInvalidSessionNextError();
        QCOMPARE(sessionId(), QString("1"));

        // WHEN
        AgentManager::self()->synchronizeCollection(mAccountsCollection); // should fail

        // THEN the session id should be forgotten
        // ... and the next attempt should succeed
        QList<QVariant> arguments;
        externalOperation(mAccountsCollection, "addAccount", "nameTestSession", "idTestSession", SIGNAL(itemAdded(Akonadi::Item,Akonadi::Collection)), arguments);
        QList<ItemData> expected{{"accountZero", "0"}, {"accountOne", "1"}, {"accountTwo", "2"}, {"nameTestSession", "idTestSession"}};
        fetchAndCompareItems<SugarAccount>(mAccountsCollection, std::move(expected));
        compareToExpectedResult<SugarAccount>(arguments, "nameTestSession", "idTestSession");
        QCOMPARE(sessionId(), QString("2")); // with a new session ID
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
        createSugarItem<SugarOpportunity>("newOpportunity", "1001", mOpportunitiesCollection, {{"validOpp", "100"}, {"oppWithNonExistingAccount", "101"}, {"newOpportunity", "1001"}}, "opportunityExists", mOpportunityItem);
    }

    void shouldUpdateOpportunity()
    {
        updateSugarItem<SugarOpportunity>(mOpportunityItem, "updateOpportunity", mOpportunitiesCollection, {{"validOpp", "100"}, {"oppWithNonExistingAccount", "101"}, {"updateOpportunity", "1001"}}, "opportunityExists");
    }

    void shouldCorrectlySaveWhenInvalidSession()
    {
        setInvalidSessionNextError();
        updateSugarItem<SugarOpportunity>(mOpportunityItem, "updated opportunity with invalid session", mOpportunitiesCollection, {{"validOpp", "100"}, {"oppWithNonExistingAccount", "101"}, {"updated opportunity with invalid session", "1001"}}, "opportunityExists");
    }

    // Forcing a resource-fetch (from the server) for an opportunity we have in cache
    void forceRefreshShouldCallResource()
    {
        // Modify an opp without changing its modificationDate
        // (Still not sure whether this can happen in real life, but just in case, we now have a workaround...)
        const QString id = "1001";
        QDBusInterface mock(serviceName(), s_dbusObjectName, s_dbusInterfaceName);
        QDBusReply<void> reply = mock.call("updateOpportunity", "no_touch", id);
        QVERIFY2(reply.isValid(), reply.error().message().toLatin1());

        // Clear the payload, to force a refetch from the resource
        Item item(mOpportunityItem.id());
        item.clearPayload();
        // Workaround akonadi bug which was fixed in akonadi 5.7.3 (commit a2a85090c)
#if AKONADI_VERSION < QT_VERSION_CHECK(5, 7, 3) || AKONADI_VERSION == QT_VERSION_CHECK(5, 7, 40)
        item.addAttribute(new Akonadi::EntityDisplayAttribute());
        item.removeAttribute<Akonadi::EntityDisplayAttribute>();
#endif
        auto *modifyJob = new ItemModifyJob(item);
        AKVERIFYEXEC(modifyJob);
        mOpportunityItem = modifyJob->item();

        // List opps again, we should see the new name
        QList<ItemData> expected{{"validOpp", "100"}, {"oppWithNonExistingAccount", "101"}, {"no_touch", "1001"}};
        fetchAndCompareItems<SugarOpportunity>(mOpportunitiesCollection, std::move(expected));
    }

    void shouldDeleteOpportunity()
    {
        deleteSugarItem<SugarOpportunity>(mOpportunityItem, mOpportunitiesCollection, {{"validOpp", "100"}, {"oppWithNonExistingAccount", "101"}});
    }

    void syncShouldNotifyOfProgress()
    {
        // GIVEN
        AgentManager *agentManager = AgentManager::self();
        AgentInstance currentAgent = agentManager->instance("akonadi_sugarcrm_resource_0");
        QVERIFY(currentAgent.isValid());
        QVERIFY(currentAgent.isOnline());

        QSignalSpy spyProgress(agentManager, &AgentManager::instanceProgressChanged);

        QStringList statusMessages;
        QVector<int> progresses;
        connect(agentManager, &AgentManager::instanceProgressChanged,
                this, [&progresses, &currentAgent, &statusMessages](const AgentInstance &instance) { // clazy:exclude=lambda-in-connect
            if (instance == currentAgent) {
                progresses << instance.progress();
                statusMessages << instance.statusMessage();
            }
        });

        QVector<AgentInstance::Status> statuses;
        connect(agentManager, &AgentManager::instanceStatusChanged,
                this, [&statuses, &currentAgent, &statusMessages](const AgentInstance &instance) { // clazy:exclude=lambda-in-connect
            if (instance == currentAgent) {
                statuses << instance.status();
                statusMessages << instance.statusMessage();
            }
        });

        // WHEN
        currentAgent.synchronize();

        // THEN
        while (!statusMessages.contains(QLatin1String("Ready"))) {
            qDebug() << "progress" << progresses;
            qDebug() << "status" << statuses << statusMessages;
            spyProgress.wait();
        }
    }

};

QTEST_AKONADIMAIN(TestSugarCRMResource)
#include "test_sugarcrmresource.moc"
