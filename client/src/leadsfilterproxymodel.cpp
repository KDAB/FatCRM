#include "leadsfilterproxymodel.h"


#include "leadstreemodel.h"

#include <kdcrmdata/sugarlead.h>

#include <akonadi/entitytreemodel.h>

static bool leadMatchesFilter( const SugarLead &lead, const QString &filterString );

using namespace Akonadi;

class LeadsFilterProxyModel::Private
{
  public:
    QString mFilter;
};

LeadsFilterProxyModel::LeadsFilterProxyModel( QObject *parent )
  : QSortFilterProxyModel( parent ), d( new Private )
{
  // lead names should be sorted correctly
  setSortLocaleAware( true );
}

LeadsFilterProxyModel::~LeadsFilterProxyModel()
{
  delete d;
}

void LeadsFilterProxyModel::setFilterString( const QString &filter )
{
  d->mFilter = filter;
  invalidateFilter();
}

bool LeadsFilterProxyModel::filterAcceptsRow( int row, const QModelIndex &parent ) const
{
  if ( d->mFilter.isEmpty() )
    return true;

  const QModelIndex index = sourceModel()->index( row, 0, parent );

  const Akonadi::Item item = index.data( Akonadi::EntityTreeModel::ItemRole ).value<Akonadi::Item>();

  if ( item.hasPayload<SugarLead>() ) {
    const SugarLead lead = item.payload<SugarLead>();
    return leadMatchesFilter( lead, d->mFilter );
  }

  return true;
}

static bool leadMatchesFilter( const SugarLead &lead, const QString &filterString )
{
  if ( lead.firstName().contains( filterString, Qt::CaseInsensitive ) )
      return true;
  if ( lead.lastName().contains( filterString, Qt::CaseInsensitive ) )
      return true;
  if ( lead.status().contains( filterString, Qt::CaseInsensitive ) )
      return true;
  if ( lead.accountName().contains( filterString, Qt::CaseInsensitive ) )
      return true;
  if ( lead.email1().contains( filterString, Qt::CaseInsensitive ) )
      return true;
  if ( lead.assignedUserName().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  return false;
}

#include "leadsfilterproxymodel.moc"

