/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "accountrepository.h"
#include "clientsettings.h"
#include "detailswidget.h"
#include "enums.h"
#include "referenceddata.h"
#include "reportgenerator.h"
#include "sugarresourcesettings.h"
#include "rearrangecolumnsproxymodel.h"

#include "kdcrmdata/enumdefinitionattribute.h"
#include "kdcrmdata/sugaraccount.h"
#include "kdcrmdata/sugaropportunity.h"
#include "kdcrmdata/sugarcampaign.h"
#include "kdcrmdata/sugarlead.h"

#include <Akonadi/AgentManager>
#include <Akonadi/ChangeRecorder>
#include <Akonadi/CollectionModifyJob>
#include <Akonadi/CollectionStatistics>
#include <Akonadi/EntityMimeTypeFilterModel>
#include <Akonadi/Item>
#include <Akonadi/ItemCreateJob>
#include <Akonadi/ItemDeleteJob>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/ItemModifyJob>
#include <Akonadi/EntityAnnotationsAttribute>

#include <KABC/Address>
#include <KABC/Addressee>

#include <KDebug>

#include <QClipboard>
#include <QDesktopServices>
#include <QMenu>
#include <QMessageBox>
#include <QShortcut>

using namespace Akonadi;

Page::Page(QWidget *parent, const QString &mimeType, DetailsType type)
    : QWidget(parent),
      mMimeType(mimeType),
      mType(type),
      mDetailsWidget(new DetailsWidget(type)),
      mChangeRecorder(0),
      mItemsTreeModel(0),
      mShowDetailsAction(0),
      mOnline(false),
      mInitialLoadingDone(false),
      mFilterModel(0)
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
        if (mDetailsWidget->details()->idForItem(item) == id) {
            DetailsDialog *dialog = createDetailsDialog();
            dialog->setItem(item);
            dialog->show();
            // cppcheck-suppress memleak as dialog deletes itself
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

void Page::setFilter(FilterProxyModel *filter)
{
    mFilter = filter;

    mFilter->setSortRole(Qt::EditRole); // to allow custom formatting for dates in DisplayRole
    connect(mFilter, SIGNAL(layoutChanged()), this, SLOT(slotVisibleRowCountChanged()));
    connect(mFilter, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(slotVisibleRowCountChanged()));
    connect(mFilter, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(slotVisibleRowCountChanged()));

    connect(mUi.searchLE, SIGNAL(textChanged(QString)),
            mFilter, SLOT(setFilterString(QString)));
}

// Connected to signal resourceSelected() from the mainwindow
void Page::slotResourceSelectionChanged(const QByteArray &identifier)
{
    delete mChangeRecorder;
    mChangeRecorder = 0;
    mCollection = Collection();
    mResourceIdentifier = identifier;
    mCurrentIndex = QModelIndex();

    // cleanup from last time (useful when switching resources)
    mFilter->setSourceModel(0);
    delete mFilterModel;
    mFilterModel = 0;
    mUi.treeView->setModel(0);
    delete mItemsTreeModel;
    mItemsTreeModel = 0;

    retrieveResourceUrl();
    mUi.reloadPB->setEnabled(false);

    mInitialLoadingDone = false;

    // now we wait for the collection manager to find our collection and tell us
}

void Page::slotOnlineStatusChanged(bool online)
{
    mOnline = online;
    emit onlineStatusChanged(online);
    if (online) {
        retrieveResourceUrl();
    }
}

void Page::setCollection(const Collection &collection)
{
    mCollection = collection;
    readSupportedFields();
    readEnumDefinitionAttributes();

    if (mCollection.isValid()) {
        mUi.newPB->setEnabled(true);
        mUi.reloadPB->setEnabled(true);

        mChangeRecorder = new ChangeRecorder(this);
        mChangeRecorder->setCollectionMonitored(mCollection, true);
        // automatically get the full data when items change
        mChangeRecorder->itemFetchScope().fetchFullPayload(true);
        // don't get remote id/rev, to avoid errors in the FATCRM-75 case
        mChangeRecorder->itemFetchScope().setFetchRemoteIdentification(false);
        mChangeRecorder->setMimeTypeMonitored(mMimeType);
        connect(mChangeRecorder, SIGNAL(collectionChanged(Akonadi::Collection,QSet<QByteArray>)),
                this, SLOT(slotCollectionChanged(Akonadi::Collection,QSet<QByteArray>)));
        connect(mChangeRecorder, SIGNAL(itemChanged(Akonadi::Item,QSet<QByteArray>)),
                this, SLOT(slotItemChanged(Akonadi::Item,QSet<QByteArray>)));

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

bool Page::queryClose()
{
    foreach (DetailsDialog *dialog, mDetailsDialogs) {
        if (dialog->isModified()) {
            if (QMessageBox::Yes == QMessageBox::question(this, i18n("Close?"),
                                                          i18n("A currently opened dialog has modifications, are you sure you want to exit without saving?"),
                                                          QMessageBox::Yes|QMessageBox::No))
            {
                dialog->close();
            } else {
                dialog->raise();
                dialog->activateWindow();
                return false;
            }
        }
    }
    return true;
}

bool Page::hasModifications() const
{
    return mDetailsWidget->isModified();
}

void Page::setModificationsIgnored(bool b)
{
    mDetailsWidget->setModificationsIgnored(b);
}

void Page::initialLoadingDone()
{
    mDetailsWidget->initialLoadingDone();
}

void Page::slotCurrentItemChanged(const QModelIndex &index)
{
    // save previous item if modified
    if (mShowDetailsAction->isChecked() && mDetailsWidget->isModified() && mCurrentIndex.isValid()) {
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
        mDetailsWidget->setItem(item);

        mCurrentIndex = mUi.treeView->selectionModel()->currentIndex();
        //kDebug() << "mCurrentIndex=" << mCurrentIndex;
    }
}

void Page::slotNewClicked()
{
    const QMap<QString, QString> data = dataForNewObject();
    if (mShowDetailsAction->isChecked()) {
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
        // cppcheck-suppress memleak as dialog deletes itself
    }
}

void Page::slotAddItem() // save new item
{
    Item item;
    details()->updateItem(item, mDetailsWidget->data());

    // job starts automatically
    ItemCreateJob *job = new ItemCreateJob(item, mCollection);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(slotCreateJobResult(KJob*)));
}

void Page::slotCreateJobResult(KJob *job)
{
    if (job->error()) {
        emit statusMessage(job->errorString());
    } else {
        emit statusMessage("Item successfully created");
    }
}

void Page::slotModifyItem(const Akonadi::Item &item) // save modified item (from details widget)
{
    // job starts automatically
    ItemModifyJob *job = new ItemModifyJob(item);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(slotModifyJobResult(KJob*)));
}

void Page::slotModifyJobResult(KJob *job) // saving done (from details widget)
{
    if (job->error()) {
        emit statusMessage(job->errorString());
    } else {
        ItemModifyJob *imJob = static_cast<ItemModifyJob *>(job);
        // As documented in ItemModifyJob, store revision of saved item
        mDetailsWidget->setItemRevision(imJob->item());
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
    msgBox.setWindowTitle(i18n("Delete record"));
    msgBox.setText(QString("The selected item will be removed permanentely!"));
    msgBox.setInformativeText(i18n("Are you sure you want to delete it?"));
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

    mDetailsWidget->setItem(Item());
}

void Page::slotVisibleRowCountChanged()
{
    if (mUi.treeView->model()) {
        mUi.itemCountLB->setText(QString("%1 %2").arg(mUi.treeView->model()->rowCount()).arg(typeToTranslatedString(mType)));
    }
}

void Page::slotRowsInserted(const QModelIndex &, int start, int end)
{
    //kDebug() << typeToString(mType) << ": rows inserted from" << start << "to" << end;

    // inserting rows into comboboxes can change the current index, thus marking the data as modified
    emit ignoreModifications(true);

    const bool emitChanges = mInitialLoadingDone;

    switch (mType) {
    case Account:
        addAccountsData(start, end, emitChanges);
        break;
    case Campaign:
        addCampaignsData(start, end, emitChanges);
        break;
    case Contact:
        addContactsData(start, end, emitChanges);
        break;
    case Lead:
        addLeadsData(start, end, emitChanges);
        break;
    case Opportunity:
        addOpportunitiesData(start, end, emitChanges);
        break;
    default: // other objects (like Note) not shown in a Page
        break;
    }
    // Select the first row; looks nicer than empty fields in the details widget.
    //kDebug() << "model has" << mItemsTreeModel->rowCount()
    //         << "rows, we expect" << mCollection.statistics().count();
    const bool done = !mInitialLoadingDone && mItemsTreeModel->rowCount() == mCollection.statistics().count();
    if (done) {
        //kDebug() << "Finished loading" << typeToString(mType);
        if (!mUi.treeView->currentIndex().isValid()) {
            mUi.treeView->setCurrentIndex(mUi.treeView->model()->index(0, 0));
        }
        mInitialLoadingDone = true;
        // Move to the next model
        //emit modelLoaded(mType, i18n("%1 %2 loaded", mItemsTreeModel->rowCount(), typeToString(mType)));
        emit modelLoaded(mType);

        if (mType == Account) {
            AccountRepository::instance()->emitInitialLoadingDone();
        }
    }
    emit ignoreModifications(false);
}

void Page::slotRowsAboutToBeRemoved(const QModelIndex &, int start, int end)
{
    // inserting rows into comboboxes can change the current index, thus marking the data as modified
    emit ignoreModifications(true);

    switch (mType) {
    case Account:
        removeAccountsData(start, end, mInitialLoadingDone);
        break;
    case Campaign:
        removeCampaignsData(start, end, mInitialLoadingDone);
        break;
    case Contact:
        removeContactsData(start, end, mInitialLoadingDone);
        break;
    case Lead:
        removeLeadsData(start, end, mInitialLoadingDone);
        break;
    case Opportunity:
        removeOpportunitiesData(start, end, mInitialLoadingDone);
        break;
    default: // other objects (like Note) not shown in a Page
        break;
    }
    emit ignoreModifications(false);
}

void Page::initialize()
{
    connect(mUi.treeView, SIGNAL(doubleClicked(Akonadi::Item)), this, SLOT(slotItemDoubleClicked(Akonadi::Item)));
    connect(mUi.treeView, SIGNAL(returnPressed(Akonadi::Item)), this, SLOT(slotItemDoubleClicked(Akonadi::Item)));
    connect(mUi.treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotItemContextMenuRequested(QPoint)));

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

void Page::slotItemContextMenuRequested(const QPoint &pos)
{
    mCurrentItemUrl = details()->itemUrl();

    if (!mCurrentItemUrl.isValid())
        return;

    QMenu contextMenu;
    contextMenu.addAction(i18n("Open in &Web Browser"), this, SLOT(slotOpenUrl()));
    contextMenu.addAction(i18n("Copy &Link Location"), this, SLOT(slotCopyLink()));
    contextMenu.exec(mUi.treeView->mapToGlobal(pos));
}

void Page::slotOpenUrl()
{
    QDesktopServices::openUrl(mCurrentItemUrl);
}

void Page::slotCopyLink()
{
    QApplication::clipboard()->setText(mCurrentItemUrl.toString());
}

void Page::setupModel()
{
    Q_ASSERT(mFilter); // must be set by derived class ctor

    Q_ASSERT(!mItemsTreeModel);
    mItemsTreeModel = new ItemsTreeModel(mType, mChangeRecorder, this);

    connect(mItemsTreeModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(slotRowsInserted(QModelIndex,int,int)));
    connect(mItemsTreeModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(slotRowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(mItemsTreeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));

    mFilterModel = new EntityMimeTypeFilterModel(this);
    mFilterModel->setSourceModel(mItemsTreeModel);
    mFilterModel->addMimeTypeInclusionFilter(mMimeType);
    mFilterModel->setHeaderGroup(EntityTreeModel::ItemListHeaders);

    mFilter->setSourceModel(mFilterModel);
    mUi.treeView->setModels(mFilter, mItemsTreeModel, mItemsTreeModel->defaultVisibleColumns());

    connect(mUi.treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,  SLOT(slotCurrentItemChanged(QModelIndex)));

    emit modelCreated(mItemsTreeModel); // give it to the reports page
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

void Page::slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    //kDebug() << typeToString(mType) << topLeft << bottomRight;
    const int start = topLeft.row();
    const int end = bottomRight.row();
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = mItemsTreeModel->index(row, 0, QModelIndex());
        if (!index.isValid()) {
            kWarning() << "Invalid index:" << "row=" << row << "/" << mItemsTreeModel->rowCount();
            return;
        }
        const Item item = index.data(EntityTreeModel::ItemRole).value<Item>();
        Q_ASSERT(item.isValid());
        emit modelItemChanged(item); // update details dialog
        if (index == mCurrentIndex && !mDetailsWidget->isModified()) {
            mDetailsWidget->setItem(item); // update details widget
        }
    }
}

bool Page::askSave()
{
    QMessageBox msgBox(this);
    msgBox.setText(i18n("The current item has been modified."));
    msgBox.setInformativeText(i18n("Do you want to save your changes?"));
    msgBox.setStandardButtons(QMessageBox::Save |
                              QMessageBox::Discard);
    msgBox.setDefaultButton(QMessageBox::Save);
    const int ret = msgBox.exec();
    return ret == QMessageBox::Save;
}

// duplicated in listentriesjob.cpp
static const char s_supportedFieldsKey[] = "supportedFields";

void Page::readSupportedFields()
{
    EntityAnnotationsAttribute *annotationsAttribute =
            mCollection.attribute<EntityAnnotationsAttribute>();
    if (annotationsAttribute) {
        mSupportedFields = annotationsAttribute->value(s_supportedFieldsKey).split(',', QString::SkipEmptyParts);
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

void Page::readEnumDefinitionAttributes()
{
    EnumDefinitionAttribute *enumsAttr = mCollection.attribute<EnumDefinitionAttribute>();
    if (enumsAttr) {
        mEnumDefinitions = EnumDefinitions::fromString(enumsAttr->value());
        mDetailsWidget->details()->setEnumDefinitions(mEnumDefinitions);
    } else {
        kWarning() << "No EnumDefinitions in collection attribute for" << mCollection.id() << mCollection.name();
        kWarning() << "Collection attributes:";
        foreach (Akonadi::Attribute *attr, mCollection.attributes()) {
            kWarning() << attr->type();
        }

        static bool errorShown = false;
        if (!errorShown) {
            errorShown = true;
            QMessageBox::warning(this, i18n("Internal error"), i18n("The list of enumeration values for type '%1'' is not available. Comboboxes will be empty. Try restarting the CRM resource and synchronizing again, making sure at least one update is fetched (then restart FatCRM).", typeToString(mType)));
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
            readSupportedFields();
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

// triggered on double-click and Key_Return
void Page::slotItemDoubleClicked(const Akonadi::Item &item)
{
    DetailsDialog *dialog = createDetailsDialog();
    dialog->setItem(item);
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
        msgBox.setWindowTitle(i18n("Long report warning"));
        msgBox.setText(i18n("The generated report will contain %1 rows, which might be long to generate and print.", count));
        msgBox.setInformativeText(i18n("Are you sure you want to proceed?"));
        msgBox.setStandardButtons(QMessageBox::Yes |
                                  QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Cancel) {
            return;
        }
    }

    // Take care of hidden and reordered columns
    QHeaderView *headerView = mUi.treeView->header();
    QVector<int> sourceColumns;
    sourceColumns.reserve(headerView->count());
    for (int col = 0; col < headerView->count(); ++col) {
        const int logicalColumn = headerView->logicalIndex(col);
        if (!headerView->isSectionHidden(logicalColumn)) {
            sourceColumns.append(logicalColumn);
        }
    }

    RearrangeColumnsProxyModel proxy;
    proxy.setSourceColumns(sourceColumns);
    proxy.setSourceModel(model);
    generator.generateListReport(&proxy, reportTitle(), reportSubTitle(count), this);
}


DetailsDialog *Page::createDetailsDialog()
{
    Details* details = DetailsWidget::createDetailsForType(mType);
    details->setResourceIdentifier(mResourceIdentifier, mResourceBaseUrl);
    details->setNotesRepository(mNotesRepository);
    details->setSupportedFields(mSupportedFields);
    details->setEnumDefinitions(mEnumDefinitions);
    connectToDetails(details);
    // Don't set a parent, so that the dialogs can be minimized/restored independently
    DetailsDialog *dialog = new DetailsDialog(details);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setOnline(mOnline);
    // in case of changes while the dialog is up
    connect(this, SIGNAL(modelItemChanged(Akonadi::Item)),
            dialog, SLOT(updateItem(Akonadi::Item)));
    connect(this, SIGNAL(onlineStatusChanged(bool)),
            dialog, SLOT(setOnline(bool)));
    connect(dialog, SIGNAL(closing()),
            this, SLOT(slotUnregisterDetailsDialog()));
    mDetailsDialogs.insert(dialog);
    return dialog;
}

void Page::slotUnregisterDetailsDialog()
{
    DetailsDialog *dialog = qobject_cast<DetailsDialog*>(sender());
    mDetailsDialogs.remove(dialog);
}

void Page::addAccountsData(int start, int end, bool emitChanges)
{
    //kDebug() << start << end;
    // QElapsedTimer dt; dt.start();
    QMap<QString, QString> accountRefMap, assignedToRefMap;
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = mItemsTreeModel->index(row, 0);
        const Item item = mItemsTreeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarAccount>()) {
            const SugarAccount account = item.payload<SugarAccount>();
            const QString accountId = account.id();
            if (accountId.isEmpty()) // it just got created on the client side, we'll wait for the server to assign it an ID
                continue;
            accountRefMap.insert(accountId, account.name());
            assignedToRefMap.insert(account.assignedUserId(), account.assignedUserName());
            AccountRepository::instance()->addAccount(account, item.id());
        }
    }
    ReferencedData::instance(AccountRef)->addMap(accountRefMap, emitChanges); // renamings are handled in slotDataChanged
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap, emitChanges); // we assume user names don't change later
    //kDebug() << "done," << dt.elapsed() << "ms";
}

void Page::removeAccountsData(int start, int end, bool emitChanges)
{
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = mItemsTreeModel->index(row, 0);
        const Item item = mItemsTreeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarAccount>()) {
            const SugarAccount account = item.payload<SugarAccount>();
            ReferencedData::instance(AccountRef)->removeReferencedData(account.id(), emitChanges);

            AccountRepository::instance()->removeAccount(account);
        }
    }
}

void Page::addCampaignsData(int start, int end, bool emitChanges)
{
    //kDebug(); QElapsedTimer dt; dt.start();
    //QMap<QString, QString> campaignRefMap;
    QMap<QString, QString> assignedToRefMap;
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = mItemsTreeModel->index(row, 0);
        const Item item = mItemsTreeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarCampaign>()) {
            const SugarCampaign campaign = item.payload<SugarCampaign>();
            //campaignRefMap.insert(campaign.id(), campaign.name());
            assignedToRefMap.insert(campaign.assignedUserId(), campaign.assignedUserName());
        }
    }
    //ReferencedData::instance(CampaignRef)->addMap(campaignRefMap, emitChanges);
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap, emitChanges);
    //kDebug() << "done," << dt.elapsed() << "ms";
}

void Page::removeCampaignsData(int start, int end, bool emitChanges)
{
    Q_UNUSED(start)
    Q_UNUSED(end)
    Q_UNUSED(emitChanges)
}

void Page::addContactsData(int start, int end, bool emitChanges)
{
    //kDebug(); QElapsedTimer dt; dt.start();
    QMap<QString, QString> contactRefMap, assignedToRefMap;
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = mItemsTreeModel->index(row, 0);
        const Item item = mItemsTreeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<KABC::Addressee>()) {
            const KABC::Addressee addressee = item.payload<KABC::Addressee>();
            const QString id = addressee.custom("FATCRM", "X-ContactId");
            if (id.isEmpty()) { // newly created, not ID yet
                continue;
            }
            const QString fullName = addressee.givenName() + ' ' + addressee.familyName();
            contactRefMap.insert(id, fullName);
            assignedToRefMap.insert(addressee.custom("FATCRM", "X-AssignedUserId"), addressee.custom("FATCRM", "X-AssignedUserName"));
        }
    }
    ReferencedData::instance(ContactRef)->addMap(contactRefMap, emitChanges);
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap, emitChanges);
    //kDebug() << "done," << dt.elapsed() << "ms";
}

void Page::removeContactsData(int start, int end, bool emitChanges)
{
    Q_UNUSED(start)
    Q_UNUSED(end)
    Q_UNUSED(emitChanges)
}

void Page::addLeadsData(int start, int end, bool emitChanges)
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
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap, emitChanges);
}

void Page::removeLeadsData(int start, int end, bool emitChanges)
{
    Q_UNUSED(start)
    Q_UNUSED(end)
    Q_UNUSED(emitChanges)
}

void Page::addOpportunitiesData(int start, int end, bool emitChanges)
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
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap, emitChanges);
}

void Page::removeOpportunitiesData(int start, int end, bool emitChanges)
{
    Q_UNUSED(start)
    Q_UNUSED(end)
    Q_UNUSED(emitChanges)
}

void Page::retrieveResourceUrl()
{
    OrgKdeAkonadiSugarCRMSettingsInterface iface(
                QLatin1String("org.freedesktop.Akonadi.Resource.") + mResourceIdentifier, QLatin1String("/Settings"), QDBusConnection::sessionBus() );
    QDBusPendingReply<QString> reply = iface.host();
    reply.waitForFinished();
    if (reply.isValid()) {
        mResourceBaseUrl = iface.host();
        mDetailsWidget->details()->setResourceIdentifier(mResourceIdentifier, mResourceBaseUrl);
    }
}

// duplicated in listentriesjob.cpp
static const char s_timeStampKey[] = "timestamp";

KJob *Page::clearTimestamp()
{
    Collection coll(mCollection.id());
    coll.setResource(mCollection.resource());
    EntityAnnotationsAttribute *annotationsAttribute =
            mCollection.attribute<EntityAnnotationsAttribute>();
    EntityAnnotationsAttribute *newAnnotationsAttribute =
            coll.attribute<EntityAnnotationsAttribute>(Entity::AddIfMissing);
    if (annotationsAttribute)
        *newAnnotationsAttribute = *annotationsAttribute;
    newAnnotationsAttribute->insert(s_timeStampKey, QString());
    Akonadi::CollectionModifyJob *modJob = new Akonadi::CollectionModifyJob(coll, this);
    return modJob;
}

void Page::slotItemChanged(const Item &item, const QSet<QByteArray> &partIdentifiers)
{
    // partIdentifiers is "REMOTEREVISION" or "PLD:RFC822"
    //qDebug() << "slotItemChanged" << partIdentifiers;
    Q_UNUSED(partIdentifiers);
    if (mType == Account && item.hasPayload<SugarAccount>()) {
        const SugarAccount account = item.payload<SugarAccount>();
        const QString id = account.id();
        Q_ASSERT(!id.isEmpty());
        const bool newAccount = !AccountRepository::instance()->hasId(id);
        bool updateNameRef = newAccount;
        // Accounts first get created without an ID, and then the remote ID comes in (after the sync).
        // So we wait for the first dataChanged to create new accounts
        if (newAccount) {
            AccountRepository::instance()->addAccount(account, item.id());
        } else {
            QVector<AccountRepository::Field> changed = AccountRepository::instance()->modifyAccount(account);
            updateNameRef = changed.contains(AccountRepository::Name);
        }
        if (updateNameRef)
            ReferencedData::instance(AccountRef)->setReferencedData(id, account.name());
    } else if (mType == Contact && item.hasPayload<KABC::Addressee>()) {
        const KABC::Addressee addressee = item.payload<KABC::Addressee>();
        const QString fullName = addressee.givenName() + ' ' + addressee.familyName();
        const QString id = addressee.custom("FATCRM", "X-ContactId");
        Q_ASSERT(!id.isEmpty());
        ReferencedData::instance(ContactRef)->setReferencedData(id, fullName);
        ReferencedData::instance(AssignedToRef)->setReferencedData(addressee.custom("FATCRM", "X-AssignedUserId"), addressee.custom("FATCRM", "X-AssignedUserName"));
    }
}
