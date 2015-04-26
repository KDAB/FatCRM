#ifndef ACCOUNTSHANDLER_H
#define ACCOUNTSHANDLER_H

#include "modulehandler.h"

class AccessorPair;

template <typename U, typename V> class QHash;

class AccountsHandler : public ModuleHandler
{
public:
    AccountsHandler(SugarSession *session);

    ~AccountsHandler();

    QStringList supportedFields() const;

    Akonadi::Collection handlerCollection() const;

    QString queryStringForListing() const;
    QString orderByForListing() const;
    QStringList selectedFieldsForListing() const;

    bool setEntry(const Akonadi::Item &item);

    Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection);

    void compare(Akonadi::AbstractDifferencesReporter *reporter,
                 const Akonadi::Item &leftItem, const Akonadi::Item &rightItem);

private:
    typedef QHash<QString, AccessorPair *> AccessorHash;
    AccessorHash *mAccessors;
};

#endif /* ACCOUNTSHANDLER_H */

