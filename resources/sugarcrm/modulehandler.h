#ifndef MODULEHANDLER_H
#define MODULEHANDLER_H

#include <akonadi/differencesalgorithminterface.h>
#include <akonadi/item.h>

namespace Akonadi
{
    class Collection;
}

class Sugarsoap;
class TNS__Entry_list;
class TNS__Entry_value;

class ListEntriesScope
{
public:
    ListEntriesScope();
    ListEntriesScope( const QString &timestamp );

    bool isUpdateScope() const;

    void setOffset( int offset );

    int offset() const;

    void fetchDeleted();

    int deleted() const;

    QString query( const QString &module ) const;

private:
    int mOffset;
    QString mUpdateTimestamp;
    bool mGetDeleted;
};

class ModuleHandler : public Akonadi::DifferencesAlgorithmInterface
{
public:
    explicit ModuleHandler( const QString &moduleName );

    virtual ~ModuleHandler();

    QString moduleName() const;

    QString latestTimestamp() const;
    void resetLatestTimestamp();

    virtual QStringList supportedFields() const = 0;

    virtual Akonadi::Collection collection() const = 0;

    virtual void listEntries( const ListEntriesScope &scope, Sugarsoap *soap, const QString &sessionId ) = 0;

    virtual bool setEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId ) = 0;

    virtual bool getEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId );

    virtual Akonadi::Item itemFromEntry( const TNS__Entry_value &entry,
                                         const Akonadi::Collection &parentCollection ) = 0;

    Akonadi::Item::List itemsFromListEntriesResponse( const TNS__Entry_list &entryList,
                                                      const Akonadi::Collection &parentCollection );

protected:
    QString mModuleName;
    QString mLatestTimestamp;
};

#endif
