#ifndef LEADSHANDLER_H
#define LEADSHANDLER_H

#include "modulehandler.h"

class AccessorPair;

template <typename U, typename V> class QHash;

class LeadsHandler : public ModuleHandler
{
public:
    LeadsHandler(SugarSession *session);

    ~LeadsHandler();

    QStringList supportedFields() const;

    Akonadi::Collection handlerCollection() const;

    QString orderByForListing() const;
    QStringList selectedFieldsForListing() const;

    bool setEntry(const Akonadi::Item &item);

    Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection);

    void compare(Akonadi::AbstractDifferencesReporter *reporter,
                 const Akonadi::Item &leftItem, const Akonadi::Item &rightItem);

private:
    inline bool isAddressValue(const QString &value) const
    {
        return (isAltAddressValue(value) || isPrimaryAddressValue(value));
    }
    inline bool isAltAddressValue(const QString &value) const
    {
        return value.startsWith(QString("alt_address_"));
    }
    inline bool isPrimaryAddressValue(const QString &value) const
    {
        return value.startsWith(QString("primary_address_"));
    }

    typedef QHash<QString, AccessorPair *> AccessorHash;
    AccessorHash *mAccessors;
};

#endif /* LEADSHANDLER_H */

