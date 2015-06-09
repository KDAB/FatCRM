/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "page.h"

#include "detailsdialog.h"
#include "detailswidget.h"
#include "enums.h"
#include "referenceddata.h"
#include "reportgenerator.h"
#include "sugarresourcesettings.h"

#include "kdcrmdata/sugaraccount.h"
#include "kdcrmdata/sugaropportunity.h"
#include "kdcrmdata/sugarcampaign.h"
#include "kdcrmdata/sugarlead.h"

#include <akonadi/agentmanager.h>
#include <akonadi/collectionstatistics.h>
#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemdeletejob.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/itemmodifyjob.h>
#include <akonadi/entityannotationsattribute.h>

#include <kabc/addressee.h>
#include <kabc/address.h>

#include <kdebug.h>
#include <QMessageBox>
#include <clientsettings.h>
#include <accountrepository.h>

using namespace Akonadi;

Page::Page(QWidget *parent, const QString &mimeType, DetailsType type)
    : QWidget(parent),
      mMimeType(mimeType),
      mType(type),
      mDetailsWidget(new DetailsWidget(type)),
      mChangeRecorder(new ChangeRecorder(this)),
      mItemsTreeModel(0),
      mShowDetailsAction(0)
{
    mUi.setupUi(this);
    mUi.splitter->setCollapsible(0, false);
    mUi.treeView->setViewName(typeToString(type));
    mUi.treeView->setAlternatingRowColors(true);
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

void Page::openDialog(const QString &id)
{
    const int count = mItemsTreeModel->rowCount();
    for (int i = 0; i < count; ++i) {
        const QModelIndex index = mItemsTreeModel->index(i, 0);
        const Item item = mItemsTreeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.remoteId() == id) {
            DetailsDialog *dialog = createDetailsDialog();
            dialog->setItem(item);
            dialog->show();
        }
    }
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

// Connected to signal resourceSelected() from the mainwindow
void Page::slotResourceSelectionChanged(const QByteArray &identifier)
{
    if (mCollection.isValid()) {
        mChangeRecorder->setCollectionMonitored(mCollection, false);
    }

    mCollection = Collection();
    mResourceIdentifier = identifier;

    mUi.treeView->setModel(0);
    delete mItemsTreeModel;
    mItemsTreeModel = 0;

    retrieveResourceUrl();
    mUi.reloadPB->setEnabled(false);

    // now we wait for the collection manager to find our collection and tell us
}

void Page::setCollection(const Collection &collection)
{
    mCollection = collection;
    updateSupportedFields();

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

void Page::setNotesRepository(NotesRepository *repo)
{
    mNotesRepository = repo;
    mDetailsWidget->details()->setNotesRepository(repo);
}

void Page::setModificationsIgnored(bool b)
{
    mDetailsWidget->setModificationsIgnored(b);
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
    const QMap<QString, QString> data = dataForNewObject();
    if (mDetailsWidget != 0 && mShowDetailsAction->isChecked()) {
        if (mDetailsWidget->isModified()) {
            if (askSave()) {
                mDetailsWidget->saveData();
            }
        }

        mDetailsWidget->clearFields();
        mDetailsWidget->setData(data);
    } else {
        DetailsDialog *dialog = createDetailsDialog();
        Item item;
        item.setParentCollection(mCollection);
        dialog->showNewItem(data, mCollection);
        dialog->show();
    }
}

void Page::slotAddItem() // save new item
{
    if (mDetailsWidget != 0) {
        Item item;
        details()->updateItem(item, mDetailsWidget->data());

        // job starts automatically
        ItemCreateJob *job = new ItemCreateJob(item, mCollection);
        connect(job, SIGNAL(result(KJob *)), this, SLOT(slotCreateJobResult(KJob *)));
    }
}

void Page::slotCreateJobResult(KJob *job)
{
    if (job->error()) {
        emit statusMessage(job->errorString());
    } else {
        emit statusMessage("Item successfully created");
    }
}

void Page::slotModifyItem(const Akonadi::Item &item) // save modified item
{
    // job starts automatically
    ItemModifyJob *job = new ItemModifyJob(item);
    connect(job, SIGNAL(result(KJob *)), this, SLOT(slotModifyJobResult(KJob *)));
}

void Page::slotModifyJobResult(KJob *job)
{
    if (job->error()) {
        emit statusMessage(job->errorString());
    } else {
        emit statusMessage("Item successfully saved");
    }
}

// Item saved in details dialog
void Page::slotItemSaved(const Item &item)
{
    // This relies on the fact that the dialog is modal: the current index can't change meanwhile
    mDetailsWidget->setItem(item);
}

QString Page::reportSubTitle(int count) const
{
    const QString itemsType = typeToTranslatedString(mType);
    const QString desc = mFilter->filterDescription();
    if (desc.isEmpty())
        return i18n("%1 %2", count, itemsType);
    return i18n("%1: %2 %3", desc, count, itemsType);
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

void Page::slotVisibleRowCountChanged()
{
    mUi.itemCountLB->setText(QString("%1 %2").arg(mUi.treeView->model()->rowCount()).arg(typeToTranslatedString(mType)));
}

void Page::slotRowsInserted(const QModelIndex &, int start, int end)
{
    //kDebug() << typeToString(mType) << ": rows inserted from" << start << "to" << end;

    // inserting rows into comboboxes can change the current index, thus marking the data as modified
    emit ignoreModifications(true);
    //kDebug() << "Finished loading" << typeToString(mType);
    switch (mType) {
    case Account:
        addAccountsData(start, end);
        break;
    case Campaign:
        addCampaignsData(start, end);
        break;
    case Contact:
        addContactsData(start, end);
        break;
    case Lead:
        addLeadsData(start, end);
        break;
    case Opportunity:
        addOpportunitiesData(start, end);
        break;
    default: // other objects (like Note) not shown in a Page
        break;
    }
    // Select the first row; looks nicer than empty fields in the details widget.
    //kDebug() << "model has" << mItemsTreeModel->rowCount()
    //         << "rows, we expect" << mCollection.statistics().count();
    if (mItemsTreeModel->rowCount() == mCollection.statistics().count()) {
        if (!mUi.treeView->currentIndex().isValid()) {
            mUi.treeView->setCurrentIndex(mUi.treeView->model()->index(0, 0));
        }
    }
    emit ignoreModifications(false);
}

void Page::initialize()
{
    connect(mUi.treeView, SIGNAL(doubleClicked(Akonadi::Item)), this, SLOT(slotItemDoubleClicked(Akonadi::Item)));

    const QIcon icon = (style() != 0 ? style()->standardIcon(QStyle::SP_BrowserReload, 0, mUi.reloadPB) : QIcon());
    if (!icon.isNull()) {
        mUi.reloadPB->setIcon(icon);
    }
    mUi.reloadPB->setEnabled(false);

    // Removing doesn't work right now, and is a rather dangerous operation anyway :-)
    mUi.removePB->hide();

    // Reloading is already available in the toolbar (and using F5 for just one collection)
    // so unclutter the GUI a bit
    mUi.reloadPB->hide();

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

    connect(mChangeRecorder, SIGNAL(collectionChanged(Akonadi::Collection, QSet<QByteArray>)),
            this, SLOT(slotCollectionChanged(Akonadi::Collection, QSet<QByteArray>)));

    mShowDetailsAction = new QAction(this);
    mShowDetailsAction->setCheckable(true);
    connect(mShowDetailsAction, SIGNAL(toggled(bool)), this, SLOT(showDetails(bool)));

    connect(mDetailsWidget, SIGNAL(modifyItem(Akonadi::Item)), this, SLOT(slotModifyItem(Akonadi::Item)));
    connect(mDetailsWidget, SIGNAL(createItem()), this, SLOT(slotAddItem()));

    QVBoxLayout *detailLayout = new QVBoxLayout(mUi.detailsWidget);
    detailLayout->setMargin(0);
    detailLayout->addWidget(mDetailsWidget);
    showDetails(ClientSettings::self()->showDetails(typeToString(mType)));

    connectToDetails(mDetailsWidget->details());
}

void Page::setupModel()
{
    Q_ASSERT(mFilter); // must be set by derived class ctor

    mItemsTreeModel = new ItemsTreeModel(mType, mChangeRecorder, this);

    connect(mItemsTreeModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(slotRowsInserted(QModelIndex,int,int)));
    connect(mItemsTreeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));

    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel(this);
    filterModel->setSourceModel(mItemsTreeModel);
    filterModel->addMimeTypeInclusionFilter(mMimeType);
    filterModel->setHeaderGroup(EntityTreeModel::ItemListHeaders);

    mFilter->setSourceModel(filterModel);
    mFilter->setSortRole(Qt::EditRole); // to allow custom formatting for dates in DisplayRole
    mUi.treeView->setModels(mFilter, mItemsTreeModel, mItemsTreeModel->defaultVisibleColumns());

    connect(mFilter, SIGNAL(layoutChanged()), this, SLOT(slotVisibleRowCountChanged()));
    connect(mFilter, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(slotVisibleRowCountChanged()));
    connect(mFilter, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(slotVisibleRowCountChanged()));

    connect(mUi.searchLE, SIGNAL(textChanged(QString)),
            mFilter, SLOT(setFilterString(QString)));

    connect(mUi.treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,  SLOT(slotCurrentItemChanged(QModelIndex)));

    emit modelCreated(mItemsTreeModel);
}

Details *Page::details() const
{
    return mDetailsWidget->details();
}

void Page::connectToDetails(Details *details)
{
    connect(details, SIGNAL(openObject(DetailsType,QString)),
            this, SIGNAL(openObject(DetailsType,QString)));
}

void Page::insertFilterWidget(QWidget *widget)
{
    mUi.verticalLayout->insertWidget(1, widget);
}

static QString countryForAccount(const SugarAccount &account)
{
    const QString billingCountry = account.billingAddressCountry();
    const QString country = billingCountry.isEmpty() ? account.shippingAddressCountry() : billingCountry;
    return country.trimmed();
}

void Page::slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    const int start = topLeft.row();
    const int end = bottomRight.row();
    const int firstColumn = topLeft.column();
    const int lastColumn = bottomRight.column();
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = mItemsTreeModel->index(row, 0, QModelIndex());
        const Item item = index.data(EntityTreeModel::ItemRole).value<Item>();
        emit modelItemChanged(item); // update details dialog
        if (index == mCurrentIndex && mDetailsWidget) {
            mDetailsWidget->setItem(item); // update details widget
        }
        if (mType == Account && item.hasPayload<SugarAccount>()) {
            const SugarAccount account = item.payload<SugarAccount>();
            if (firstColumn <= ItemsTreeModel::Country && ItemsTreeModel::Country <= lastColumn) {
                ReferencedData::instance(AccountCountryRef)->setReferencedData(account.name(), countryForAccount(account));
            }
        }
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

// duplicated in listentriesjob.cpp
static const char s_supportedFieldsKey[] = "supportedFields";

void Page::updateSupportedFields()
{
    EntityAnnotationsAttribute *annotationsAttribute =
            mCollection.attribute<EntityAnnotationsAttribute>();
    if (annotationsAttribute) {
        mSupportedFields = annotationsAttribute->value(s_supportedFieldsKey).split(",", QString::SkipEmptyParts);
        //kDebug() << typeToString(mType) << "supported fields" << msupportedFields;
        if (mSupportedFields.isEmpty()) {
            static bool errorShown = false;
            if (!errorShown) {
                errorShown = true;
                QMessageBox::warning(this, i18n("Internal error"), i18n("The list of fields for type '%1'' is not available. Creating new items will not work. Try restarting the CRM resource and synchronizing again (then restart FatCRM).", typeToString(mType)));
            }
        } else {
            mDetailsWidget->details()->setSupportedFields(mSupportedFields);
        }
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

void Page::slotCollectionChanged(const Akonadi::Collection &collection, const QSet<QByteArray> &attributeNames)
{
    if (mCollection.isValid() && collection == mCollection) {
        mCollection = collection;

        if (attributeNames.contains(s_supportedFieldsKey)) {
            updateSupportedFields();
        }
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

void Page::slotItemDoubleClicked(const Akonadi::Item &item)
{
    DetailsDialog *dialog = createDetailsDialog();
    dialog->setItem(item);
    // in case of changes while the dialog is up
    connect(this, SIGNAL(modelItemChanged(Akonadi::Item)),
            dialog, SLOT(updateItem(Akonadi::Item)));
    // show changes made in the dialog
    connect(dialog, SIGNAL(itemSaved(Akonadi::Item)),
            this, SLOT(slotItemSaved(Akonadi::Item)));
    dialog->show();
}

void Page::printReport()
{
    ReportGenerator generator;
    QAbstractItemModel *model = mUi.treeView->model();
    const int count = model->rowCount();
    if (count > 1000) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(i18n("SugarClient - Long report warning"));
        msgBox.setText(i18n("The generated report will contain %1 rows, which might be long to generate and print.", count));
        msgBox.setInformativeText(tr("Are you sure you want to proceed?"));
        msgBox.setStandardButtons(QMessageBox::Yes |
                                  QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Cancel) {
            return;
        }
    }
    generator.generateListReport(model, reportTitle(), reportSubTitle(count), this);
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

QString Page::typeToTranslatedString(const DetailsType &type) const
{
    if (type == Account) {
        return i18n("accounts");
    } else if (type == Opportunity) {
        return i18n("opportunities");
    } else if (type == Lead) {
        return i18n("leads");
    } else if (type == Contact) {
        return i18n("contacts");
    } else if (type == Campaign) {
        return i18n("campaigns");
    } else {
        return QString();
    }
}

DetailsDialog *Page::createDetailsDialog()
{
    Details* details = DetailsWidget::createDetailsForType(mType);
    details->setResourceIdentifier(mResourceIdentifier, mResourceBaseUrl);
    details->setNotesRepository(mNotesRepository);
    details->setSupportedFields(mSupportedFields);
    connectToDetails(details);
    DetailsDialog *dialog = new DetailsDialog(details, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    return dialog;
}

void Page::addAccountsData(int start, int end)
{
    //kDebug(); QElapsedTimer dt; dt.start();
    QMap<QString, QString> accountRefMap, assignedToRefMap, accountCountryRefMap;
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = mItemsTreeModel->index(row, 0);
        const Item item = mItemsTreeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarAccount>()) {
            const SugarAccount account = item.payload<SugarAccount>();
            accountRefMap.insert(account.id(), account.name());
            assignedToRefMap.insert(account.assignedUserId(), account.assignedUserName());
            // See comment in itemstreemodel.cpp about why this isn't account.id()
            accountCountryRefMap.insert(account.name(), countryForAccount(account));

            AccountRepository::instance()->addAccount(account);
        }
    }
    ReferencedData::instance(AccountRef)->addMap(accountRefMap); // TODO detect renamings
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap); // we assume user names don't change later
    ReferencedData::instance(AccountCountryRef)->addMap(accountCountryRefMap); // country changes are handled in slotDataChanged
    //kDebug() << "done," << dt.elapsed() << "ms";
}

void Page::addCampaignsData(int start, int end)
{
    //kDebug(); QElapsedTimer dt; dt.start();
    QMap<QString, QString> campaignRefMap, assignedToRefMap;
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = mItemsTreeModel->index(row, 0);
        const Item item = mItemsTreeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarCampaign>()) {
            const SugarCampaign campaign = item.payload<SugarCampaign>();
            campaignRefMap.insert(campaign.id(), campaign.name());
            assignedToRefMap.insert(campaign.assignedUserId(), campaign.assignedUserName());
        }
    }
    ReferencedData::instance(CampaignRef)->addMap(campaignRefMap);
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap);
    //kDebug() << "done," << dt.elapsed() << "ms";
}

void Page::addContactsData(int start, int end)
{
    //kDebug(); QElapsedTimer dt; dt.start();
    QMap<QString, QString> reportsToRefMap, assignedToRefMap;
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = mItemsTreeModel->index(row, 0);
        const Item item = mItemsTreeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<KABC::Addressee>()) {
            const KABC::Addressee addressee = item.payload<KABC::Addressee>();
            const QString fullName = addressee.givenName() + " " + addressee.familyName();
            reportsToRefMap.insert(addressee.custom("FATCRM", "X-ContactId"), fullName);
            assignedToRefMap.insert(addressee.custom("FATCRM", "X-AssignedUserId"), addressee.custom("FATCRM", "X-AssignedUserName"));
        }
    }
    ReferencedData::instance(ReportsToRef)->addMap(reportsToRefMap); // TODO handle changes in slotDataChanged
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap);
    //kDebug() << "done," << dt.elapsed() << "ms";
}

void Page::addLeadsData(int start, int end)
{
    //kDebug();
    QMap<QString, QString> assignedToRefMap;

    for (int row = start; row <= end; ++row) {
        const QModelIndex index = mItemsTreeModel->index(row, 0);
        const Item item = mItemsTreeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarLead>()) {
            const SugarLead lead = item.payload<SugarLead>();
            assignedToRefMap.insert(lead.assignedUserId(), lead.assignedUserName());
        }
    }
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap);
}

void Page::addOpportunitiesData(int start, int end)
{
    //kDebug();
    QMap<QString, QString> assignedToRefMap;
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = mItemsTreeModel->index(row, 0);
        const Item item = mItemsTreeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarOpportunity>()) {
            const SugarOpportunity opportunity = item.payload<SugarOpportunity>();
            assignedToRefMap.insert(opportunity.assignedUserId(), opportunity.assignedUserName());
        }
    }
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap);
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

void Page::retrieveResourceUrl()
{
    OrgKdeAkonadiSugarCRMSettingsInterface iface(
                QLatin1String("org.freedesktop.Akonadi.Resource.") + mResourceIdentifier, QLatin1String("/Settings"), QDBusConnection::sessionBus() );
    mResourceBaseUrl = iface.host();
    mDetailsWidget->details()->setResourceIdentifier(mResourceIdentifier, mResourceBaseUrl);
}
