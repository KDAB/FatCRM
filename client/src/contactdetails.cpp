#include "contactdetails.h"

#include <akonadi/item.h>

#include <kabc/addressee.h>

using namespace Akonadi;

ContactDetails::ContactDetails( QWidget *parent )
    : QWidget( parent )

{
    mUi.setupUi( this );
}

ContactDetails::~ContactDetails()
{
}

void ContactDetails::setItem (const Item &item )
{
    const KABC::Addressee addressee = item.payload<KABC::Addressee>();
    mUi.firstName->setText( addressee.givenName() );
    mUi.lastName->setText( addressee.familyName() );
    // Pending Michel ( complete )
}
