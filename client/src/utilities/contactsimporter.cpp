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
