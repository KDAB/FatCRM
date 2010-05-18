#ifndef MODULEHANDLER_H
#define MODULEHANDLER_H

#include <akonadi/item.h>

namespace Akonadi
{
    class Collection;
}

class Sugarsoap;
class TNS__Entry_list;

class ModuleHandler
{
public:
    explicit ModuleHandler( const QString &moduleName );

    virtual ~ModuleHandler();

    QString moduleName() const;

    virtual QStringList supportedFields() const = 0;

    virtual Akonadi::Collection collection() const = 0;

    virtual void listEntries( int offset, Sugarsoap* soap, const QString &sessionId ) = 0;

    virtual bool setEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId ) = 0;

    virtual Akonadi::Item::List itemsFromListEntriesResponse( const TNS__Entry_list &entryList,
                                                              const Akonadi::Collection &parentCollection ) = 0;

protected:
    QString mModuleName;
};

#endif
