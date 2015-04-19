#include "accountspage.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "sugarclient.h"
#include "details.h"

#include "kdcrmdata/sugaraccount.h"

#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

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
