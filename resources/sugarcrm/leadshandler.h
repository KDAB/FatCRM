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

    Akonadi::Item::List itemsFromListEntriesResponse( const TNS__Entry_list &entryList, const Akonadi::Collection &parentCollection );

    void compare( Akonadi::AbstractDifferencesReporter *reporter,
                  const Akonadi::Item &leftItem, const Akonadi::Item &rightItem );

private:
    QString adjustedTime( const QString datetime ) const;

    typedef QHash<QString, AccessorPair*> AccessorHash;
    AccessorHash *mAccessors;
};

#endif /* LEADSHANDLER_H */

