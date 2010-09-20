#include "contactstreemodel.h"

#include <kabc/addressee.h>
#include <kabc/phonenumber.h>

#include <kglobal.h>
#include <kicon.h>
#include <kiconloader.h>
#include <klocale.h>

using namespace Akonadi;

class ContactsTreeModel::Private
{
  public:
    Private()
      : mColumns( ContactsTreeModel::Columns() << ContactsTreeModel::FullName ),
        mIconSize( KIconLoader::global()->currentSize( KIconLoader::Small ) )
    {
    }

    Columns mColumns;
    const int mIconSize;
};

ContactsTreeModel::ContactsTreeModel( ChangeRecorder *monitor, QObject *parent )
  : EntityTreeModel( monitor, parent ), d( new Private )
{
}

ContactsTreeModel::~ContactsTreeModel()
{
  delete d;
}

void ContactsTreeModel::setColumns( const Columns &columns )
{
  emit layoutAboutToBeChanged();
  d->mColumns = columns;
  emit layoutChanged();
}

ContactsTreeModel::Columns ContactsTreeModel::columns() const
{
  return d->mColumns;
}

QVariant ContactsTreeModel::entityData( const Item &item, int column, int role ) const
{

  if ( item.mimeType() == KABC::Addressee::mimeType() ) {
    if ( !item.hasPayload<KABC::Addressee>() ) {

      // Pass modeltest
      if ( role == Qt::DisplayRole )
        return item.remoteId();

      return QVariant();
    }

    const KABC::Addressee addressee = item.payload<KABC::Addressee>();

    if ( (role == Qt::DisplayRole) || (role == Qt::EditRole) ) {
      switch ( d->mColumns.at( column ) ) {
        case FullName:
            return addressee.assembledName();
            break;
        case Role:
            return addressee.role();
            break;
        case Organization:
            return addressee.organization();
            break;
        case PreferredEmail:
            return addressee.preferredEmail();
            break;
        case PhoneNumber:
            return addressee.phoneNumber(KABC::PhoneNumber::Work).number();
            break;
        case GivenName:
          return addressee.givenName();
          break;
      }
    }
  }
  return EntityTreeModel::entityData( item, column, role );
}

QVariant ContactsTreeModel::entityData( const Collection &collection, int column, int role ) const
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

int ContactsTreeModel::entityColumnCount( HeaderGroup headerGroup ) const
{
  if ( headerGroup == EntityTreeModel::CollectionTreeHeaders ) {
    return 1;
  } else if ( headerGroup == EntityTreeModel::ItemListHeaders ) {
    return d->mColumns.count();
  } else {
    return EntityTreeModel::entityColumnCount( headerGroup );
  }
}

QVariant ContactsTreeModel::entityHeaderData( int section, Qt::Orientation orientation, int role, HeaderGroup headerGroup ) const
{
  if ( role == Qt::DisplayRole ) {
    if ( orientation == Qt::Horizontal ) {
      if ( headerGroup == EntityTreeModel::CollectionTreeHeaders ) {

        if ( section >= 1 )
          return QVariant();

        switch ( section ) {
          case 0:
            return i18nc( "@title:Sugar Contacts overview", "Contacts" );
            break;
        }
      } else if ( headerGroup == EntityTreeModel::ItemListHeaders ) {
        if ( section < 0 || section >= d->mColumns.count() )
          return QVariant();

        switch ( d->mColumns.at( section ) ) {
        case FullName:
            return i18nc( "@title:column full name of a contact ", "Name" );
            break;
        case Role:
            return i18nc( "@title:column role - role", "Role" );
            break;
        case Organization:
            return i18nc( "@title:column organization - company", "Organization" );
            break;
        case PreferredEmail:
            return i18nc( "@title:column preferred email - email", "Preferred Email" );
            break;
        case PhoneNumber:
            return i18nc( "@title:column phone number - work", "Phone Work" );
            break;
        case GivenName:
            return i18nc( "@title:column given name", "Given Name" );
            break;
        }
      }
    }
  }

  return EntityTreeModel::entityHeaderData( section, orientation, role, headerGroup );
}

#include "contactstreemodel.moc"
