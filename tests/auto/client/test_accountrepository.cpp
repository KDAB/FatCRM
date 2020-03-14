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

#include <QTest>
#include <QDebug>
#include "sugaraccount.h"
#include "accountrepository.h"
#include <QSignalSpy>


class TestAccountRepository : public QObject
{
    Q_OBJECT
public:
private:
    using Field = AccountRepository::Field;

    static QString fieldsVectorToString(QVector<AccountRepository::Field> fields)
    {
        std::sort(fields.begin(), fields.end());
        QString str;
        for (int i = 0; i < fields.size(); i++) {
            if (!str.isEmpty()) {
                str += ',';
            }
            switch (fields.at(i)) {
            case AccountRepository::Field::Country:
                str += "country";
            break;
            case AccountRepository::Field::Name:
                str += "name";
            break;
            }
        }
        return str;
    }

private Q_SLOTS:

    void shouldFindAccountById()
    {
        //GIVEN
        AccountRepository *repository = AccountRepository::instance();
        SugarAccount account;
        account.setId("12");
        repository->addAccount(account, 18);
        //THEN
        QVERIFY(repository->hasId("12"));
        QVERIFY(!repository->hasId("13"));
    }

    void shouldClearCorrectly()
    {
        //GIVEN
        AccountRepository *repository = AccountRepository::instance();
        SugarAccount account;
        account.setId("32");
        repository->addAccount(account, 20);
        //WHEN
        repository->clear();
        //THEN
        QVERIFY(!repository->hasId("32"));
    }

    void shouldDisappearAfterRemove()
    {
        //GIVEN
        AccountRepository *repository = AccountRepository::instance();
        SugarAccount account;
        account.setId("15");
        repository->addAccount(account, 22);
        //WHEN
        repository->removeAccount(account);
        //THEN
        QVERIFY(!repository->hasId("15"));
    }

    void shouldReturnCorrectAccountById()
    {
        //GIVEN
        AccountRepository *repository = AccountRepository::instance();
        repository->clear();
        SugarAccount account;
        account.setId("1");
        account.setName("test");
        repository->addAccount(account, 42);
        //WHEN
        const SugarAccount accountReceived = repository->accountById("1");
        //THEN
        QCOMPARE(account.name(), accountReceived.name());
        QCOMPARE(account.id(), accountReceived.id());
    }

    void shouldReturnCorrectAccountByKey()
    {
        //GIVEN
        AccountRepository *repository = AccountRepository::instance();
        repository->clear();
        SugarAccount account1;
        account1.setId("1");
        account1.setName("KDAB");
        repository->addAccount(account1, 1);

        SugarAccount account2;
        account2.setId("2");
        account2.setName("test");
        repository->addAccount(account2, 1);
        //WHEN
        const QString k = account1.key();
        const QList<SugarAccount> accountsReceived = repository->accountsByKey(k);
        //THEN
        QCOMPARE(accountsReceived.size(), 1);
        QCOMPARE(account1.name(), accountsReceived.at(0).name());
        QCOMPARE(account1.id(), accountsReceived.at(0).id());
    }

    void shouldReturnCorrectCountries_data()
    {
        QTest::addColumn<QStringList>("accountsCountries");
        QTest::addColumn<QStringList>("exceptCountries");

        QTest::newRow("simple") << QStringList{"FR", "UK"} << QStringList{"FR", "UK"};
        QTest::newRow("repeated_country") << QStringList{"FR", "FR", "UK"} << QStringList{"FR", "UK"};
    }

    void shouldReturnCorrectCountries()
    {
        QFETCH(QStringList, accountsCountries);
        QFETCH(QStringList, exceptCountries);

        //GIVEN
        AccountRepository *repository = AccountRepository::instance();
        repository->clear();

        for (int i = 0; i < accountsCountries.size(); i++) {
            SugarAccount account;
            account.setId(QString::number(i));
            account.setBillingAddressCountry(accountsCountries.at(i));
            repository->addAccount(account, 1);
        }
        //WHEN
        QStringList countryList = repository->countries();
        //THEN
        countryList.sort();
        QCOMPARE(countryList, exceptCountries);
    }


    void findCorrectWhatWasChanged_data()
    {
        QTest::addColumn<SugarAccount>("originalAccount");
        QTest::addColumn<SugarAccount>("modifyAccount");
        QTest::addColumn<QVector<Field>>("expectedField");

        auto createAccount = [](const QString &name, const QString &billingCountry, const QString &shippingCountry)
        {
            SugarAccount account;
            account.setId("1");
            account.setName(name);
            account.setBillingAddressCountry(billingCountry);
            account.setShippingAddressCountry(shippingCountry);

            return account;
        };

        QTest::newRow("no_Modification") << createAccount("KDAB", "D", "D")
                                         << createAccount("KDAB", "D", "D")
                                         << QVector<Field>{};
        QTest::newRow("Name_Modification") << createAccount("KDAB_france", "D", "D")
                                           << createAccount("KDAB", "D", "D")
                                           << QVector<Field>{Field::Name};
        QTest::newRow("billing_Country_Modification") << createAccount("KDAB", "D", "D")
                                                      << createAccount("KDAB", "FR", "D")
                                                      << QVector<Field>{Field::Country};
        QTest::newRow("shipping_Country_Modification") << createAccount("KDAB", "D", "D")
                                                       << createAccount("KDAB", "D", "FR")
                                                       << QVector<Field>{};
        QTest::newRow("billing_shipping_Country_Modification") << createAccount("KDAB_france", "D", "D")
                                                               << createAccount("KDAB_france", "FR", "FR")
                                                               << QVector<Field>{Field::Country};
        QTest::newRow("Name_billing_modification") << createAccount("KDAB", "D", "D")
                                                   << createAccount("KDAB_france", "FR", "D")
                                                   << QVector<Field>{Field::Name,Field::Country};
        QTest::newRow("Name_shipping_modification") << createAccount("KDAB", "D", "D")
                                                    << createAccount("KDAB_france", "D", "FR")
                                                    << QVector<Field>{Field::Name};
        QTest::newRow("Name_billing_shipping_modification") << createAccount("KDAB", "D", "D")
                                                            << createAccount("KDAB_france", "FR", "FR")
                                                            << QVector<Field>{Field::Name,Field::Country};
    }

    void findCorrectWhatWasChanged()
    {
        QFETCH(SugarAccount, originalAccount);
        QFETCH(SugarAccount, modifyAccount);
        QFETCH(QVector<Field>, expectedField);
        //GIVEN
        AccountRepository *repository = AccountRepository::instance();
        repository->clear();
        repository->addAccount(originalAccount, 1);
        QSignalSpy spy(repository, SIGNAL(accountModified(QString,QVector<AccountRepository::Field>)));
        //WHEN
        const QVector<AccountRepository::Field> modifyField = repository->modifyAccount(modifyAccount);
        //THEN
        const QString sModifyField = fieldsVectorToString(modifyField);
        const QString sExpectedField = fieldsVectorToString(expectedField);
        QCOMPARE(sModifyField, sExpectedField);
        if (!expectedField.isEmpty()) {
            QCOMPARE(spy.count(), 1);
            const QList<QVariant> arguments = spy.at(0);
            QCOMPARE(arguments.at(0).toString(), QString("1"));
            const QVector<Field> vFields = arguments.at(1).value<QVector<Field>>();
            const QString sFields = fieldsVectorToString(vFields);
            QCOMPARE(sFields, sExpectedField);
        } else {
            QCOMPARE(spy.count(), 0);
        }
    }


    void addAccountShouldEmitSignal()
    {
        //GIVEN
        AccountRepository *repository = AccountRepository::instance();
        repository->clear();
        qRegisterMetaType<Akonadi::Item::Id>("Akonadi::Item::Id");
        QSignalSpy spy(repository, SIGNAL(accountAdded(QString,Akonadi::Item::Id)));
        //WHEN
        SugarAccount account;
        account.setId("1");
        repository->addAccount(account, 2);
        //THEN
        QCOMPARE(spy.count(), 1);
        const QList<QVariant> arguments = spy.at(0);
        QCOMPARE(arguments.at(0).toString(), QString("1"));
        QCOMPARE(arguments.at(1).toInt(), 2);
    }

    void removeAccountShouldEmitSignal()
    {
        //GIVEN
        AccountRepository *repository = AccountRepository::instance();
        repository->clear();
        SugarAccount account;
        account.setId("1");
        repository->addAccount(account, 2);
        QSignalSpy spy(repository, &AccountRepository::accountRemoved);
        //WHEN
        repository->removeAccount(account);
        //THEN
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("1"));
    }
};

QTEST_MAIN(TestAccountRepository)
#include "test_accountrepository.moc"
