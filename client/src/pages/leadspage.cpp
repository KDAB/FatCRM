#include "leadspage.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "sugarclient.h"
#include "details.h"

#include "kdcrmdata/sugarlead.h"

#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

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

void LeadsPage::addItem(const QMap<QString, QString> &data)
{
    Item item;
    details()->updateItem(item, data);

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob(item, collection());
    Q_UNUSED(job);

    clientWindow()->setEnabled(false);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    emit statusMessage(tr("Be patient the data is being saved remotely!..."));
}

QString LeadsPage::reportTitle() const
{
    return tr("List of Leads");
}

