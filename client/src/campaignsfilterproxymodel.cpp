#include "campaignsfilterproxymodel.h"

#include "campaignstreemodel.h"

#include <kdcrmdata/sugarcampaign.h>

#include <akonadi/entitytreemodel.h>

static bool campaignMatchesFilter( const SugarCampaign &campaign, const QString &filterString );

using namespace Akonadi;

class CampaignsFilterProxyModel::Private
{
  public:
    QString mFilter;
};

CampaignsFilterProxyModel::CampaignsFilterProxyModel( QObject *parent )
  : QSortFilterProxyModel( parent ), d( new Private )
{
  // campaign names should be sorted correctly
  setSortLocaleAware( true );
}

CampaignsFilterProxyModel::~CampaignsFilterProxyModel()
{
  delete d;
}

void CampaignsFilterProxyModel::setFilterString( const QString &filter )
{
  d->mFilter = filter;
  invalidateFilter();
}

bool CampaignsFilterProxyModel::filterAcceptsRow( int row, const QModelIndex &parent ) const
{
  if ( d->mFilter.isEmpty() )
    return true;

  const QModelIndex index = sourceModel()->index( row, 0, parent );

  const Akonadi::Item item = index.data( Akonadi::EntityTreeModel::ItemRole ).value<Akonadi::Item>();

  if ( item.hasPayload<SugarCampaign>() ) {
    const SugarCampaign campaign = item.payload<SugarCampaign>();
    return campaignMatchesFilter( campaign, d->mFilter );
  }

  return true;
}

static bool campaignMatchesFilter( const SugarCampaign &campaign, const QString &filterString )
{
  if ( campaign.name().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( campaign.status().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( campaign.campaignType().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( campaign.endDate().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( campaign.assignedUserName().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  return false;
}

#include "campaignsfilterproxymodel.moc"


