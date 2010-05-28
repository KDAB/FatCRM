#ifndef CONTACTSHANDLER_H
#define CONTACTSHANDLER_H

#include "modulehandler.h"

class AccessorPair;

template <typename U, typename V> class QHash;

class ContactsHandler : public ModuleHandler
{
public:
    ContactsHandler();

    ~ContactsHandler();

    QStringList supportedFields() const;

    virtual void setDescriptionResult( const TNS__DescribeSObjectResult &description );

    Akonadi::Collection collection() const;

    virtual void listEntries( const TNS__QueryLocator &locator, SforceService* soap );

    virtual bool setEntry( const Akonadi::Item &item, SforceService *soap );

    virtual Akonadi::Item::List itemsFromListEntriesResponse( const TNS__QueryResult &queryResult,
                                                              const Akonadi::Collection &parentCollection );
private:
    typedef QHash<QString, AccessorPair> AccessorHash;
    AccessorHash *mAccessors;
};

#endif
