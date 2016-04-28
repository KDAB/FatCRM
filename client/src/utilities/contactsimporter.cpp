/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#define QT_NO_CAST_FROM_ASCII
#define QT_NO_CAST_TO_ASCII

#include "contactsimporter.h"
#include "qcsvreader.h"
#include "kdcrmdata/kdcrmfields.h"
#include "kdcrmdata/kdcrmutils.h"

#include <QFile>
#include <QDebug>
#include <QTextCodec>

static const int COLUMN_COUNTRY = 10;

ContactsImporter::ContactsImporter()
{
}

bool ContactsImporter::importFile(const QString &fileName)
{
    mContacts.clear();

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QCsvStandardBuilder builder;
    QCsvReader reader(&builder);
    reader.setDelimiter(QLatin1Char(','));
    reader.setTextCodec(QTextCodec::codecForName("utf-8"));
    if (!reader.read(&file))
        return false;

    QMap<int, QString> accountColumns;
    accountColumns.insert(5, KDCRMFields::name());
    accountColumns.insert(6, KDCRMFields::billingAddressStreet());
    accountColumns.insert(7, KDCRMFields::billingAddressCity());
    accountColumns.insert(8, KDCRMFields::billingAddressPostalcode());
    accountColumns.insert(9, KDCRMFields::billingAddressState());
    accountColumns.insert(COLUMN_COUNTRY, KDCRMFields::billingAddressCountry());
    accountColumns.insert(11, KDCRMFields::vatNo());
    accountColumns.insert(12, KDCRMFields::website());
    accountColumns.insert(13, KDCRMFields::description());

    const int rows = builder.rowCount();
    mContacts.reserve(rows);
    for (int row = 1; row < rows; ++row) { // skip title row
        QMap<QString, QString> accountData;
        QMap<int, QString>::const_iterator it = accountColumns.constBegin();
        for ( ; it != accountColumns.end() ; ++it) {
            QString value = builder.data(row, it.key());
            //qDebug() << it.key() << value << "->" << it.value();
            if (it.key() == COLUMN_COUNTRY) {
                value = KDCRMUtils::canonicalCountryName(value);
            }
            if (!value.isEmpty()) {
                accountData.insert(it.value(), value);
            }
        }

        KABC::Addressee addressee;
        const QString givenName = builder.data(row, 0).trimmed();
        if (!givenName.isEmpty())
            addressee.setGivenName(givenName);

        const QString familyName = builder.data(row, 1).trimmed();
        if (!familyName.isEmpty())
            addressee.setFamilyName(familyName);

        const QString prefix = builder.data(row, 2).trimmed();
        if (!prefix.isEmpty())
            addressee.insertCustom(QLatin1String("FATCRM"), QLatin1String("X-Salutation"), prefix);

        const QString phoneNumber = builder.data(row, 3).trimmed();
        if (!phoneNumber.isEmpty())
            addressee.insertPhoneNumber(KABC::PhoneNumber(phoneNumber, KABC::PhoneNumber::Work));

        const QString emailAddress = builder.data(row, 4).trimmed();
        if (!emailAddress.isEmpty())
            addressee.insertEmail(emailAddress, true);

        const QString companyName = builder.data(row, 5).trimmed();
        if (!companyName.isEmpty())
            addressee.setOrganization(companyName);

        KABC::Address workAddress(KABC::Address::Work|KABC::Address::Pref);
        const QString workStreet = builder.data(row, 6).trimmed();
        if (!workStreet.isEmpty())
            workAddress.setStreet(workStreet);

        const QString workCity = builder.data(row, 7).trimmed();
        if (!workCity.isEmpty())
            workAddress.setLocality(workCity);

        const QString workZipCode = builder.data(row, 8).trimmed();
        if (!workZipCode.isEmpty())
            workAddress.setPostalCode(workZipCode);

        const QString workState = builder.data(row, 9).trimmed();
        if (!workState.isEmpty())
            workAddress.setRegion(workState);

        const QString workCountry = builder.data(row, 10).trimmed();
        if (!workCountry.isEmpty())
            workAddress.setCountry(workCountry);

        if (!workAddress.isEmpty())
            addressee.insertAddress(workAddress);

        const QString jobTitle = builder.data(row, 14).trimmed();
        if (!jobTitle.isEmpty())
            addressee.setTitle(jobTitle);

        if (accountData.value(KDCRMFields::name()).trimmed().isEmpty()) {
            const QString identifier = ((!givenName.isEmpty() || !familyName.isEmpty()) ? QString::fromLatin1("%1 %2").arg(givenName, familyName).trimmed() : emailAddress);
            accountData.insert(KDCRMFields::name(), QString::fromLatin1("%1 (individual)").arg(identifier));
        }

        SugarAccount newAccount;
        newAccount.setData(accountData);

        auto existingSet = std::find_if(mContacts.begin(), mContacts.end(),
                                        [&newAccount](const ContactsSet &contactSet) { return contactSet.account.isSameAccount(newAccount); });

        if (existingSet != mContacts.end()) {
            (*existingSet).addressees.append(addressee);
        } else {
            ContactsSet contactsSet;
            contactsSet.account = newAccount;
            contactsSet.addressees.append(addressee);

            mContacts.append(contactsSet);
        }
    }

    return true;
}

QVector<ContactsSet> ContactsImporter::contacts() const
{
    return mContacts;
}
