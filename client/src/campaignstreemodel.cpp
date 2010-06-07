#include "campaignstreemodel.h"

#include <kdcrmdata/sugarcampaign.h>

#include <kglobal.h>
#include <kicon.h>
#include <kiconloader.h>
#include <klocale.h>

using namespace Akonadi;

class CampaignsTreeModel::Private
{
  public:
    Private()
      : mColumns( CampaignsTreeModel::Columns() << CampaignsTreeModel::Campaign ),
        mIconSize( KIconLoader::global()->currentSize( KIconLoader::Small ) )
    {
    }

    Columns mColumns;
    const int mIconSize;
};

CampaignsTreeModel::CampaignsTreeModel( ChangeRecorder *monitor, QObject *parent )
  : EntityTreeModel( monitor, parent ), d( new Private )
{
}

CampaignsTreeModel::~CampaignsTreeModel()
{
  delete d;
}

void CampaignsTreeModel::setColumns( const Columns &columns )
{
  emit layoutAboutToBeChanged();
  d->mColumns = columns;
  emit layoutChanged();
}

CampaignsTreeModel::Columns CampaignsTreeModel::columns() const
{
  return d->mColumns;
}

QVariant CampaignsTreeModel::entityData( const Item &item, int column, int role ) const
{
  if ( item.mimeType() == SugarCampaign::mimeType() ) {
    if ( !item.hasPayload<SugarCampaign>() ) {

      // Pass modeltest
      if ( role == Qt::DisplayRole )
        return item.remoteId();

      return QVariant();
    }

    const SugarCampaign campaign = item.payload<SugarCampaign>();

    if ( (role == Qt::DisplayRole) || (role == Qt::EditRole) ) {
      switch ( d->mColumns.at( column ) ) {
        case Campaign:
            return campaign.name();
            break;
        case Status:
            return campaign.status();
            break;
        case Type:
            return campaign.campaignType();
            break;
        case EndDate:
            return campaign.endDate();
            break;
        case User:
            return campaign.assignedUserName();
            break;
      }
    }
  }
  return EntityTreeModel::entityData( item, column, role );
}

QVariant CampaignsTreeModel::entityData( const Collection &collection, int column, int role ) const
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

int CampaignsTreeModel::entityColumnCount( HeaderGroup headerGroup ) const
{
  if ( headerGroup == EntityTreeModel::CollectionTreeHeaders ) {
    return 1;
  } else if ( headerGroup == EntityTreeModel::ItemListHeaders ) {
    return d->mColumns.count();
  } else {
    return EntityTreeModel::entityColumnCount( headerGroup );
  }
}

QVariant CampaignsTreeModel::entityHeaderData( int section, Qt::Orientation orientation, int role, HeaderGroup headerGroup ) const
{
  if ( role == Qt::DisplayRole ) {
    if ( orientation == Qt::Horizontal ) {
      if ( headerGroup == EntityTreeModel::CollectionTreeHeaders ) {

        if ( section >= 1 )
          return QVariant();

        switch ( section ) {
          case 0:
            return i18nc( "@title:Sugar Campaigns overview", "Campaigns" );
            break;
        }
      } else if ( headerGroup == EntityTreeModel::ItemListHeaders ) {
        if ( section < 0 || section >= d->mColumns.count() )
          return QVariant();

        switch ( d->mColumns.at( section ) ) {
        case Campaign:
            return i18nc( "@title:column name of a campaign ", "Campaign" );
            break;
        case Status:
            return i18nc( "@title:column status - Campaign status", "Status" );
            break;
        case Type:
            return i18nc( "@title:column type - Campaign type", "Type" );
            break;
        case EndDate:
            return i18nc( "@title:column end date  - Campaign End Date", "End Date" );
            break;
        case User:
            return i18nc( "@title:column Campaign Assigned User Name", "User" );
            break;
        }
      }
    }
  }

  return EntityTreeModel::entityHeaderData( section, orientation, role, headerGroup );
}

#include "campaignstreemodel.moc"

