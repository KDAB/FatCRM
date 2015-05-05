#ifndef MODULEHANDLER_H
#define MODULEHANDLER_H

#include <akonadi/differencesalgorithminterface.h>
#include <akonadi/item.h>

namespace Akonadi
{
class Collection;
}

class SugarSession;
class ListEntriesScope;

namespace KDSoapGenerated
{
class Sugarsoap;
class TNS__Entry_list;
class TNS__Entry_value;
}

class ModuleHandler : public Akonadi::DifferencesAlgorithmInterface
{
public:
    explicit ModuleHandler(const QString &moduleName, SugarSession *session);

    virtual ~ModuleHandler();

    QString moduleName() const;

    QString latestTimestamp() const;
    void resetLatestTimestamp();

    virtual QStringList supportedFields() const = 0;

    Akonadi::Collection collection() const;

    void getEntriesCount(const ListEntriesScope &scope);
    void listEntries(const ListEntriesScope &scope);

    virtual bool setEntry(const Akonadi::Item &item) = 0;

    bool getEntry(const Akonadi::Item &item);

    virtual QString queryStringForListing() const { return QString(); }
    virtual QString orderByForListing() const = 0;
    virtual QStringList selectedFieldsForListing() const = 0;

    virtual Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry,
                                        const Akonadi::Collection &parentCollection) = 0;

    Akonadi::Item::List itemsFromListEntriesResponse(const KDSoapGenerated::TNS__Entry_list &entryList,
            const Akonadi::Collection &parentCollection);

    virtual bool needBackendChange(const Akonadi::Item &item, const QSet<QByteArray> &modifiedParts) const;

protected:
    SugarSession *mSession;
    QString mModuleName;
    QString mLatestTimestamp;

protected:
    static QString formatDate(const QString &dateString);
    static QByteArray partIdFromPayloadPart(const char *part);

    virtual Akonadi::Collection handlerCollection() const = 0;

    QString sessionId() const;
    KDSoapGenerated::Sugarsoap *soap() const;
};

#endif
