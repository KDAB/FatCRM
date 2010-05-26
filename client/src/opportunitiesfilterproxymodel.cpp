#include "opportunitiesfilterproxymodel.h"

#include "opportunitiestreemodel.h"

#include <kdcrmdata/sugaropportunity.h>

#include <akonadi/entitytreemodel.h>

static bool opportunityMatchesFilter( const SugarOpportunity &account, const QString &filterString );

using namespace Akonadi;

class OpportunitiesFilterProxyModel::Private
{
  public:
    QString mFilter;
};

OpportunitiesFilterProxyModel::OpportunitiesFilterProxyModel( QObject *parent )
  : QSortFilterProxyModel( parent ), d( new Private )
{
  // opportunity names should be sorted correctly
  setSortLocaleAware( true );
}

OpportunitiesFilterProxyModel::~OpportunitiesFilterProxyModel()
{
  delete d;
}

void OpportunitiesFilterProxyModel::setFilterString( const QString &filter )
{
  d->mFilter = filter;
  invalidateFilter();
}

bool OpportunitiesFilterProxyModel::filterAcceptsRow( int row, const QModelIndex &parent ) const
{
  if ( d->mFilter.isEmpty() )
    return true;

  const QModelIndex index = sourceModel()->index( row, 0, parent );

  const Akonadi::Item item = index.data( Akonadi::EntityTreeModel::ItemRole ).value<Akonadi::Item>();

  if ( item.hasPayload<SugarOpportunity>() ) {
    const SugarOpportunity opportunity = item.payload<SugarOpportunity>();
    return opportunityMatchesFilter( opportunity, d->mFilter );
  }

  return true;
}

static bool opportunityMatchesFilter( const SugarOpportunity &opportunity, const QString &filterString )
{
  if ( opportunity.name().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( opportunity.accountName().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( opportunity.salesStage().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( opportunity.amount().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( opportunity.dateClosed().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( opportunity.assignedUserName().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  return false;
}

#include "opportunitiesfilterproxymodel.moc"


