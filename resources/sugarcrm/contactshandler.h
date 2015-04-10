#ifndef CONTACTSHANDLER_H
#define CONTACTSHANDLER_H

#include "modulehandler.h"

class AccessorPair;

template <typename U, typename V> class QHash;

class ContactsHandler : public ModuleHandler
{
public:
    ContactsHandler(SugarSession *session);

    ~ContactsHandler();

    QStringList supportedFields() const;

    Akonadi::Collection handlerCollection() const;

    void listEntries(const ListEntriesScope &scope);

    bool setEntry(const Akonadi::Item &item);

    Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection);

    bool needBackendChange(const Akonadi::Item &item, const QSet<QByteArray> &modifiedParts) const;

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

    QString adjustedTime(const QString datetime) const;

    typedef QHash<QString, AccessorPair *> AccessorHash;
    AccessorHash *mAccessors;
};

#endif
