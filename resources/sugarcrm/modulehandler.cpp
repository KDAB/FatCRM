#include "modulehandler.h"

#include "sugarsoap.h"

#include <KLocale>

#include <QInputDialog>
#include <QStringList>

void ListEntriesScope::setOffset( int offset )
{
    mOffset = offset;
}

int ListEntriesScope::offset() const
{
    return mOffset;
}

int ListEntriesScope::deleted() const
{
    return mGetDeleted ? 1 : 0;
}

QString ListEntriesScope::query( const QString &module ) const
{
    if ( mUpdateTimestamp.isEmpty() ) {
        return QLatin1String( "" );
    }

    return module + QLatin1String( ".date_modified >= '") + mUpdateTimestamp + QLatin1String( "'" );
}

ListEntriesScope ListEntriesScope::scopeForAll()
{
    return ListEntriesScope();
}

ListEntriesScope ListEntriesScope::scopeForUpdatedSince( const QString &timestamp )
{
    return ListEntriesScope( timestamp );
}

ListEntriesScope ListEntriesScope::scopeForDeletedSince( const QString &timestamp )
{
    ListEntriesScope scope( timestamp );
    scope.mGetDeleted = true;
    return scope;
}

ListEntriesScope::ListEntriesScope()
    : mOffset( 0 ),
      mGetDeleted( false )
{
}

ListEntriesScope::ListEntriesScope( const QString &timestamp )
    : mOffset( 0 ),
      mUpdateTimestamp( timestamp ),
      mGetDeleted( false )
{
}


ModuleHandler::ModuleHandler( const QString &moduleName )
    : mModuleName( moduleName )
{
}

ModuleHandler::~ModuleHandler()
{
}

QString ModuleHandler::moduleName() const
{
    return mModuleName;
}

bool ModuleHandler::getEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId )
{
    if ( item.remoteId().isEmpty() ) {
        kError() << "Item remoteId is empty. id=" << item.id();
        return false;
    }

    TNS__Select_fields selectedFields;
    selectedFields.setItems( supportedFields() );

    soap->asyncGet_entry( sessionId, mModuleName, item.remoteId(), selectedFields );
    return true;
}

Akonadi::Item::List ModuleHandler::itemsFromListEntriesResponse( const TNS__Entry_list &entryList, const Akonadi::Collection &parentCollection )
{
    Akonadi::Item::List items;

    Q_FOREACH( const TNS__Entry_value &entry, entryList.items() ) {
        const Akonadi::Item item = itemFromEntry( entry, parentCollection );
        if ( !item.remoteId().isEmpty() ) {
            items << item;
        }
    }

    return items;
}
