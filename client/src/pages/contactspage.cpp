#include "contactspage.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "sugarclient.h"

#include <kabc/addressee.h>
#include <kabc/address.h>

#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

using namespace Akonadi;

ContactsPage::ContactsPage(QWidget *parent)
    : Page(parent, QString(KABC::Addressee::mimeType()), Contact)
{
    setupModel();
}

ContactsPage::~ContactsPage()
{
}

void ContactsPage::addItem(const QMap<QString, QString> &data)
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

void ContactsPage::modifyItem(Item &item, const QMap<QString, QString> &data)
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
