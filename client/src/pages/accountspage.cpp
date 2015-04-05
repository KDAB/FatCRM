#include "accountspage.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "sugarclient.h"

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

void AccountsPage::addItem(const QMap<QString, QString> &data)
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

void AccountsPage::modifyItem(Item &item, const QMap<QString, QString> &data)
{
    details()->updateItem(item, data);

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemModifyJob *job = new ItemModifyJob(item);

    if (!job->exec()) {
        return;    //qDebug() << "Error:" << job->errorString();
    }

    clientWindow()->setEnabled(false);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    emit statusMessage(tr("Be patient the data is being saved remotely!..."));
}
