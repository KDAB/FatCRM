#include "detailsdialog.h"

#include "details.h"
#include "ui_detailsdialog.h"
#include "referenceddatamodel.h"
#include "sugarclient.h"
#include "clientsettings.h"

#include "kdcrmdata/kdcrmutils.h"

#include <akonadi/collection.h>
#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>

using namespace Akonadi;

class DetailsDialog::Private
{
    DetailsDialog *const q;

public:
    explicit Private(Details *details, DetailsDialog *parent)
        : q(parent), mDetails(details),
          mButtonBox(0), mSaveButton(0)
    {
    }

    void setData(const QMap<QString, QString> data);
    QMap<QString, QString> data() const;

public:
    Ui::DetailsDialog mUi;

    Item mItem;
    Details *mDetails;
    QDialogButtonBox *mButtonBox;
    QPushButton *mSaveButton;

public: // slots
    void saveClicked();
    void dataModified();
    void saveResult(KJob *job);

private:
    QString currentAccountId() const;
    QString currentAssignedToId() const;
    QString currentCampaignId() const;
    QString currentReportsToId() const;
};

// similar with detailswidget
void DetailsDialog::Private::setData(const QMap<QString, QString> data)
{
    mDetails->setData(data, mUi.createdModifiedContainer);
    // Transform the time returned by the server to system time
    // before it is displayed.
    const QString localTime = KDCRMUtils::formatTimestamp(data.value("dateModified"));
    mUi.dateModified->setText(localTime);

    mUi.description->setPlainText((mDetails->type() != Campaign) ?
                                  data.value("description") :
                                  data.value("content"));
}

// similar with detailswidget
QMap<QString, QString> DetailsDialog::Private::data() const
{
    QMap<QString, QString> currentData = mDetails->getData();

    currentData["description"] = mUi.description->toPlainText();
    currentData["content"] = mUi.description->toPlainText();

    // TODO FIXME
    SugarClient *client = dynamic_cast<SugarClient *>(q->parentWidget()->window());
    if (client != 0) {
        const QString accountId = currentAccountId();
        currentData["parentId"] = accountId;
        currentData["accountId"] = accountId;
        currentData["campaignId"] = currentCampaignId();
        const QString assignedToId = currentAssignedToId();
        currentData["assignedUserId"] = assignedToId;
        currentData["assignedToId"] = assignedToId;
        currentData["reportsToId"] = currentReportsToId();
    }

    return currentData;
}

void DetailsDialog::Private::saveClicked()
{
    Item item = mItem;
    mDetails->updateItem(item, data());

    Job *job = 0;
    if (item.isValid()) {
        kDebug() << "Item modify";
        job = new ItemModifyJob(item, q);
    } else {
        kDebug() << "Item create";
        Q_ASSERT(item.parentCollection().isValid());
        job = new ItemCreateJob(item, item.parentCollection(), q);
    }

    QObject::connect(job, SIGNAL(result(KJob*)), q, SLOT(saveResult(KJob*)));
}

void DetailsDialog::Private::dataModified()
{
    mSaveButton->setEnabled(true);
}

void DetailsDialog::Private::saveResult(KJob *job)
{
    kDebug() << "save result=" << job->error();
    if (job->error() != 0) {
        kError() << job->errorText();
        // TODO
        return;
    }
    q->accept();
#if 0
    ItemCreateJob *createJob = qobject_cast<ItemCreateJob *>(job);
    if (createJob != 0) {
        kDebug() << "item" << createJob->item().id() << "created";
        q->setItem(createJob->item());
    }
#endif
}

QString DetailsDialog::Private::currentAccountId() const
{
    if (mDetails->type() != Campaign) {
        const QList<QComboBox *> comboBoxes =  mUi.informationGB->findChildren<QComboBox *>();
        Q_FOREACH (QComboBox *cb, comboBoxes) {
            const QString key = cb->objectName();
            if (key == QLatin1String("parentName") || key == QLatin1String("accountName")) {
                return cb->itemData(cb->currentIndex(), ReferencedDataModel::IdRole).toString();
            }
        }
    }
    return QString();
}

QString DetailsDialog::Private::currentAssignedToId() const
{
    const QList<QComboBox *> comboBoxes =  mUi.informationGB->findChildren<QComboBox *>();
    Q_FOREACH (QComboBox *cb, comboBoxes) {
        const QString key = cb->objectName();
        if (key == QLatin1String("assignedUserName") || key == QLatin1String("assignedTo")) {
            return cb->itemData(cb->currentIndex(), ReferencedDataModel::IdRole).toString();
        }
    }
    return QString();
}

QString DetailsDialog::Private::currentCampaignId() const
{
    if (mDetails->type() != Campaign) {
        const QList<QComboBox *> comboBoxes =  mUi.informationGB->findChildren<QComboBox *>();
        Q_FOREACH (QComboBox *cb, comboBoxes) {
            const QString key = cb->objectName();
            if (key == QLatin1String("campaignName") || key == QLatin1String("campaign")) {
                return cb->itemData(cb->currentIndex(), ReferencedDataModel::IdRole).toString();
            }
        }
    }
    return QString();
}

QString DetailsDialog::Private::currentReportsToId() const
{
    if (mDetails->type() == Contact) {
        const QList<QComboBox *> comboBoxes =  mUi.informationGB->findChildren<QComboBox *>();
        Q_FOREACH (QComboBox *cb, comboBoxes) {
            const QString key = cb->objectName();
            if (key == QLatin1String("reportsTo")) {
                return cb->itemData(cb->currentIndex(), ReferencedDataModel::IdRole).toString();
            }
        }
    }
    return QString();
}

DetailsDialog::DetailsDialog(Details *details, QWidget *parent)
    : QDialog(parent), d(new Private(details, this))
{
    d->mUi.setupUi(this);

    QVBoxLayout *detailsLayout = new QVBoxLayout(d->mUi.detailsContainer);
    detailsLayout->addWidget(details);

    connect(d->mDetails, SIGNAL(modified()), this, SLOT(dataModified()));

    connect(d->mUi.description, SIGNAL(textChanged()), this,  SLOT(dataModified()));

    d->mSaveButton = d->mUi.buttonBox->button(QDialogButtonBox::Save);
    d->mSaveButton->setEnabled(false);
    connect(d->mSaveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));

    QPushButton *cancelButton = d->mUi.buttonBox->button(QDialogButtonBox::Cancel);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    ClientSettings::self()->restoreWindowSize("details", this);
}

DetailsDialog::~DetailsDialog()
{
    ClientSettings::self()->saveWindowSize("details", this);
    delete d;
}

void DetailsDialog::setItem(const Akonadi::Item &item)
{
    d->mItem = item;

    if (item.isValid()) {
        // modify
        d->setData(d->mDetails->data(item));
    } else {
        // create
        d->setData(QMap<QString, QString>());

        // hide creation/modification date/user
        d->mUi.createdModifiedContainer->hide();
    }

    d->mSaveButton->setEnabled(false);
}

void DetailsDialog::updateItem(const Akonadi::Item &item)
{
    if (item == d->mItem) {
        setItem(item);
    }
}

#include "detailsdialog.moc"
