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
    void setOffset( int offset );
    int offset() const;
    int deleted() const;
    QString query( const QString &module ) const;

public:
    static ListEntriesScope scopeForAll();
    static ListEntriesScope scopeForUpdatedSince( const QString &timestamp );
    static ListEntriesScope scopeForDeletedSince( const QString &timestamp );

private:
    ListEntriesScope();
    ListEntriesScope( const QString &timestamp );

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
};

#endif
