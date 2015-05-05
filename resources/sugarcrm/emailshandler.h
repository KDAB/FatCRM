#ifndef EMAILSHANDLER_H
#define EMAILSHANDLER_H

#include "modulehandler.h"
#include <kdcrmdata/sugaremail.h>

class EmailAccessorPair;

template <typename U, typename V> class QHash;

class EmailsHandler : public ModuleHandler
{
public:
    EmailsHandler(SugarSession *session);

    ~EmailsHandler();

    QStringList supportedFields() const;

    Akonadi::Collection handlerCollection() const;

    bool setEntry(const Akonadi::Item &item);

    QString queryStringForListing() const;
    QString orderByForListing() const;
    QStringList selectedFieldsForListing() const;

    Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection);

    void compare(Akonadi::AbstractDifferencesReporter *reporter,
                 const Akonadi::Item &leftItem, const Akonadi::Item &rightItem);

private:
    SugarEmail::AccessorHash mAccessors;
};

#endif

