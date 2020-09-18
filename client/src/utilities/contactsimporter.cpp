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

#define QT_NO_CAST_FROM_ASCII
#define QT_NO_CAST_TO_ASCII

#include "contactsimporter.h"
#include "qcsvreader.h"
#include "kdcrmdata/kdcrmfields.h"
#include "kdcrmdata/kdcrmutils.h"
#include "fatcrm_client_debug.h"

#include <QFile>
#include <QLocale>
#include <QString>
#include <QTextCodec>

#include <config-phonenumber.h>

#if USE_PHONENUMBER

#include <phonenumbers/phonenumberutil.h>
#include <phonenumbers/region_code.h>
#include <phonenumbers/geocoding/phonenumber_offline_geocoder.h>
#include <unicode/locid.h>

static QString extractCountryFromPhoneNumber(const QString &phone)
{
    if (phone.isEmpty()) {
        return QString();
    }

    auto phoneUtil = i18n::phonenumbers::PhoneNumberUtil::GetInstance();
    i18n::phonenumbers::PhoneNumber number;
    if (phoneUtil->Parse(phone.toStdString(), i18n::phonenumbers::RegionCode::ZZ(), &number) == i18n::phonenumbers::PhoneNumberUtil::NO_PARSING_ERROR) {
        // use ZZ region to make sure that GetDescriptionForNumber will always return a country name
        const std::string description = i18n::phonenumbers::PhoneNumberOfflineGeocoder().GetDescriptionForNumber(number,
                                                                                                                 icu::Locale("en", "US"),
                                                                                                                 i18n::phonenumbers::RegionCode::ZZ());
        std::size_t startIndex = description.find_last_of(',');
        if (startIndex == std::string::npos)
            startIndex = 0;

        return QString::fromStdString(description.substr(startIndex)).trimmed();
    } else {
        qWarning() << "Fail to parse number" << phone;
    }
    return QString();
}

#endif /* USE_PHONENUMBER */

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
        for (auto it = accountColumns.constBegin(); it != accountColumns.constEnd() ; ++it) {
            QString value = builder.data(row, it.key()).trimmed();
             //qCDebug(FATCRM_CLIENT_LOG) << it.key() << value << "->" << it.value();
            if (it.key() == COLUMN_COUNTRY) {
                value = KDCRMUtils::canonicalCountryName(value);
            }
            if (!value.isEmpty()) {
                accountData.insert(it.value(), value);
            }
        }

        KContacts::Addressee addressee;
        const QString givenName = builder.data(row, 0).trimmed();
        if (!givenName.isEmpty())
            addressee.setGivenName(givenName);

        const QString familyName = builder.data(row, 1).trimmed();
        if (!familyName.isEmpty())
            addressee.setFamilyName(familyName);

        const QString prefix = builder.data(row, 2).trimmed();
        if (!prefix.isEmpty())
            addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-Salutation"), prefix);

        const QString phoneNumber = builder.data(row, 3).trimmed();
        if (!phoneNumber.isEmpty())
            addressee.insertPhoneNumber(KContacts::PhoneNumber(phoneNumber, KContacts::PhoneNumber::Work));

        const QString emailAddress = builder.data(row, 4).trimmed();
        if (!emailAddress.isEmpty())
            addressee.insertEmail(emailAddress, true);

        const QString companyName = builder.data(row, 5).trimmed();
        if (!companyName.isEmpty())
            addressee.setOrganization(companyName);

        KContacts::Address workAddress(KContacts::Address::Work|KContacts::Address::Pref);
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

        QString workCountry = builder.data(row, 10).trimmed();
#if USE_PHONENUMBER
        if (workCountry.isEmpty())
            workCountry = extractCountryFromPhoneNumber(phoneNumber);
#endif
        if (!workCountry.isEmpty())
            workAddress.setCountry(workCountry);

        if (!workAddress.isEmpty())
            addressee.insertAddress(workAddress);

        const QString jobTitle = builder.data(row, 14).trimmed();
        if (!jobTitle.isEmpty())
            addressee.setTitle(jobTitle);

        const QString contactDescription = builder.data(row, 15).trimmed();
        if (!contactDescription.isEmpty())
            addressee.setNote(contactDescription);

        if (accountData.value(KDCRMFields::name()).trimmed().isEmpty()) {
            const QString identifier = ((!givenName.isEmpty() || !familyName.isEmpty()) ? QStringLiteral("%1 %2").arg(givenName, familyName).trimmed() : emailAddress);
            accountData.insert(KDCRMFields::name(), QStringLiteral("%1 (individual)").arg(identifier));
        }

        if (accountData.value(KDCRMFields::billingAddressCity()).isEmpty())
            accountData.insert(KDCRMFields::billingAddressCity(), workCity);

        if (accountData.value(KDCRMFields::billingAddressState()).isEmpty())
            accountData.insert(KDCRMFields::billingAddressState(), workState);

        if (accountData.value(KDCRMFields::billingAddressCountry()).isEmpty())
            accountData.insert(KDCRMFields::billingAddressCountry(), workCountry);

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
