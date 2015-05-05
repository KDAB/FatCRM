#ifndef OPPORTUNITIESHANDLER_H
#define OPPORTUNITIESHANDLER_H

#include "modulehandler.h"

class AccessorPair;

template <typename U, typename V> class QHash;

class OpportunitiesHandler : public ModuleHandler
{
public:
    OpportunitiesHandler(SugarSession *session);

    ~OpportunitiesHandler();

    QStringList supportedFields() const;

    Akonadi::Collection handlerCollection() const;

    bool setEntry(const Akonadi::Item &item);

    QString orderByForListing() const;
    QStringList selectedFieldsForListing() const;

    Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection);

    void compare(Akonadi::AbstractDifferencesReporter *reporter,
                 const Akonadi::Item &leftItem, const Akonadi::Item &rightItem);

private:
    typedef QHash<QString, AccessorPair *> AccessorHash;
    AccessorHash *mAccessors;
};

#endif /* OPPORTUNITIESHANDLER_H */

