#ifndef CONTACTSHANDLER_H
#define CONTACTSHANDLER_H

#include "modulehandler.h"

class AccessorPair;

template <typename U, typename V> class QHash;

class ContactsHandler : public ModuleHandler
{
public:
    ContactsHandler();

    ~ContactsHandler();

    Akonadi::Collection collection() const;

    void listEntries( int offset, Sugarsoap* soap, const QString &sessionId );

    bool setEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId );

    Akonadi::Item::List itemsFromListEntriesResponse( const TNS__Entry_list &entryList, const Akonadi::Collection &parentCollection );

private:
    bool isAddressValue( const QString& value ) const;
    bool isAltAddressValue( const QString& value ) const;
    bool isPrimaryAddressValue( const QString& value ) const;
    typedef QHash<QString, AccessorPair> AccessorHash;
    AccessorHash *mAccessors;
};

#endif
