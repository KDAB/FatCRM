#include "accountsfilterproxymodel.h"

#include "accountstreemodel.h"

#include <kdcrmdata/sugaraccount.h>

#include <akonadi/entitytreemodel.h>

static bool accountMatchesFilter( const SugarAccount &account, const QString &filterString );

using namespace Akonadi;

class AccountsFilterProxyModel::Private
{
  public:
    QString mFilter;
};

AccountsFilterProxyModel::AccountsFilterProxyModel( QObject *parent )
  : QSortFilterProxyModel( parent ), d( new Private )
{
  // account names should be sorted correctly
  setSortLocaleAware( true );
}

AccountsFilterProxyModel::~AccountsFilterProxyModel()
{
  delete d;
}

void AccountsFilterProxyModel::setFilterString( const QString &filter )
{
  d->mFilter = filter;
  invalidateFilter();
}

bool AccountsFilterProxyModel::filterAcceptsRow( int row, const QModelIndex &parent ) const
{
  if ( d->mFilter.isEmpty() )
    return true;

  const QModelIndex index = sourceModel()->index( row, 0, parent );

  const Akonadi::Item item = index.data( Akonadi::EntityTreeModel::ItemRole ).value<Akonadi::Item>();

  if ( item.hasPayload<SugarAccount>() ) {
    const SugarAccount account = item.payload<SugarAccount>();
    return accountMatchesFilter( account, d->mFilter );
  }

  return true;
}

static bool accountMatchesFilter( const SugarAccount &account, const QString &filterString )
{
  if ( account.name().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( account.billingAddressCity().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( account.email1().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( account.billingAddressCountry().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( account.phoneOffice().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  return false;
}

#include "accountsfilterproxymodel.moc"

