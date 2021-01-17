/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "contactsimporter.h"
#include "accountrepository.h"

#include <QTest>
#include <QTemporaryFile>

#include <config-phonenumber.h>

class TestContactsImporter : public QObject
{
    Q_OBJECT
public:
private Q_SLOTS:
    void testSimilarAccountNames_data()
    {
        QTest::addColumn<QString>("account1");
        QTest::addColumn<QString>("account2");
        QTest::addColumn<bool>("expectedSame");

        QTest::newRow("equal") << "KDAB" << "KDAB" << true;
        QTest::newRow("inc_comma") << "KDAB, Inc." << "KDAB" << true;
        QTest::newRow("inc2_comma") << "KDAB" << "KDAB, Inc." << true;
        QTest::newRow("sa_nocomma") << "KDAB" << "KDAB S.A." << true;
        QTest::newRow("AB_nospace") << "KDAB" << "KD" << false;
        QTest::newRow("sas_sa") << "KDAB S.A.S." << "KDAB S.A." << true;
        QTest::newRow("case_insensitive") << "EXAMPLE SAS" << "Example" << true;
        QTest::newRow("And_sign") << "KDAB Research & Development" << "KDAB Research And Development" << true;
        QTest::newRow("and_sign") << "KDAB Research & Development" << "KDAB Research and Development" << true;
        QTest::newRow("Spa") << "Foo Bar Spa" << "Foo Bar" << true;
        QTest::newRow("gmbh_and_co_kg") << "One Two GmbH & Co. KG" << "One Two" << true;
        QTest::newRow("NV") << "One Two" << "One Two N.V." << true;
        QTest::newRow("nv_lowercase") << "One Two" << "One Two n.v." << true;
        QTest::newRow("brackets") << "KDAB" << "KDAB [France]" << true; // these should be considered similar; the separate country comparison will tell more
    }

    void testSimilarAccountNames()
    {
        QFETCH(QString, account1);
        QFETCH(QString, account2);
        QFETCH(bool, expectedSame);

        SugarAccount ac1;
        ac1.setName(account1);
        SugarAccount ac2;
        ac2.setName(account2);
        QCOMPARE(ac1.isSameAccount(ac2), expectedSame);
        QCOMPARE(ac2.isSameAccount(ac1), expectedSame);
        if (expectedSame)
            QCOMPARE(ac1.key(), ac2.key());
        else
            QVERIFY(ac1.key() != ac2.key());

        AccountRepository *repo = AccountRepository::instance();
        repo->clear();
        ac1.setId(QStringLiteral("id1"));
        repo->addAccount(ac1, 0);
        QCOMPARE(repo->similarAccounts(ac2).count(), expectedSame ? 1 : 0);
    }

    void testMultipleAccounts_data()
    {
        QTest::addColumn<QString>("csv");
        QTest::addColumn<QStringList>("expectedAccountNames");
        QTest::addColumn<QStringList>("expectedEmails");

        QTest::newRow("empty") << "" << QStringList() << QStringList();
        QTest::newRow("one") << "David,Faure,Mr,12345,david.faure@example.com,KDAB,\"32, street name\",Vedène,84000,,France,FR 12345"
            << (QStringList() << QStringLiteral("KDAB")) << (QStringList() << QStringLiteral("david.faure@example.com"));
        QTest::newRow("dupe") << "David,Faure,Mr,12345,david.faure@example.com,KDAB,\"32, street name\",Vedène,84000,,France,FR 12345\n"
                                 "Clone,Faure,Mr,12345,clone.faure@example.com,KDAB,\"32, street name\",Vedène,84000,,France,FR 12345"
            << (QStringList() << QStringLiteral("KDAB")) << (QStringList() << QStringLiteral("david.faure@example.com") << QStringLiteral("clone.faure@example.com"));

        QTest::newRow("2+1") << "David,Faure,Mr,12345,david.faure@example.com,KDAB,\"32, street name\",Vedène,84000,,France,FR 12345\n"
                                "Clone,Faure,Mr,12345,clone.faure@example.com,KDAB Inc.,\"32, street name\",Vedène,84000,,France,FR 12345\n"
                                "Clone,Faure,Mr,12345,clone.faure@foo.com,Foo,\"32, street name\",Vedène,84000,,France,FR 12345"
            << (QStringList() << QStringLiteral("KDAB") << QStringLiteral("Foo")) << (QStringList() << QStringLiteral("david.faure@example.com") << QStringLiteral("clone.faure@example.com") << QStringLiteral("clone.faure@foo.com"));
    }

    void testMultipleAccounts()
    {
        QFETCH(QString, csv);
        QFETCH(QStringList, expectedAccountNames);
        QFETCH(QStringList, expectedEmails);

        ContactsImporter importer;
        QTemporaryFile file;
        writeTempFile(file, csv.toUtf8());
        QVERIFY(importer.importFile(file.fileName()));
        const QVector<ContactsSet> contacts = importer.contacts();
        QCOMPARE(extractAccountNames(contacts), expectedAccountNames);
        QCOMPARE(extractEmails(contacts), expectedEmails);
    }

    void testImportingAccounts()
    {
        ContactsImporter importer;
        QTemporaryFile file;
        writeTempFile(file,
        "David,Faure,Mr,12345,david.faure@kdab.com,KDAB,\"32, street name\",Vedène,84000,,France,FR 12345");
        QVERIFY(importer.importFile(file.fileName()));
        const QVector<ContactsSet> contacts = importer.contacts();
        QCOMPARE(contacts.size(), 1);
        const SugarAccount account = contacts.at(0).account;
        QCOMPARE(account.name(), QString("KDAB"));
        QCOMPARE(account.billingAddressStreet(), QString::fromUtf8("32, street name"));
        QCOMPARE(account.billingAddressCity(), QString::fromUtf8("Vedène"));
        QCOMPARE(account.billingAddressCountry(), QString::fromUtf8("France"));
        QCOMPARE(account.billingAddressPostalcode(), QString::fromUtf8("84000"));
        QCOMPARE(account.billingAddressState(), QString());
    }

#if USE_PHONENUMBER
    void testImportingAccountsParsingPhoneNumber_data()
    {
        QTest::addColumn<QString>("csv");
        QTest::addColumn<QLocale>("locale");
        QTest::addColumn<QString>("expectedCountry");

        // auto fill country based on phone number
        QTest::newRow("without_country") << "David,Faure,Mr,+331454532 45,david.faure@kdab.com,KDAB,\"32, street name\",,,,,FR 12345"
                                         << QLocale()
                                         << QStringLiteral("France");

        // Make sure that the country name is still in English even if the app Locale is not
        QTest::newRow("with_different_locale") << "Norris,Chuck,Mr,+1 6502530000,chuck.norris@hollywood.com,,,,,,,"
                                               << QLocale(QLocale::Portuguese, QLocale::Brazil)
                                               << QStringLiteral("United States");

        // country field has priority over phone number country
        QTest::newRow("with_different_country") << "David,Faure,Mr,+1 6502530000,david.faure@kdab.com,KDAB,\"32, street name\",Vedène,84000,,France,FR 12345"
                                                << QLocale()
                                                << QStringLiteral("France");

        // no country information
        QTest::newRow("with_local_phone_number") << "David,Faure,Mr,9427030155,david.faure@kdab.com,KDAB,\"32, street name\",Vedène,84000,,,"
                                                 << QLocale()
                                                 << QStringLiteral();
        QTest::newRow("without_phone_number") << "David,Faure,Mr,,david.faure@kdab.com,KDAB,\"32, street name\",Vedène,84000,,,"
                                              << QLocale()
                                              << QStringLiteral();
    }

    void testImportingAccountsParsingPhoneNumber()
    {
        QFETCH(QString, csv);
        QFETCH(QLocale, locale);
        QFETCH(QString, expectedCountry);

        QLocale::setDefault(locale);
        ContactsImporter importer;
        QTemporaryFile file;
        writeTempFile(file, csv.toUtf8());
        QVERIFY(importer.importFile(file.fileName()));

        const QVector<ContactsSet> contacts = importer.contacts();
        QCOMPARE(contacts.size(), 1);

        const SugarAccount account = contacts.at(0).account;
        QCOMPARE(account.billingAddressCountry(), expectedCountry);
    }
#endif // USE_PHONENUMBER

private:
    void writeTempFile(QTemporaryFile &file, const QByteArray& data) {
        QVERIFY(file.open());
        const QByteArray header = "First Name,Last Name,Title,Phone,Email,Company Name,Address,City,Zipcode,State/Province,Country,EU VAT ID\n";
        QCOMPARE(file.write(header), qlonglong(header.size()));
        QCOMPARE(file.write(data), qlonglong(data.size()));
        QVERIFY(file.seek(0));
    }

    static QStringList extractAccountNames(const QVector<ContactsSet> &contacts)
    {
        QStringList ret;
        ret.reserve(contacts.count());
        foreach(const ContactsSet &contactsSet, contacts) {
            ret << contactsSet.account.name();
        }
        return ret;
    }
    static QStringList extractEmails(const QVector<ContactsSet> &contacts)
    {
        QStringList ret;
        ret.reserve(contacts.count());
        foreach(const ContactsSet &contactsSet, contacts) {
            foreach (const KContacts::Addressee &addressee, contactsSet.addressees) {
                qDebug() << addressee.fullEmail();
                ret << addressee.preferredEmail();
            }
        }
        return ret;
    }
};

QTEST_MAIN(TestContactsImporter)
#include "test_contactsimporter.moc"
