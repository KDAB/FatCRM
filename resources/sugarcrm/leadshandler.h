#ifndef LEADSHANDLER_H
#define LEADSHANDLER_H

#include "modulehandler.h"

class AccessorPair;

template <typename U, typename V> class QHash;

class LeadsHandler : public ModuleHandler
{
public:
    LeadsHandler();

    ~LeadsHandler();

    QStringList supportedFields() const;

    Akonadi::Collection collection() const;

    void listEntries( int offset, Sugarsoap* soap, const QString &sessionId );

    bool setEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId );

    Akonadi::Item itemFromEntry( const TNS__Entry_value &entry, const Akonadi::Collection &parentCollection );

    void compare( Akonadi::AbstractDifferencesReporter *reporter,
                  const Akonadi::Item &leftItem, const Akonadi::Item &rightItem );

private:
    QString adjustedTime( const QString datetime ) const;
    inline bool isAddressValue( const QString& value ) const
    { return ( isAltAddressValue( value ) || isPrimaryAddressValue( value ) );}
    inline bool isAltAddressValue( const QString& value ) const
    { return value.startsWith( QString( "alt_address_" ) );}
    inline bool isPrimaryAddressValue( const QString& value ) const
    { return value.startsWith( QString( "primary_address_" ) );}

    typedef QHash<QString, AccessorPair*> AccessorHash;
    AccessorHash *mAccessors;
};

#endif /* LEADSHANDLER_H */

