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

    QStringList supportedFields() const;

    Akonadi::Collection collection() const;

    void listEntries( int offset, Sugarsoap* soap, const QString &sessionId );

    bool setEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId );

    Akonadi::Item::List itemsFromListEntriesResponse( const TNS__Entry_list &entryList, const Akonadi::Collection &parentCollection );

private:
    inline bool isAddressValue( const QString& value ) const
    { return ( isAltAddressValue( value ) || isPrimaryAddressValue( value ) );}
    inline bool isAltAddressValue( const QString& value ) const
    { return value.startsWith( QString( "alt_address_" ) );}
    inline bool isPrimaryAddressValue( const QString& value ) const
    { return value.startsWith( QString( "primary_address_" ) );}

    QString adjustedTime( const QString datetime ) const;

    typedef QHash<QString, AccessorPair> AccessorHash;
    AccessorHash *mAccessors;
};

#endif
