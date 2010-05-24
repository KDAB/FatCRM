#include "accountstreemodel.h"

#include <kdcrmdata/sugaraccount.h>

#include <kglobal.h>
#include <kicon.h>
#include <kiconloader.h>
#include <klocale.h>

using namespace Akonadi;

class AccountsTreeModel::Private
{
  public:
    Private()
      : mColumns( AccountsTreeModel::Columns() << AccountsTreeModel::Name ),
        mIconSize( KIconLoader::global()->currentSize( KIconLoader::Small ) )
    {
    }

    Columns mColumns;
    const int mIconSize;
};

AccountsTreeModel::AccountsTreeModel( ChangeRecorder *monitor, QObject *parent )
  : EntityTreeModel( monitor, parent ), d( new Private )
{
}

AccountsTreeModel::~AccountsTreeModel()
{
  delete d;
}

void AccountsTreeModel::setColumns( const Columns &columns )
{
  emit layoutAboutToBeChanged();
  d->mColumns = columns;
  emit layoutChanged();
}

AccountsTreeModel::Columns AccountsTreeModel::columns() const
{
  return d->mColumns;
}

QVariant AccountsTreeModel::entityData( const Item &item, int column, int role ) const
{
  if ( item.mimeType() == SugarAccount::mimeType() ) {
    if ( !item.hasPayload<SugarAccount>() ) {

      // Pass modeltest
      if ( role == Qt::DisplayRole )
        return item.remoteId();

      return QVariant();
    }

    const SugarAccount account = item.payload<SugarAccount>();

    if ( (role == Qt::DisplayRole) || (role == Qt::EditRole) ) {
      switch ( d->mColumns.at( column ) ) {
        case Name:
            return account.name();
            break;
        case City:
            return account.billingAddressCity();
            break;
        case Phone:
            return account.phoneOffice();
            break;
        case User:
          return account.createdByName();
          break;
      }
    }
  }
  return EntityTreeModel::entityData( item, column, role );
}

QVariant AccountsTreeModel::entityData( const Collection &collection, int column, int role ) const
{
  if ( role == Qt::DisplayRole ) {
    switch ( column ) {
      case 0:
        return EntityTreeModel::entityData( collection, column, role );
      default:
        return QString(); // pass model test
    }
  }

  return EntityTreeModel::entityData( collection, column, role );
}

int AccountsTreeModel::entityColumnCount( HeaderGroup headerGroup ) const
{
  if ( headerGroup == EntityTreeModel::CollectionTreeHeaders ) {
    return 1;
  } else if ( headerGroup == EntityTreeModel::ItemListHeaders ) {
    return d->mColumns.count();
  } else {
    return EntityTreeModel::entityColumnCount( headerGroup );
  }
}

QVariant AccountsTreeModel::entityHeaderData( int section, Qt::Orientation orientation, int role, HeaderGroup headerGroup ) const
{
  if ( role == Qt::DisplayRole ) {
    if ( orientation == Qt::Horizontal ) {
      if ( headerGroup == EntityTreeModel::CollectionTreeHeaders ) {

        if ( section >= 1 )
          return QVariant();

        switch ( section ) {
          case 0:
            return i18nc( "@title:Sugar Accounts overview", "Accounts" );
            break;
        }
      } else if ( headerGroup == EntityTreeModel::ItemListHeaders ) {
        if ( section < 0 || section >= d->mColumns.count() )
          return QVariant();

        switch ( d->mColumns.at( section ) ) {
        case Name:
            return i18nc( "@title:column name of a company ", "Name" );
            break;
        case City:
            return i18nc( "@title:column city - billing address", "City" );
            break;
        case Phone:
            return i18nc( "@title:column phone number  - office", "Phone" );
            break;
        case User:
            return i18nc( "@title:column created by user", "User" );
            break;
        }
      }
    }
  }

  return EntityTreeModel::entityHeaderData( section, orientation, role, headerGroup );
}

#include "accountstreemodel.moc"
