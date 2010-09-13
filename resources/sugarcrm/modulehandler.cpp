#include "modulehandler.h"

#include "sugarsoap.h"

#include <KLocale>

#include <QInputDialog>
#include <QStringList>

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
