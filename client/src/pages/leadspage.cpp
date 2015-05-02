#include "leadspage.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"

#include "kdcrmdata/sugarlead.h"

#include <QDebug>

using namespace Akonadi;

LeadsPage::LeadsPage(QWidget *parent)
    : Page(parent, QString(SugarLead::mimeType()), Lead)
{
    setFilter(new FilterProxyModel(Lead, this));
}

LeadsPage::~LeadsPage()
{
}

QString LeadsPage::reportTitle() const
{
    return tr("List of Leads");
}

