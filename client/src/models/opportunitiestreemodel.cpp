#include "opportunitiestreemodel.h"

#include <kdcrmdata/sugaropportunity.h>

#include <kglobal.h>
#include <kicon.h>
#include <kiconloader.h>
#include <klocale.h>

using namespace Akonadi;

class OpportunitiesTreeModel::Private
{
  public:
    Private()
      : mColumns( OpportunitiesTreeModel::Columns() << OpportunitiesTreeModel::Name ),
        mIconSize( KIconLoader::global()->currentSize( KIconLoader::Small ) )
    {
    }

    Columns mColumns;
    const int mIconSize;
};

OpportunitiesTreeModel::OpportunitiesTreeModel( ChangeRecorder *monitor, QObject *parent )
  : EntityTreeModel( monitor, parent ), d( new Private )
{
}

OpportunitiesTreeModel::~OpportunitiesTreeModel()
{
  delete d;
}

void OpportunitiesTreeModel::setColumns( const Columns &columns )
{
  emit layoutAboutToBeChanged();
  d->mColumns = columns;
  emit layoutChanged();
}

OpportunitiesTreeModel::Columns OpportunitiesTreeModel::columns() const
{
  return d->mColumns;
}

QVariant OpportunitiesTreeModel::entityData( const Item &item, int column, int role ) const
{
  if ( item.mimeType() == SugarOpportunity::mimeType() ) {
    if ( !item.hasPayload<SugarOpportunity>() ) {

      // Pass modeltest
      if ( role == Qt::DisplayRole )
        return item.remoteId();

      return QVariant();
    }

    const SugarOpportunity opportunity = item.payload<SugarOpportunity>();

    if ( (role == Qt::DisplayRole) || (role == Qt::EditRole) ) {
      switch ( d->mColumns.at( column ) ) {
        case Name:
            return opportunity.name();
            break;
        case AccountName:
            return opportunity.accountName();
            break;
        case SalesStage:
            return opportunity.salesStage();
            break;
        case Amount:
            return opportunity.amount();
            break;
        case Close:
            return opportunity.dateClosed();
            break;
        case AssignedTo:
          return opportunity.assignedUserName();
          break;
      }
    }
  }
  return EntityTreeModel::entityData( item, column, role );
}

QVariant OpportunitiesTreeModel::entityData( const Collection &collection, int column, int role ) const
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

int OpportunitiesTreeModel::entityColumnCount( HeaderGroup headerGroup ) const
{
  if ( headerGroup == EntityTreeModel::CollectionTreeHeaders ) {
    return 1;
  } else if ( headerGroup == EntityTreeModel::ItemListHeaders ) {
    return d->mColumns.count();
  } else {
    return EntityTreeModel::entityColumnCount( headerGroup );
  }
}

QVariant OpportunitiesTreeModel::entityHeaderData( int section, Qt::Orientation orientation, int role, HeaderGroup headerGroup ) const
{
  if ( role == Qt::DisplayRole ) {
    if ( orientation == Qt::Horizontal ) {
      if ( headerGroup == EntityTreeModel::CollectionTreeHeaders ) {

        if ( section >= 1 )
          return QVariant();

        switch ( section ) {
          case 0:
            return i18nc( "@title:Sugar Opportunitiess overview", "Opportunities" );
            break;
        }
      } else if ( headerGroup == EntityTreeModel::ItemListHeaders ) {
        if ( section < 0 || section >= d->mColumns.count() )
          return QVariant();

        switch ( d->mColumns.at( section ) ) {
        case Name:
            return i18nc( "@title:column name for the Opportunity", "Name" );
            break;
        case AccountName:
            return i18nc( "@title:column account name", "Account Name" );
            break;
        case SalesStage:
            return i18nc( "@title:column sales stage", "Sales Stage" );
            break;
        case Amount:
            return i18nc( "@title:column amount", "Amount" );
            break;
        case Close:
            return i18nc( "@title:column date closed", "Close" );
            break;
        case AssignedTo:
            return i18nc( "@title:column assigned to name", "Assigned To" );
            break;
        }
      }
    }
  }

  return EntityTreeModel::entityHeaderData( section, orientation, role, headerGroup );
}

#include "opportunitiestreemodel.moc"
