#ifndef CAMPAIGNSHANDLER_H
#define CAMPAIGNSHANDLER_H

#include "modulehandler.h"

class AccessorPair;

template <typename U, typename V> class QHash;

class CampaignsHandler : public ModuleHandler
{
public:
    CampaignsHandler();

    ~CampaignsHandler();

    QStringList supportedFields() const;

    Akonadi::Collection collection() const;

    void listEntries( int offset, Sugarsoap* soap, const QString &sessionId );

    bool setEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId );

    Akonadi::Item::List itemsFromListEntriesResponse( const TNS__Entry_list &entryList, const Akonadi::Collection &parentCollection );

private:
    QString adjustedTime( const QString datetime ) const;

    typedef QHash<QString, AccessorPair> AccessorHash;
    AccessorHash *mAccessors;
};


#endif /* CAMPAIGNSHANDLER_H */
