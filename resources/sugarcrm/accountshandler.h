#ifndef ACCOUNTSHANDLER_H
#define ACCOUNTSHANDLER_H

#include "modulehandler.h"

class AccessorPair;

template <typename U, typename V> class QHash;

class AccountsHandler : public ModuleHandler
{
public:
    AccountsHandler();

    ~AccountsHandler();

    QStringList supportedFields() const;

    Akonadi::Collection collection() const;

    void listEntries( int offset, Sugarsoap* soap, const QString &sessionId );

    bool setEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId );

    Akonadi::Item::List itemsFromListEntriesResponse( const TNS__Entry_list &entryList, const Akonadi::Collection &parentCollection );

private:
    typedef QHash<QString, AccessorPair> AccessorHash;
    AccessorHash *mAccessors;
};

#endif /* ACCOUNTSHANDLER_H */

