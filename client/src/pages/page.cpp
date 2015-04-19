#include "page.h"

#include "detailsdialog.h"
#include "detailswidget.h"
#include "enums.h"
#include "referenceddata.h"
#include "sugarclient.h"
#include "reportgenerator.h"

#include "kdcrmdata/sugaraccount.h"
#include "kdcrmdata/sugaropportunity.h"
#include "kdcrmdata/sugarcampaign.h"
#include "kdcrmdata/sugarlead.h"

#include <akonadi/contact/contactstreemodel.h>
#include <akonadi/contact/contactsfilterproxymodel.h>
#include <akonadi/agentmanager.h>
#include <akonadi/collectionfetchjob.h>
#include <akonadi/collectionfetchscope.h>
#include <akonadi/collectionstatistics.h>
#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemdeletejob.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/itemmodifyjob.h>
#include <akonadi/collectionmodifyjob.h>
#include <akonadi/entityannotationsattribute.h>

#include <kabc/addressee.h>
#include <kabc/address.h>

#include <kdebug.h>
#include <QMessageBox>
#include <clientsettings.h>

using namespace Akonadi;

Page::Page(QWidget *parent, const QString &mimeType, DetailsType type)
    : QWidget(parent),
      mMimeType(mimeType),
      mType(type),
      mDetailsWidget(new DetailsWidget(type)),
      mChangeRecorder(new ChangeRecorder(this)),
      mShowDetailsAction(0)
{
    mUi.setupUi(this);
    mUi.splitter->setCollapsible(0, false);
    initialize();
}

Page::~Page()
{
}

QAction *Page::showDetailsAction(const QString &title) const
{
    mShowDetailsAction->setText(title);
    return mShowDetailsAction;
}

bool Page::showsDetails() const
{
    return mShowDetailsAction->isChecked();
}

void Page::showDetails(bool on)
{
    mUi.detailsWidget->setVisible(on);
    if (on) {
        QMetaObject::invokeMethod(this, "slotEnsureDetailsVisible", Qt::QueuedConnection);
    }
    ClientSettings::self()->setShowDetails(typeToString(mType), on);
    mShowDetailsAction->setChecked(on);
    emit showDetailsChanged(on);
}

void Page::slotResourceSelectionChanged(const QByteArray &identifier)
{
    if (mCollection.isValid()) {
        mChangeRecorder->setCollectionMonitored(mCollection, false);
    }

    mCollection = Collection();
    mResourceIdentifier = identifier;

    mDetailsWidget->details()->setResourceIdentifier(identifier);

    /*
     * Look for the wanted collection explicitly by listing all collections
     * of the currently selected resource, filtering by MIME type.
     * include statistics to get the number of items in each collection
     */
    CollectionFetchJob *job = new CollectionFetchJob(Collection::root(), CollectionFetchJob::Recursive);
    job->fetchScope().setResource(identifier);
    job->fetchScope().setContentMimeTypes(QStringList() << mMimeType);
    job->fetchScope().setIncludeStatistics(true);
    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotCollectionFetchResult(KJob*)));

    mUi.reloadPB->setEnabled(false);
}

void Page::slotCollectionFetchResult(KJob *job)
{
    CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob *>(job);

    // look for the collection
    Q_FOREACH (const Collection &collection, fetchJob->collections()) {
        if (collection.remoteId() == typeToString(mType).toLatin1()) {
            mCollection = collection;
            break;
        }
    }

    if (mCollection.isValid()) {
        mUi.newPB->setEnabled(true);
        mUi.reloadPB->setEnabled(true);
        mChangeRecorder->setCollectionMonitored(mCollection, true);

        // if empty, the collection might not have been loaded yet, try synchronizing
        if (mCollection.statistics().count() == 0) {
            AgentManager::self()->synchronizeCollection(mCollection);
        }

        setupModel();
    } else {
        mUi.newPB->setEnabled(false);
        mUi.reloadPB->setEnabled(false);
    }
}

void Page::slotCurrentItemChanged(const QModelIndex &index)
{
    // save previous item if modified
    if (mDetailsWidget && mDetailsWidget->isModified() && mCurrentIndex.isValid()) {
        if (mCurrentIndex == index) // called by the setCurrentIndex below
            return;
        //kDebug() << "going from" << mCurrentIndex << "to" << index;
        if (askSave()) {
            //kDebug() << "Saving" << mCurrentIndex;
            mDetailsWidget->saveData();
        }
    }

    // show the new item
    //kDebug() << "showing new item" << index;
    Item item = mUi.treeView->model()->data(index, EntityTreeModel::ItemRole).value<Item>();
    if (item.isValid()) {
        if (mDetailsWidget != 0) {
            mDetailsWidget->setItem(item);
        }

        mCurrentIndex = mUi.treeView->selectionModel()->currentIndex();
        //kDebug() << "mCurrentIndex=" << mCurrentIndex;
    }
}

void Page::slotNewClicked()
{
    if (mDetailsWidget != 0 && mShowDetailsAction->isChecked()) {
        if (mDetailsWidget->isModified()) {
            if (askSave()) {
                mDetailsWidget->saveData();
            }
        }

        mDetailsWidget->clearFields();
    } else {
        DetailsDialog *dialog = createDetailsDialog();
        Item item;
        item.setParentCollection(mCollection);
        dialog->setItem(item);
        dialog->show();
    }
}

void Page::slotAddItem() // save new item
{
    if (mDetailsWidget != 0) {
        addItem(mDetailsWidget->data());
    }
}

void Page::slotModifyItem() // save modified item
{
    //kDebug() << "saving" << mCurrentIndex;
    Item item = mUi.treeView->model()->data(mCurrentIndex, EntityTreeModel::ItemRole).value<Item>();
    if (item.isValid() && mDetailsWidget != 0) {

        details()->updateItem(item, mDetailsWidget->data());

        // job starts automatically
        ItemModifyJob *job = new ItemModifyJob(item);
        connect(job, SIGNAL(result(KJob *)), this, SLOT(slotModifyJobResult(KJob *)));
    }
}

void Page::slotModifyJobResult(KJob *job)
{
    if (job->error()) {
        emit statusMessage(job->errorString());
    } else {
        emit statusMessage("Item successfully saved");
    }
}

void Page::slotRemoveItem()
{
    const QModelIndex index = mUi.treeView->selectionModel()->currentIndex();
    if (!index.isValid()) {
        return;
    }

    Item item = mUi.treeView->model()->data(index, EntityTreeModel::ItemRole).value<Item>();

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("SugarClient - Delete record"));
    msgBox.setText(QString("The selected item will be removed permanentely!"));
    msgBox.setInformativeText(tr("Are you sure you want to delete it?"));
    msgBox.setStandardButtons(QMessageBox::Yes |
                              QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Cancel) {
        return;
    }

    if (item.isValid()) {
        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemDeleteJob *job = new ItemDeleteJob(item);
        Q_UNUSED(job);
    }
    const QModelIndex newIndex = mUi.treeView->selectionModel()->currentIndex();
    if (!newIndex.isValid()) {
        mUi.removePB->setEnabled(false);
    }

    if (mDetailsWidget != 0) {
        mDetailsWidget->setItem(Item());
    }

    if (mType == Account) {
        removeAccountsData(item);
    } else if (mType == Campaign) {
        removeCampaignsData(item);
    }
}

void Page::slotRowsInserted(const QModelIndex &, int start, int end)
{
    Q_UNUSED(start);
    Q_UNUSED(end);
    if (mUi.treeView->model()->rowCount() == mCollection.statistics().count()) {
        if (mType == Account) {
            addAccountsData();
        } else if (mType == Campaign) {
            addCampaignsData();
        } else if (mType == Contact) {
            addContactsData();
        } else if (mType == Lead) {
            addLeadsData();
        } else if (mType == Opportunity) {
            addOpportunitiesData();
        }
    }
}

void Page::initialize()
{
    mClientWindow = dynamic_cast<SugarClient *>(window());
    mUi.treeView->header()->setResizeMode(QHeaderView::ResizeToContents);

    const QIcon icon = (style() != 0 ? style()->standardIcon(QStyle::SP_BrowserReload, 0, mUi.reloadPB) : QIcon());
    if (!icon.isNull()) {
        mUi.reloadPB->setIcon(icon);
    }
    mUi.reloadPB->setEnabled(false);

    connect(mUi.clearSearchPB, SIGNAL(clicked()),
            this, SLOT(slotResetSearch()));
    connect(mUi.newPB, SIGNAL(clicked()),
            this, SLOT(slotNewClicked()));
    connect(mUi.removePB, SIGNAL(clicked()),
            this, SLOT(slotRemoveItem()));
    connect(mUi.reloadPB, SIGNAL(clicked()),
            this, SLOT(slotReloadCollection()));

    QShortcut* reloadShortcut = new QShortcut(QKeySequence::Refresh, this);
    connect(reloadShortcut, SIGNAL(activated()), this, SLOT(slotReloadCollection()));

    // automatically get the full data when items change
    mChangeRecorder->itemFetchScope().fetchFullPayload(true);
    mChangeRecorder->setMimeTypeMonitored(mMimeType);

    connect(mChangeRecorder, SIGNAL(collectionChanged(Akonadi::Collection)),
            this, SLOT(slotCollectionChanged(Akonadi::Collection)));

    connect(mUi.treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotItemDoubleClicked(QModelIndex)));

    mShowDetailsAction = new QAction(this);
    mShowDetailsAction->setCheckable(true);
    connect(mShowDetailsAction, SIGNAL(toggled(bool)), this, SLOT(showDetails(bool)));

    connect(mDetailsWidget, SIGNAL(modifyItem()), this, SLOT(slotModifyItem()));
    connect(mDetailsWidget, SIGNAL(createItem()), this, SLOT(slotAddItem()));

    QVBoxLayout *detailLayout = new QVBoxLayout(mUi.detailsWidget);
    detailLayout->setMargin(0);
    detailLayout->addWidget(mDetailsWidget);
    showDetails(ClientSettings::self()->showDetails(typeToString(mType)));
}

void Page::setupModel()
{
    Q_ASSERT(mFilter); // must be set by derived class ctor

    ItemsTreeModel *model = new ItemsTreeModel(mType, recorder(), this);

    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel(this);
    filterModel->setSourceModel(model);
    filterModel->addMimeTypeInclusionFilter(mimeType());
    filterModel->setHeaderGroup(EntityTreeModel::ItemListHeaders);

    mFilter->setSourceModel(filterModel);
    mFilter->setSortRole(Qt::EditRole); // to allow custom formatting for dates in DisplayRole
    mUi.treeView->setModel(mFilter);

    connect(mUi.searchLE, SIGNAL(textChanged(QString)),
            mFilter, SLOT(setFilterString(QString)));

    connect(mUi.treeView->model(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(slotRowsInserted(QModelIndex,int,int)));

    connect(mUi.treeView->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));

    connect(mUi.treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,  SLOT(slotCurrentItemChanged(QModelIndex)));
}

Details *Page::details() const
{
    return mDetailsWidget->details();
}

void Page::insertFilterWidget(QWidget *widget)
{
    mUi.verticalLayout->insertWidget(1, widget);
}

void Page::slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
        const QModelIndex index = mUi.treeView->model()->index(row, 0, QModelIndex());
        const Item item = mUi.treeView->model()->data(index, EntityTreeModel::ItemRole).value<Item>();
        emit modelItemChanged(item);
        if (index == mCurrentIndex && mDetailsWidget) {
            mDetailsWidget->setItem(item);
        }
    }

    if (!mClientWindow->isEnabled()) {
        do {
            QApplication::restoreOverrideCursor();
        } while (QApplication::overrideCursor() != 0);
        mClientWindow->setEnabled(true);
    }
}

bool Page::askSave()
{
    QMessageBox msgBox(this);
    msgBox.setText(tr("The current item has been modified."));
    msgBox.setInformativeText(tr("Do you want to save your changes?"));
    msgBox.setStandardButtons(QMessageBox::Save |
                              QMessageBox::Discard);
    msgBox.setDefaultButton(QMessageBox::Save);
    const int ret = msgBox.exec();
    return ret == QMessageBox::Save;
}

void Page::slotSetItem()
{
    mCurrentIndex  = treeView()->selectionModel()->currentIndex();
    if (mCurrentIndex.isValid()) {
        slotCurrentItemChanged(mCurrentIndex);
    } else {
        slotNewClicked();
    }
}

void Page::slotResetSearch()
{
    mUi.searchLE->clear();
}

void Page::slotReloadCollection()
{
    if (mCollection.isValid()) {
        emit synchronizeCollection(mCollection);
    }
}

void Page::slotCollectionChanged(const Akonadi::Collection &collection)
{
    if (mCollection.isValid() && collection == mCollection) {
        mCollection = collection;
    }
}

void Page::slotEnsureDetailsVisible()
{
    if (mShowDetailsAction->isChecked()) {
        QList<int> splitterSizes = mUi.splitter->sizes();
        if (splitterSizes[ 1 ] == 0) {
            splitterSizes[ 1 ] = mUi.splitter->height() / 2;
            mUi.splitter->setSizes(splitterSizes);
        }
    } else {
        mShowDetailsAction->setChecked(true);
    }
}

void Page::slotItemDoubleClicked(const QModelIndex &index)
{
    const Item item = mUi.treeView->model()->data(index, EntityTreeModel::ItemRole).value<Item>();
    if (item.isValid()) {
        DetailsDialog *dialog = createDetailsDialog();
        dialog->setItem(item);
        connect(this, SIGNAL(modelItemChanged(Akonadi::Item)),
                dialog, SLOT(updateItem(Akonadi::Item)));
        dialog->show();
    }
}

void Page::printReport()
{
    ReportGenerator generator;
    generator.generateListReport(mUi.treeView->model(), reportTitle(), this);
}

QString Page::typeToString(const DetailsType &type) const
{
    if (type == Account) {
        return QString("Accounts");
    } else if (type == Opportunity) {
        return QString("Opportunities");
    } else if (type == Lead) {
        return QString("Leads");
    } else if (type == Contact) {
        return QString("Contacts");
    } else if (type == Campaign) {
        return QString("Campaigns");
    } else {
        return QString();
    }
}

DetailsDialog *Page::createDetailsDialog()
{
    Details* details = DetailsWidget::createDetailsForType(mType);
    details->setResourceIdentifier(mResourceIdentifier);
    DetailsDialog *dialog = new DetailsDialog(details, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    return dialog;
}

void Page::addAccountsData()
{
    ReferencedData *accountRefData = ReferencedData::instance(AccountRef);
    ReferencedData *assignedToRefData = ReferencedData::instance(AssignedToRef);
    ReferencedData *accountCountryRefData = ReferencedData::instance(AccountCountryRef);
    for (int i = 0; i <  mUi.treeView->model()->rowCount(); ++i) {
        const QModelIndex index = mUi.treeView->model()->index(i, 0);
        const Item item = mUi.treeView->model()->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarAccount>()) {
            const SugarAccount account = item.payload<SugarAccount>();
            accountRefData->setReferencedData(account.id(), account.name());
            assignedToRefData->setReferencedData(account.assignedUserId(), account.assignedUserName());
            const QString billingCountry = account.billingAddressCountry();
            const QString country = billingCountry.isEmpty() ? account.shippingAddressCountry() : billingCountry;
            // See comment in itemstreemodel.cpp about why this isn't account.id()
            accountCountryRefData->setReferencedData(account.name(), country);
        }
    }
}

void Page::addCampaignsData()
{
    ReferencedData *campaignRefData = ReferencedData::instance(CampaignRef);
    ReferencedData *assignedToRefData = ReferencedData::instance(AssignedToRef);
    for (int i = 0; i <  mUi.treeView->model()->rowCount(); ++i) {
        const QModelIndex index = mUi.treeView->model()->index(i, 0);
        const Item item = mUi.treeView->model()->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarCampaign>()) {
            const SugarCampaign campaign = item.payload<SugarCampaign>();
            campaignRefData->setReferencedData(campaign.id(), campaign.name());
            assignedToRefData->setReferencedData(campaign.assignedUserId(), campaign.assignedUserName());
        }
    }
}

void Page::addContactsData()
{
    ReferencedData *reportsToRefData = ReferencedData::instance(ReportsToRef);
    ReferencedData *assignedToRefData = ReferencedData::instance(AssignedToRef);
    for (int i = 0; i <  mUi.treeView->model()->rowCount(); ++i) {
        const QModelIndex index = mUi.treeView->model()->index(i, 0);
        const Item item = mUi.treeView->model()->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<KABC::Addressee>()) {
            const KABC::Addressee addressee = item.payload<KABC::Addressee>();
            const QString fullName = addressee.givenName() + " " + addressee.familyName();
            reportsToRefData->setReferencedData(addressee.custom("FATCRM", "X-ContactId"), fullName);
            assignedToRefData->setReferencedData(addressee.custom("FATCRM", "X-AssignedUserId"), addressee.custom("FATCRM", "X-AssignedUserName"));
        }
    }
}

void Page::addLeadsData()
{
    ReferencedData *assignedToRefData = ReferencedData::instance(AssignedToRef);
    for (int i = 0; i <  mUi.treeView->model()->rowCount(); ++i) {
        const QModelIndex index = mUi.treeView->model()->index(i, 0);
        const Item item = mUi.treeView->model()->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarLead>()) {
            const SugarLead lead = item.payload<SugarLead>();
            assignedToRefData->setReferencedData(lead.assignedUserId(), lead.assignedUserName());
        }
    }
}

void Page::addOpportunitiesData()
{
    ReferencedData *assignedToRefData = ReferencedData::instance(AssignedToRef);
    for (int i = 0; i <  mUi.treeView->model()->rowCount(); ++i) {
        const QModelIndex index = mUi.treeView->model()->index(i, 0);
        const Item item = mUi.treeView->model()->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarOpportunity>()) {
            const SugarOpportunity opportunity = item.payload<SugarOpportunity>();
            assignedToRefData->setReferencedData(opportunity.assignedUserId(), opportunity.assignedUserName());
        }
    }
}

void Page::removeAccountsData(Akonadi::Item &item)
{
    if (item.hasPayload<SugarAccount>()) {
        const SugarAccount account = item.payload<SugarAccount>();
        ReferencedData *data = ReferencedData::instance(AccountRef);
        data->removeReferencedData(account.id());
    }
}

void Page::removeCampaignsData(Akonadi::Item &item)
{
    if (item.hasPayload<SugarCampaign>()) {
        const SugarCampaign campaign = item.payload<SugarCampaign>();
        ReferencedData *data = ReferencedData::instance(CampaignRef);
        data->removeReferencedData(campaign.id());
    }
}
