#ifndef MODULEHANDLER_H
#define MODULEHANDLER_H

#include <akonadi/item.h>

#include <QStringList>

namespace Akonadi
{
    class Collection;
}

class SforceService;
class TNS__DescribeSObjectResult;
class TNS__QueryLocator;
class TNS__QueryResult;

class ModuleHandler
{
public:
    explicit ModuleHandler( const QString &moduleName );

    virtual ~ModuleHandler();

    QString moduleName() const;

    QStringList availableFields() const;

    virtual QStringList supportedFields() const = 0;

    virtual void setDescriptionResult( const TNS__DescribeSObjectResult &description );

    virtual Akonadi::Collection collection() const = 0;

    virtual void listEntries( const TNS__QueryLocator &locator, SforceService* soap ) = 0;

    virtual bool setEntry( const Akonadi::Item &item, SforceService *soap ) = 0;

    virtual Akonadi::Item::List itemsFromListEntriesResponse( const TNS__QueryResult &queryResult,
                                                              const Akonadi::Collection &parentCollection ) = 0;

protected:
    QString mModuleName;

    QStringList mAvailableFields;
};

#endif
