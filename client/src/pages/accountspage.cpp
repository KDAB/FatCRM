#include "accountspage.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "sugarclient.h"

#include "kdcrmdata/sugaraccount.h"

#include <QDebug>

using namespace Akonadi;

AccountsPage::AccountsPage(QWidget *parent)
    : Page(parent, QString(SugarAccount::mimeType()), Account)
{
    setFilter(new FilterProxyModel(Account, this));
}

AccountsPage::~AccountsPage()
{
}

QString AccountsPage::reportTitle() const
{
    return tr("List of accounts");
}
