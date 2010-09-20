#include "contactsfilterproxymodel.h"

#include "contactstreemodel.h"

#include <kabc/addressee.h>
#include <kabc/phonenumber.h>

#include <akonadi/entitytreemodel.h>

static bool contactMatchesFilter( const KABC::Addressee addressee, const QString &filterString );

using namespace Akonadi;

class ContactsFilterProxyModel::Private
{
  public:
    QString mFilter;
};

ContactsFilterProxyModel::ContactsFilterProxyModel( QObject *parent )
  : QSortFilterProxyModel( parent ), d( new Private )
{
  // contact names should be sorted correctly
  setSortLocaleAware( true );
}

ContactsFilterProxyModel::~ContactsFilterProxyModel()
{
  delete d;
}

void ContactsFilterProxyModel::setFilterString( const QString &filter )
{
  d->mFilter = filter;
  invalidateFilter();
}

bool ContactsFilterProxyModel::filterAcceptsRow( int row, const QModelIndex &parent ) const
{
  if ( d->mFilter.isEmpty() )
    return true;

  const QModelIndex index = sourceModel()->index( row, 0, parent );

  const Akonadi::Item item = index.data( Akonadi::EntityTreeModel::ItemRole ).value<Akonadi::Item>();

  if ( item.hasPayload<KABC::Addressee>() ) {
    const KABC::Addressee contact = item.payload<KABC::Addressee>();
    return contactMatchesFilter( contact, d->mFilter );
  }

  return true;
}

static bool contactMatchesFilter( const KABC::Addressee contact, const QString &filterString )
{
  if ( contact.assembledName().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( contact.role().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( contact.organization().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( contact.preferredEmail().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( contact.phoneNumber(KABC::PhoneNumber::Work).number().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  if ( contact.givenName().contains( filterString, Qt::CaseInsensitive ) )
      return true;

  return false;
}

#include "contactsfilterproxymodel.moc"


