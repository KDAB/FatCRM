#include "contactspage.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"

#include <kabc/addressee.h>
#include <kabc/address.h>

using namespace Akonadi;

ContactsPage::ContactsPage(QWidget *parent)
    : Page(parent, QString(KABC::Addressee::mimeType()), Contact)
{
    setFilter(new FilterProxyModel(Contact, this));
}

ContactsPage::~ContactsPage()
{
}

QString ContactsPage::reportTitle() const
{
    return tr("List of Contacts");
}
