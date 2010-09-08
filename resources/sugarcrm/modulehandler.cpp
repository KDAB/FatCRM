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

ModuleHandler::ConflictSolution ModuleHandler::resolveConflict( const Akonadi::Item &localItem, const Akonadi::Item &remoteItem )
{
    kWarning() << "No type specific conflict UI, falling back to primitive choice";

    const QString title = i18nc( "@title:window", "Conflict between local change and server data" );
    const QString label = i18nc( "@label:listbox", "Which item do you want to keep?" );
    const QString labelLocal = i18nc( "@item:inlistbox", "Local Item" );
    const QString labelRemote = i18nc( "@item:inlistbox", "Server Item" );
    const QString labelBoth = i18nc( "@item:inlistbox", "Both Items (create new entry for local change)" );
    const QStringList options = QStringList() << labelLocal << labelRemote << labelBoth;
    const QString choice = QInputDialog::getItem( 0, title, label, options, 0, false );

    if ( choice == labelBoth ) {
        return BothItems;
    } else if ( choice == labelRemote ) {
        return RemoteItem;
    } else {
        return LocalItem;
    }
}
