#ifndef ACCOUNTSHANDLER_H
#define ACCOUNTSHANDLER_H

#include "modulehandler.h"

class AccessorPair;

template <typename U, typename V> class QHash;

class AccountsHandler : public ModuleHandler
{
public:
    AccountsHandler( SugarSession *session );

    ~AccountsHandler();

    QStringList supportedFields() const;

    Akonadi::Collection collection() const;

    void listEntries( const ListEntriesScope &scope );

    bool setEntry( const Akonadi::Item &item );

    Akonadi::Item itemFromEntry( const TNS__Entry_value &entry, const Akonadi::Collection &parentCollection );

    void compare( Akonadi::AbstractDifferencesReporter *reporter,
                  const Akonadi::Item &leftItem, const Akonadi::Item &rightItem );

private:
    QString adjustedTime( const QString datetime ) const;

    typedef QHash<QString, AccessorPair*> AccessorHash;
    AccessorHash *mAccessors;
};

#endif /* ACCOUNTSHANDLER_H */

