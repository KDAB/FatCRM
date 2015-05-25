#ifndef CONTACTSIMPORTER_H
#define CONTACTSIMPORTER_H

#include <QVector>
#include <kabc/addressee.h>
#include <kdcrmdata/sugaraccount.h>

class ContactsImporter
{
public:
    ContactsImporter();

    bool importFile(const QString &fileName);

    QVector<KABC::Addressee> contacts() const;
    QVector<SugarAccount> accounts() const;

private:
    QVector<KABC::Addressee> mAddressees;
    QVector<SugarAccount> mAccounts;

};

#endif // CONTACTSIMPORTER_H
