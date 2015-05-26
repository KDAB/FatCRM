/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QFile>
#include <QDebug>
#include <qcsvreader.h>
#include <QTextCodec>

ContactsImporter::ContactsImporter()
{
}

bool ContactsImporter::importFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QCsvStandardBuilder builder;
    QCsvReader reader(&builder);
    reader.setDelimiter(QLatin1Char(','));
    reader.setTextCodec(QTextCodec::codecForName("utf-8"));
    if (!reader.read(&file))
        return false;

    // the column names below must match SugarAccount::setData
    QMap<int, QString> accountColumns;
    accountColumns.insert(5, QLatin1String("name"));
    accountColumns.insert(6, QLatin1String("billingAddressStreet"));
    accountColumns.insert(7, QLatin1String("billingAddressCity"));
    accountColumns.insert(8, QLatin1String("billingAddressPostalcode"));
    accountColumns.insert(9, QLatin1String("billingAddressState"));
    accountColumns.insert(10, QLatin1String("billingAddressCountry"));

    const int rows = builder.rowCount();
    mAccounts.reserve(rows);
    for (int row = 1; row < rows; ++row) { // skip title row
        QMap<QString, QString> accountData;
        QMap<int, QString>::const_iterator it = accountColumns.constBegin();
        for ( ; it != accountColumns.end() ; ++it) {
            const QString value = builder.data(row, it.key());
            //qDebug() << it.key() << value << "->" << it.value();
            if (!value.isEmpty()) {
                accountData.insert(it.value(), value);
            }
        }
        SugarAccount account;
        account.setData(accountData);
        // Fast-path duplicate filtering, more will happen later
        if (!mAccounts.isEmpty() && mAccounts.at(0).isSameAccount(account))
            continue;
        mAccounts.append(account);
    }

    return true;
}

QVector<KABC::Addressee> ContactsImporter::contacts() const
{
    return mAddressees;
}

QVector<SugarAccount> ContactsImporter::accounts() const
{
    return mAccounts;
}
