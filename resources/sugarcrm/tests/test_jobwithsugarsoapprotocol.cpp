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
#include <QSettings>
#include <itemtransferinterface.h>
#include <sugardocument.h>
#include <enumdefinitionattribute.h>
#include <AkonadiCore/Item>
#include "sugaraccount.h"
#include "sugarsession.h"
#include "sugarsoapprotocol.h"
#include "loginjob.h"
#include "listentriesjob.h"
#include "createentryjob.h"
#include "updateentryjob.h"
#include "deleteentryjob.h"
#include "fetchentryjob.h"
#include "listmodulesjob.h"
#include "accountshandler.h"
#include "opportunitieshandler.h"
#include "documentshandler.h"

/**
 * This test talks to a REAL SugarCRM server.
 * It's the only test that makes sure we implement the SOAP API correctly.
 * Obviously you want to connect this to a test server, not to a production one.
 * The user/pass/host information is read from a hand-written QSettings file.
 * On Unix, that is ~/config/KDAB/test_jobwithsugarsoapprotocol.conf
 */
class TestJobWithSugarSoapProtocol : public QObject
{
    Q_OBJECT

public:
    TestJobWithSugarSoapProtocol()
        : mSession(nullptr)
    {
    }

private:
    SugarSession mSession;
    SugarSoapProtocol *mProtocol;
    SugarAccount mNewAccount;
    SugarOpportunity mNewOpp;
    QString mRemoteRevision;
    QScopedPointer<AccountsHandler> mAccountsHandler;
    QScopedPointer<OpportunitiesHandler> mOpportunitiesHandler;
    QScopedPointer<DocumentsHandler> mDocumentsHandler;

private Q_SLOTS:

    void initTestCase()
    {
        qApp->setOrganizationName("KDAB");
        QSettings settings;
        settings.beginGroup("SugarAuthentication");
        const QString userName = settings.value("username", QString()).toString();
        const QString password = settings.value("password", QString()).toString();
        const QString host = settings.value("host", QString()).toString();

        if (userName.isEmpty() || password.isEmpty() || host.isEmpty()) { // Create a template file
            if (userName.isEmpty()) {
                settings.setValue("username", QString());
            }
            if (password.isEmpty()) {
                settings.setValue("password", QString());
            }
            if (host.isEmpty()) {
                settings.setValue("host", QString());
            }
            QSKIP(qPrintable(QString("Missing authentication information for a test SugarCRM server, please edit %1 to write your username, password, and scheme+host. Do NOT use a production server!").arg(settings.fileName())),SkipAll);
        }
        mSession.setSessionParameters(userName, password, host);
        mSession.createSoapInterface();

        mProtocol = new SugarSoapProtocol;
        mSession.setProtocol(mProtocol);
        mProtocol->setSession(&mSession);

        mAccountsHandler.reset(new AccountsHandler(&mSession));
        mOpportunitiesHandler.reset(new OpportunitiesHandler(&mSession));
        mDocumentsHandler.reset(new DocumentsHandler(&mSession));

        LoginJob job(&mSession);
        QVERIFY2(job.exec(), qPrintable(job.errorString()));

        qRegisterMetaType<Akonadi::Collection>();
    }

    void shouldCorrectlyListModules()
    {
        //GIVEN
        ListModulesJob job(&mSession);
        //WHEN
        QVERIFY2(job.exec(), qPrintable(job.errorString()));
        //THEN
        QVERIFY(job.modules().contains(moduleToName(Module::Accounts)));
        QVERIFY(job.modules().contains(moduleToName(Module::Opportunities)));
        QVERIFY(job.modules().contains(moduleToName(Module::Documents)));
        QVERIFY(job.modules().contains(moduleToName(Module::Contacts)));
        QVERIFY(job.modules().contains(moduleToName(Module::Emails)));
    }

    void shouldListFields()
    {
        Akonadi::Collection collection; // unused

        // Accounts
        const KDSoapGenerated::TNS__Field_list accountFields = mAccountsHandler->listAvailableFields(&mSession, moduleToName(Accounts));
        QVERIFY(mAccountsHandler->parseFieldList(collection, accountFields));
        const EnumDefinitions accountEnums = mAccountsHandler->enumDefinitions();
        QCOMPARE(accountEnums.at(0).mEnumName, QStringLiteral("account_type"));
        QCOMPARE(accountEnums.at(1).mEnumName, QStringLiteral("industry"));

        // Test that the enums were stored into the collection
        // (but of course this doesn't check that the collection actually got synced)
        EnumDefinitionAttribute *enumsAttr = collection.attribute<EnumDefinitionAttribute>();
        QVERIFY(enumsAttr);
        const EnumDefinitions enumDefinitions = EnumDefinitions::fromString(enumsAttr->value());
        QCOMPARE(enumDefinitions.at(0).mEnumName, QStringLiteral("account_type"));
        QCOMPARE(enumDefinitions.at(1).mEnumName, QStringLiteral("industry"));

        // Opportunities
        const KDSoapGenerated::TNS__Field_list opportunitiesFields = mOpportunitiesHandler->listAvailableFields(&mSession, moduleToName(Opportunities));
        QVERIFY(mOpportunitiesHandler->parseFieldList(collection, opportunitiesFields));
        const EnumDefinitions opportunityEnums = mOpportunitiesHandler->enumDefinitions();
        QCOMPARE(opportunityEnums.at(0).mEnumName, QStringLiteral("opportunity_type"));
        QCOMPARE(opportunityEnums.at(1).mEnumName, QStringLiteral("lead_source"));
        QCOMPARE(opportunityEnums.at(2).mEnumName, QStringLiteral("sales_stage"));
        QCOMPARE(opportunityEnums.at(3).mEnumName, QStringLiteral("opportunity_priority"));
        QCOMPARE(opportunityEnums.at(4).mEnumName, QStringLiteral("opportunity_size"));
    }

    void shouldAddAccount()
    {
        //GIVEN
        SugarAccount account;
        account.setName("newAccount");
        Akonadi::Item item;
        item.setId(0);
        item.setPayload<SugarAccount>(account);
        CreateEntryJob *job = new CreateEntryJob(item, &mSession);
        job->setModule(mAccountsHandler.data());
        //WHEN
        QVERIFY2(job->exec(), qPrintable(job->errorString()));
        mNewAccount = job->item().payload<SugarAccount>();
        mRemoteRevision = job->item().remoteRevision();
    }

    void listAccountsShouldSeeNewAccount()
    {
        //WHEN
        QStringList newAccountIds, newAccountNames, deletedAccountIds, deletedAccountNames;
        QList<SugarAccount> newAccounts;
        listNewAndDeletedItems<SugarAccount>(mAccountsHandler.data(), newAccountIds, newAccountNames, deletedAccountIds, deletedAccountNames, newAccounts);
        // THEN
        QVERIFY2(newAccountNames.contains("newAccount"), qPrintable(newAccountNames.join(",")));
        QVERIFY2(!deletedAccountNames.contains("newAccount"), qPrintable(deletedAccountNames.join(",")));
        QVERIFY2(newAccountIds.contains(mNewAccount.id()), qPrintable(newAccountIds.join(',')));
        QVERIFY2(!deletedAccountIds.contains(mNewAccount.id()), qPrintable(deletedAccountIds.join(',')));
    }

    void listAccountsAgainShouldSeeTheSame() // it's incremental... but due to the one-second granularity, we have to get the same stuff again...
    {
        //WHEN
        QStringList newAccountIds, newAccountNames, deletedAccountIds, deletedAccountNames;
        QList<SugarAccount> newAccounts;
        listNewAndDeletedItems<SugarAccount>(mAccountsHandler.data(), newAccountIds, newAccountNames, deletedAccountIds, deletedAccountNames, newAccounts);
        // THEN
#if 0
        QVERIFY2(!newAccountNames.contains("newAccount"), qPrintable(newAccountNames.join(",")));
        QVERIFY2(!deletedAccountNames.contains("newAccount"), qPrintable(deletedAccountNames.join(",")));
        QVERIFY2(!newAccountIds.contains(mNewAccount.id()), qPrintable(newAccountIds.join(',')));
        QVERIFY2(!deletedAccountIds.contains(mNewAccount.id()), qPrintable(deletedAccountIds.join(',')));
#else
        QVERIFY2(newAccountNames.contains("newAccount"), qPrintable(newAccountNames.join(",")));
        QVERIFY2(!deletedAccountNames.contains("newAccount"), qPrintable(deletedAccountNames.join(",")));
        QVERIFY2(newAccountIds.contains(mNewAccount.id()), qPrintable(newAccountIds.join(',')));
        QVERIFY2(!deletedAccountIds.contains(mNewAccount.id()), qPrintable(deletedAccountIds.join(',')));
#endif
    }

    void shouldFetchAccount()
    {
        //GIVEN
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewAccount.id());
        FetchEntryJob job(item, &mSession);
        job.setModule(mAccountsHandler.data());
        //WHEN
        QVERIFY2(job.exec(), qPrintable(job.errorString()));
        //THEN
        SugarAccount foundAccount = job.item().payload<SugarAccount>();
        QCOMPARE(foundAccount.id(), mNewAccount.id());
        QCOMPARE(foundAccount.name(), mNewAccount.name());
    }

    void shouldUpdateAccount()
    {
        //GIVEN
        SugarAccount account;
        account.setName("updateAccount");
        account.setId(mNewAccount.id());
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewAccount.id());
        item.setRemoteRevision(mRemoteRevision);
        item.setPayload<SugarAccount>(account);
        UpdateEntryJob job(item, &mSession);
        job.setModule(mAccountsHandler.data());
        //WHEN
        QVERIFY2(job.exec(), qPrintable(job.errorString()));
        //THEN
        SugarAccount foundAccount = job.item().payload<SugarAccount>();
        QCOMPARE(foundAccount.name(), QString("updateAccount"));
        QCOMPARE(foundAccount.id(), mNewAccount.id());
    }

    void listAccountsShouldSeeUpdatedAccount()
    {
        //WHEN
        QStringList newAccountIds, newAccountNames, deletedAccountIds, deletedAccountNames;
        QList<SugarAccount> newAccounts;
        listNewAndDeletedItems<SugarAccount>(mAccountsHandler.data(), newAccountIds, newAccountNames, deletedAccountIds, deletedAccountNames, newAccounts);
        // THEN
        QVERIFY2(newAccountNames.contains("updateAccount"), qPrintable(newAccountNames.join(",")));
        QVERIFY2(!newAccountNames.contains("newAccount"), qPrintable(newAccountNames.join(",")));
        QVERIFY2(!deletedAccountNames.contains("newAccount"), qPrintable(deletedAccountNames.join(",")));
        QVERIFY2(!deletedAccountNames.contains("updateAccount"), qPrintable(deletedAccountNames.join(",")));
        QVERIFY2(newAccountIds.contains(mNewAccount.id()), qPrintable(newAccountIds.join(',')));
        QVERIFY2(!deletedAccountIds.contains(mNewAccount.id()), qPrintable(deletedAccountIds.join(',')));
    }

    void shouldDeleteAccount()
    {
        //GIVEN
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewAccount.id());
        DeleteEntryJob job(item, &mSession, Module::Accounts);
        //WHEN
        QVERIFY2(job.exec(), qPrintable(job.errorString()));
    }

    void listAccountsShouldSeeDeletedAccount()
    {
        //WHEN
        QStringList newAccountIds, newAccountNames, deletedAccountIds, deletedAccountNames;
        QList<SugarAccount> newAccounts;
        listNewAndDeletedItems<SugarAccount>(mAccountsHandler.data(), newAccountIds, newAccountNames, deletedAccountIds, deletedAccountNames, newAccounts);
        // THEN
        QVERIFY2(!newAccountNames.contains("updateAccount"), qPrintable(newAccountNames.join(",")));
        QVERIFY2(deletedAccountNames.contains("updateAccount"), qPrintable(deletedAccountNames.join(",")));
        QVERIFY2(!newAccountIds.contains(mNewAccount.id()), qPrintable(newAccountIds.join(',')));
        QVERIFY2(deletedAccountIds.contains(mNewAccount.id()), qPrintable(deletedAccountIds.join(',')));
    }

    void shouldFetchDeletedAccount()
    {
        //GIVEN
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewAccount.id());
        FetchEntryJob *job = new FetchEntryJob(item, &mSession);
        job->setModule(mAccountsHandler.data());
        //WHEN
        QVERIFY2(job->exec(), qPrintable(job->errorString()));
        //THEN
        SugarAccount foundAccount = job->item().payload<SugarAccount>();
        QCOMPARE(foundAccount.id(), mNewAccount.id());
        QCOMPARE(foundAccount.name(), QString());
        QCOMPARE(foundAccount.deleted(), QStringLiteral("1"));
    }

    void shouldCreateOpportunity()
    {
        //GIVEN
        SugarOpportunity opp;
        opp.setName("newOpportunity");
        Akonadi::Item item;
        item.setId(0);
        item.setPayload<SugarOpportunity>(opp);
        CreateEntryJob *job = new CreateEntryJob(item, &mSession);
        job->setModule(mOpportunitiesHandler.data());
        //WHEN
        QVERIFY2(job->exec(), qPrintable(job->errorString()));
        mNewOpp = job->item().payload<SugarOpportunity>();
        QVERIFY(!mNewOpp.dateModifiedRaw().isEmpty());
        mRemoteRevision = job->item().remoteRevision();
    }

    void shouldFetchOpportunity()
    {
        //GIVEN
        QVERIFY(!mNewOpp.isEmpty()); // this test requires shouldCreateOpportunity
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewOpp.id());
        FetchEntryJob *job = new FetchEntryJob(item, &mSession);
        job->setModule(mOpportunitiesHandler.data());
        //WHEN
        QVERIFY2(job->exec(), qPrintable(job->errorString()));
        //THEN
        SugarOpportunity foundOpp = job->item().payload<SugarOpportunity>();
        QCOMPARE(foundOpp.id(), mNewOpp.id());
        QCOMPARE(foundOpp.name(), mNewOpp.name());
    }

    void shouldUpdateOpportunity()
    {
        //GIVEN
        QVERIFY(!mNewOpp.isEmpty()); // this test requires shouldCreateOpportunity
        SugarOpportunity opp;
        opp.setName("updateOpportunity");
        opp.setId(mNewOpp.id());
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewOpp.id());
        item.setRemoteRevision(mRemoteRevision);
        item.setPayload<SugarOpportunity>(opp);
        UpdateEntryJob *job = new UpdateEntryJob(item, &mSession);
        job->setModule(mOpportunitiesHandler.data());
        //WHEN
        QVERIFY2(job->exec(), qPrintable(job->errorString()));
        //THEN
        SugarOpportunity foundOpp = job->item().payload<SugarOpportunity>();
        QCOMPARE(foundOpp.name(), QString("updateOpportunity"));
        QCOMPARE(foundOpp.id(), mNewOpp.id());
    }

    void shouldAttachDocument()
    {
        // GIVEN
        ItemTransferInterface transferInterface;
        transferInterface.setSession(&mSession);
        const QString localFilePath = QFINDTESTDATA("CMakeLists.txt");
        QVERIFY(!localFilePath.isEmpty());
        // WHEN
        const QString documentId = transferInterface.uploadDocument("MyDocument", "Paid", "Description", localFilePath);
        // THEN
        QVERIFY(!documentId.isEmpty());
        QVERIFY2(!documentId.contains("ERROR"), qPrintable(documentId));

        // AND WHEN
        QVERIFY(!mNewOpp.isEmpty()); // this test requires shouldCreateOpportunity
        const bool ok = transferInterface.linkItem(documentId, moduleToName(Documents), mNewOpp.id(), moduleToName(Opportunities));
        // THEN
        QVERIFY(ok);

        // Check that updating locally shows the attached document
        {
            QStringList newDocumentIds, newDocumentNames, deletedDocumentIds, deletedDocumentNames;
            QList<SugarDocument> newDocuments;
            listNewAndDeletedItems<SugarDocument>(mDocumentsHandler.data(), newDocumentIds, newDocumentNames, deletedDocumentIds, deletedDocumentNames, newDocuments);
            QVERIFY2(newDocumentIds.contains(documentId), qPrintable(newDocumentIds.join(',')));
            QVERIFY2(newDocumentNames.contains("MyDocument"), qPrintable(newDocumentNames.join(',')));
            QVERIFY2(!deletedDocumentIds.contains(documentId), qPrintable(deletedDocumentIds.join(',')));
            QVERIFY2(!deletedDocumentNames.contains("MyDocument"), qPrintable(deletedDocumentNames.join(',')));

            QCOMPARE(newDocuments.count(), 1);
            QCOMPARE(newDocuments.at(0).linkedOpportunityIds().count(), 1);
        }


        // Cleanup
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(documentId);
        DeleteEntryJob *job = new DeleteEntryJob(item, &mSession, Module::Documents);
        QVERIFY2(job->exec(), qPrintable(job->errorString()));

        QStringList newDocumentIds, newDocumentNames, deletedDocumentIds, deletedDocumentNames;
        QList<SugarDocument> newDocuments;
        listNewAndDeletedItems<SugarDocument>(mDocumentsHandler.data(), newDocumentIds, newDocumentNames, deletedDocumentIds, deletedDocumentNames, newDocuments);
        QVERIFY2(!newDocumentIds.contains(documentId), qPrintable(newDocumentIds.join(',')));
        QVERIFY2(!newDocumentNames.contains("MyDocument"), qPrintable(newDocumentNames.join(',')));
        QVERIFY2(deletedDocumentIds.contains(documentId), qPrintable(deletedDocumentIds.join(',')));
        QVERIFY2(deletedDocumentNames.contains("MyDocument"), qPrintable(deletedDocumentNames.join(',')));
    }

    void shouldDeleteOpportunity()
    {
        //GIVEN
        QVERIFY(!mNewOpp.isEmpty()); // this test requires shouldCreateOpportunity
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewOpp.id());
        DeleteEntryJob *job = new DeleteEntryJob(item, &mSession, Module::Opportunities);
        //WHEN
        QVERIFY2(job->exec(), qPrintable(job->errorString()));
    }

    void shouldFetchDeletedOpportunity()
    {
        //GIVEN
        QVERIFY(!mNewOpp.isEmpty()); // this test requires shouldCreateOpportunity
        Akonadi::Item item;
        item.setId(0);
        item.setRemoteId(mNewOpp.id());
        FetchEntryJob *job = new FetchEntryJob(item, &mSession);
        job->setModule(mOpportunitiesHandler.data());
        //WHEN
        QVERIFY2(job->exec(), qPrintable(job->errorString()));
        //THEN
        SugarOpportunity foundOpp = job->item().payload<SugarOpportunity>();
        QCOMPARE(foundOpp.id(), mNewOpp.id());
        QCOMPARE(foundOpp.name(), QString());
        QCOMPARE(foundOpp.deleted(), QStringLiteral("1"));
    }

private: // helper methods

    template <typename T> QString extractName(const T& object) { return object.name(); }
    QString extractName(const SugarDocument& document) { return document.documentName(); }

    template <typename T>
    void listNewAndDeletedItems(ModuleHandler *handler, QStringList &newIds, QStringList &newNames, QStringList &deletedIds, QStringList &deletedNames, QList<T>& newObjects)
    {
        Akonadi::Collection collection;
        ListEntriesJob *job = new ListEntriesJob(collection, &mSession);
        job->setModule(handler);
        job->setLatestTimestamp(mRemoteRevision);
        QSignalSpy receivedSpy(job, &ListEntriesJob::itemsReceived);
        QSignalSpy progressSpy(job, &ListEntriesJob::progress);
        QSignalSpy totalSpy(job, &ListEntriesJob::totalItems);

        QVERIFY2(job->exec(), qPrintable(job->errorString()));

        if (!receivedSpy.isEmpty()) {
            const Akonadi::Item::List items = receivedSpy.at(0).at(0).value<Akonadi::Item::List>();
            QVERIFY(!items.isEmpty());
            for (const Akonadi::Item &item : items) {
                QVERIFY(item.hasPayload<T>());
                const T object = item.payload<T>();
                newIds.append(object.id());
                newNames.append(extractName(object));
                newObjects.append(object);
            }
        }
        const Akonadi::Item::List deletedItems = job->deletedItems();
        for (const Akonadi::Item &item : deletedItems) {
            QVERIFY(item.hasPayload<T>());
            const T object = item.payload<T>();
            deletedIds.append(object.id());
            deletedNames.append(extractName(object));
        }
        if (!job->newTimestamp().isEmpty()) {
            mRemoteRevision = job->newTimestamp();
        }
        QCOMPARE(totalSpy.count(), 1);
        const int numberOfItems = newNames.count() + deletedNames.count();
        QCOMPARE(totalSpy.at(0).at(0).toInt(), numberOfItems);
        if (numberOfItems > 0) {
            QCOMPARE(progressSpy.count(), 1);
            QCOMPARE(progressSpy.at(0).at(0).toInt(), numberOfItems);
        }
    }
};

QTEST_MAIN(TestJobWithSugarSoapProtocol)
#include "test_jobwithsugarsoapprotocol.moc"
