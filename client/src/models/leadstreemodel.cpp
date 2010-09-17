#include "leadstreemodel.h"

#include <kdcrmdata/sugarlead.h>

#include <kglobal.h>
#include <kicon.h>
#include <kiconloader.h>
#include <klocale.h>

using namespace Akonadi;

class LeadsTreeModel::Private
{
  public:
    Private()
      : mColumns( LeadsTreeModel::Columns() << LeadsTreeModel::Name ),
        mIconSize( KIconLoader::global()->currentSize( KIconLoader::Small ) )
    {
    }

    Columns mColumns;
    const int mIconSize;
};

LeadsTreeModel::LeadsTreeModel( ChangeRecorder *monitor, QObject *parent )
  : EntityTreeModel( monitor, parent ), d( new Private )
{
}

LeadsTreeModel::~LeadsTreeModel()
{
  delete d;
}

void LeadsTreeModel::setColumns( const Columns &columns )
{
  emit layoutAboutToBeChanged();
  d->mColumns = columns;
  emit layoutChanged();
}

LeadsTreeModel::Columns LeadsTreeModel::columns() const
{
  return d->mColumns;
}

QVariant LeadsTreeModel::entityData( const Item &item, int column, int role ) const
{
  if ( item.mimeType() == SugarLead::mimeType() ) {
    if ( !item.hasPayload<SugarLead>() ) {

      // Pass modeltest
      if ( role == Qt::DisplayRole )
        return item.remoteId();

      return QVariant();
    }

    const SugarLead lead = item.payload<SugarLead>();

    if ( (role == Qt::DisplayRole) || (role == Qt::EditRole) ) {
      switch ( d->mColumns.at( column ) ) {
        case Name:
            return lead.lastName();
            break;
        case Status:
            return lead.status();
            break;
        case AccountName:
            return lead.accountName();
            break;
        case Email:
            return lead.email1();
            break;
        case User:
            return lead.assignedUserName();
            break;
      }
    }
  }
  return EntityTreeModel::entityData( item, column, role );
}

QVariant LeadsTreeModel::entityData( const Collection &collection, int column, int role ) const
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

int LeadsTreeModel::entityColumnCount( HeaderGroup headerGroup ) const
{
  if ( headerGroup == EntityTreeModel::CollectionTreeHeaders ) {
    return 1;
  } else if ( headerGroup == EntityTreeModel::ItemListHeaders ) {
    return d->mColumns.count();
  } else {
    return EntityTreeModel::entityColumnCount( headerGroup );
  }
}

QVariant LeadsTreeModel::entityHeaderData( int section, Qt::Orientation orientation, int role, HeaderGroup headerGroup ) const
{
  if ( role == Qt::DisplayRole ) {
    if ( orientation == Qt::Horizontal ) {
      if ( headerGroup == EntityTreeModel::CollectionTreeHeaders ) {

        if ( section >= 1 )
          return QVariant();

        switch ( section ) {
          case 0:
            return i18nc( "@title:Sugar Leads overview", "Leads" );
            break;
        }
      } else if ( headerGroup == EntityTreeModel::ItemListHeaders ) {
        if ( section < 0 || section >= d->mColumns.count() )
          return QVariant();

        switch ( d->mColumns.at( section ) ) {
        case Name:
            return i18nc( "@title:column Lead's Full Name", "Name" );
            break;
        case Status:
            return i18nc( "@title:column Lead's Status", "Status" );
            break;
        case AccountName:
            return i18nc( "@title:column Lead's Account Name", "Account Name" );
            break;
        case Email:
            return i18nc( "@title:column Lead's Primary email", "Email" );
            break;
        case User:
            return i18nc( "@title:column The name of the Lead's Assigny", "User" );
            break;
        }
      }
    }
  }

  return EntityTreeModel::entityHeaderData( section, orientation, role, headerGroup );
}

#include "leadstreemodel.moc"

