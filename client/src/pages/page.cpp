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

#include "accountrepository.h"
#include "clientsettings.h"
#include "details.h"
#include "detailsdialog.h"
#include "enums.h"
#include "fatcrminputdialog.h"
#include "itemdataextractor.h"
#include "kjobprogresstracker.h"
#include "modelrepository.h"
#include "rearrangecolumnsproxymodel.h"
#include "referenceddata.h"
#include "reportgenerator.h"
#include "sugarresourcesettings.h"

#include "kdcrmdata/enumdefinitionattribute.h"
#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/sugaraccount.h"
#include "kdcrmdata/sugaropportunity.h"
#include "kdcrmdata/sugarcampaign.h"
#include "kdcrmdata/sugarlead.h"

#include <Akonadi/AgentManager>
#include <Akonadi/ChangeRecorder>
#include <Akonadi/CollectionModifyJob>
#include <Akonadi/CollectionStatistics>
#include <Akonadi/EntityAnnotationsAttribute>
#include <Akonadi/EntityMimeTypeFilterModel>
#include <Akonadi/Item>
#include <Akonadi/ItemCreateJob>
#include <Akonadi/ItemDeleteJob>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/ItemModifyJob>

#include <KABC/Address>
#include <KABC/Addressee>

#include <KDebug>
#include <KInputDialog>

#include <QClipboard>
#include <QDesktopServices>
#include <QMenu>
#include <QMessageBox>
#include <QShortcut>
#include <KPIMUtils/Email>

namespace {

enum ModifierField
{
    CityField,
    CountryField,
    NextStepDateField,
    AssigneeField
};

static const char s_modifierFieldId[] = "__modifierField";

} // namespace

Q_DECLARE_METATYPE(ModifierField);

using namespace Akonadi;

Page::Page(QWidget *parent, const QString &mimeType, DetailsType type)
    : QWidget(parent),
      mMimeType(mimeType),
      mType(type),
      mChangeRecorder(0),
      mItemsTreeModel(0),
      mOnline(false),
      mInitialLoadingDone(false),
      mFilterModel(0),
      mJobProgressTracker(Q_NULLPTR)
{
    mUi.setupUi(this);
    mUi.treeView->setViewName(typeToString(type));
    mUi.treeView->setAlternatingRowColors(true);
    initialize();
}

Page::~Page()
{
}

void Page::openDialog(const QString &id)
{
    const int count = mItemsTreeModel->rowCount();
    for (int i = 0; i < count; ++i) {
        const QModelIndex index = mItemsTreeModel->index(i, 0);
        const Item item = mItemsTreeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        // we do not check for itemDataExtractor == 0 because we know it is an AccountDataExtractor
        if (itemDataExtractor()->idForItem(item) == id) {
            DetailsDialog *dialog = openedDialogForItem(item);
            if (!dialog) {
                dialog = createDetailsDialog();
                dialog->setItem(item);
                dialog->show();
                // cppcheck-suppress memleak as dialog deletes itself
            } else {
                dialog->raise();
            }
        }
    }
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
        } else {
            dialog->close();
        }
    }
    return true;
}

void Page::slotNewClicked()
{
    const QMap<QString, QString> data = dataForNewObject();
    DetailsDialog *dialog = createDetailsDialog();
    Item item;
    item.setParentCollection(mCollection);
    dialog->showNewItem(data, mCollection);
    dialog->show();
    // cppcheck-suppress memleak as dialog deletes itself
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
    msgBox.setText(QString("The selected item will be deleted permanently!"));
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
        ItemDeleteJob *job = new ItemDeleteJob(item, this);
        connect( job, SIGNAL(result(KJob*)), this, SLOT(slotDeleteJobResult(KJob*)));
    }
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
}

void Page::slotRowsAboutToBeRemoved(const QModelIndex &, int start, int end)
{
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

    // Reloading is already available in the toolbar (and using F5 for just one collection)
    // so unclutter the GUI a bit
    mUi.reloadPB->hide();

    connect(mUi.clearSearchPB, SIGNAL(clicked()),
            this, SLOT(slotResetSearch()));
    connect(mUi.newPB, SIGNAL(clicked()),
            this, SLOT(slotNewClicked()));
    connect(mUi.reloadPB, SIGNAL(clicked()),
            this, SLOT(slotReloadCollection()));

    QShortcut* reloadShortcut = new QShortcut(QKeySequence::Refresh, this);
    connect(reloadShortcut, SIGNAL(activated()), this, SLOT(slotReloadCollection()));
}

void Page::slotItemContextMenuRequested(const QPoint &pos)
{
    const QModelIndex idx = treeView()->selectionModel()->currentIndex();
    if (idx.isValid()) {
        const Item item = treeView()->model()->data(idx, EntityTreeModel::ItemRole).value<Item>();
        ItemDataExtractor *dataExtractor = itemDataExtractor();
        if (!dataExtractor) {
            mCurrentItemUrl = QUrl();
            return;
        }
        mCurrentItemUrl = dataExtractor->itemUrl(mResourceBaseUrl, item);
    }

    mSelectedEmails.clear();

    const QModelIndexList selectedIndexes = treeView()->selectionModel()->selectedRows();
    Q_FOREACH (const QModelIndex &index, selectedIndexes) {
        const Item item = index.data(EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<KABC::Addressee>()) {
            const KABC::Addressee addressee = item.payload<KABC::Addressee>();
            const QString preferredEmail = KPIMUtils::normalizedAddress(addressee.assembledName(), addressee.preferredEmail());

            if (!preferredEmail.isEmpty())
                mSelectedEmails.append(preferredEmail);
        }
    }

    QMenu contextMenu;

    contextMenu.addAction(i18n("Delete..."), this, SLOT(slotRemoveItem()));

    if (mCurrentItemUrl.isValid()) {
        contextMenu.addAction(i18n("Open in &Web Browser"), this, SLOT(slotOpenUrl()));
        contextMenu.addAction(i18n("Copy &Link Location"), this, SLOT(slotCopyLink()));
    }

    if (!mSelectedEmails.isEmpty()) {
        contextMenu.addAction(i18n("Open in &Email Client"), this, SLOT(slotOpenEmailClient()));
    }

    if (!selectedIndexes.isEmpty() && (mType == Account || mType == Opportunity || mType == Contact)) {
        QMenu *changeMenu = contextMenu.addMenu(i18n("Change..."));
        if (mType == Account || mType == Contact) {
            QAction *cityAction = changeMenu->addAction(i18n("City"), this, SLOT(slotChangeFields()));
            cityAction->setProperty(s_modifierFieldId, QVariant::fromValue(CityField));

            QAction *countryAction = changeMenu->addAction(i18n("Country"), this, SLOT(slotChangeFields()));
            countryAction->setProperty(s_modifierFieldId, QVariant::fromValue(CountryField));
        } else if (mType == Opportunity) {
            QAction *dateAction = changeMenu->addAction(i18n("Next Step Date"), this, SLOT(slotChangeFields()));
            dateAction->setProperty(s_modifierFieldId, QVariant::fromValue(NextStepDateField));

            QAction *assigneeAction = changeMenu->addAction(i18n("Assignee"), this, SLOT(slotChangeFields()));
            assigneeAction->setProperty(s_modifierFieldId, QVariant::fromValue(AssigneeField));
        }
    }

    if (contextMenu.actions().isEmpty()) {
        return;
    }

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

void Page::slotOpenEmailClient()
{
    QDesktopServices::openUrl("mailto:" + mSelectedEmails.join(","));
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

    ModelRepository::instance()->setModel(mType, treeView()->model());

    emit modelCreated(mItemsTreeModel); // give it to the reports page
}

void Page::insertFilterWidget(QWidget *widget)
{
    mUi.verticalLayout->insertWidget(0, widget);
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
    }
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
        }
    }
}

void Page::readEnumDefinitionAttributes()
{
    EnumDefinitionAttribute *enumsAttr = mCollection.attribute<EnumDefinitionAttribute>();
    if (enumsAttr) {
        mEnumDefinitions = EnumDefinitions::fromString(enumsAttr->value());
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
    kDebug() << collection.id() << attributeNames;
    if (mCollection.isValid() && collection == mCollection) {
        mCollection = collection;

        if (attributeNames.contains("entityannotations")) {
            readSupportedFields();
        }
        if (attributeNames.contains("CRM-enumdefinitions")) { // EnumDefinitionAttribute::type()
            readEnumDefinitionAttributes();
        }
    }
}

// triggered on double-click and Key_Return
void Page::slotItemDoubleClicked(const Akonadi::Item &item)
{
    DetailsDialog *dialog = openedDialogForItem(item);
    if (!dialog) {
        dialog = createDetailsDialog();
        dialog->setItem(item);
        dialog->show();
    } else {
        dialog->raise();
    }
}

void Page::slotItemSaved()
{
    emit statusMessage("Item successfully saved");
}

void Page::printReport()
{
    ReportGenerator generator;
    QAbstractItemModel *model = mUi.treeView->model();
    if (!model)
        return;
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
    Details* details = DetailsDialog::createDetailsForType(mType);
    details->setResourceIdentifier(mResourceIdentifier, mResourceBaseUrl);
    details->setNotesRepository(mNotesRepository);
    details->setSupportedFields(mSupportedFields);
    details->setEnumDefinitions(mEnumDefinitions);
    connect(details, SIGNAL(openObject(DetailsType,QString)),
            this, SIGNAL(openObject(DetailsType,QString)));
    // Don't set a parent, so that the dialogs can be minimized/restored independently
    DetailsDialog *dialog = new DetailsDialog(details);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setOnline(mOnline);
    // in case of changes while the dialog is up
    connect(this, SIGNAL(modelItemChanged(Akonadi::Item)),
            dialog, SLOT(updateItem(Akonadi::Item)));
    connect(this, SIGNAL(onlineStatusChanged(bool)),
            dialog, SLOT(setOnline(bool)));
    connect( dialog, SIGNAL(itemSaved()),
             this, SLOT(slotItemSaved()));
    connect(dialog, SIGNAL(closing()),
            this, SLOT(slotUnregisterDetailsDialog()));
    mDetailsDialogs.insert(dialog);
    return dialog;
}

DetailsDialog *Page::openedDialogForItem(const Item &item)
{
    auto it = std::find_if(mDetailsDialogs.constBegin(), mDetailsDialogs.constEnd(),
                           [&](DetailsDialog *dialog) { return item.id() == dialog->item().id(); });
    if (it == mDetailsDialogs.constEnd()) {
        return nullptr;
    }
    return *it;
}

void Page::slotUnregisterDetailsDialog()
{
    DetailsDialog *dialog = qobject_cast<DetailsDialog*>(sender());
    mDetailsDialogs.remove(dialog);
}

void Page::slotChangeFields()
{
    const QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    const ModifierField field = action->property(s_modifierFieldId).value<ModifierField>();

    QString dialogTitle;
    QString dialogText;
    switch (field) {
    case CityField:
        dialogTitle = i18n("Change City");
        dialogText = i18n("Please enter the new city name.");
        break;
    case CountryField:
        dialogTitle = i18n("Change Country");
        dialogText = i18n("Please enter the new country name.");
        break;
    case NextStepDateField:
        dialogTitle = i18n("Change Next Step Date");
        dialogText = i18n("Please select the next step date.");
        break;
    case AssigneeField:
        dialogTitle = i18n("Change Assignee");
        dialogText = i18n("Please select the new assignee.");
        break;
    }

    bool ok = false;
    QString replacementString;
    QPair<QString, QString> replacementAssignee;
    QDate replacementDate;

    if (field == NextStepDateField) {
        replacementDate = FatCRMInputDialog::getDate(dialogTitle, dialogText, QDate::currentDate(), &ok);
    } else if (field == AssigneeField) {
        replacementAssignee = FatCRMInputDialog::getAssignedUser(dialogTitle, dialogText, &ok);
    } else {
        replacementString = KInputDialog::getText(dialogTitle, dialogText, QString(), &ok);
    }

    if (!ok)
        return;

    const QModelIndexList selectedIndexes = treeView()->selectionModel()->selectedRows();

    QVector<Item> modifiedItems;
    modifiedItems.reserve(selectedIndexes.count());

    Q_FOREACH (const QModelIndex &index, selectedIndexes) {
        Item item = index.data(EntityTreeModel::ItemRole).value<Item>();

        if (mType == Account) {
            SugarAccount account = item.payload<SugarAccount>();

            if (field == CityField) {
                if (account.shippingAddressCity().isEmpty())
                    account.setBillingAddressCity(replacementString);
                else
                    account.setShippingAddressCity(replacementString);
            } else if (field == CountryField) {
                if (account.shippingAddressCountry().isEmpty())
                    account.setBillingAddressCountry(replacementString);
                else
                    account.setShippingAddressCountry(replacementString);
            }

            item.setPayload(account);
        } else if (mType == Opportunity) {
            SugarOpportunity opportunity = item.payload<SugarOpportunity>();

            if (field == NextStepDateField) {
                opportunity.setNextCallDate(replacementDate);
            } else if (field == AssigneeField) {
                opportunity.setAssignedUserId(replacementAssignee.first);
                opportunity.setAssignedUserName(replacementAssignee.second);
            }

            item.setPayload(opportunity);
        } else if (mType == Contact) {
            KABC::Addressee addressee = item.payload<KABC::Addressee>();

            if (field == CityField) {
                KABC::Address workAddress = addressee.address(KABC::Address::Work | KABC::Address::Pref);
                workAddress.setLocality(replacementString);
                addressee.insertAddress(workAddress);
            } else if (field == CountryField) {
                KABC::Address workAddress = addressee.address(KABC::Address::Work | KABC::Address::Pref);
                workAddress.setCountry(replacementString);
                addressee.insertAddress(workAddress);
            }

            item.setPayload(addressee);
        }

        modifiedItems << item;
    }

    mJobProgressTracker = new KJobProgressTracker(this, this);
    mJobProgressTracker->setCaption(dialogTitle);
    mJobProgressTracker->setLabel(i18n("Please wait..."));
    connect(mJobProgressTracker, SIGNAL(finished()), mJobProgressTracker, SLOT(deleteLater()));

    QString errorMessage;
    switch (mType) {
    case Account:
        errorMessage = i18n("Failed to change account: %1");
        break;
    case Opportunity:
        errorMessage = i18n("Failed to change opportunity: %1");
        break;
    case Contact:
        errorMessage = i18n("Failed to change contact: %1");
        break;
    default:
        break;
    };

    foreach (const Akonadi::Item &item, modifiedItems) {
        Akonadi::ItemModifyJob *job = new Akonadi::ItemModifyJob(item, this);
        mJobProgressTracker->addJob(job, errorMessage);
    }

    mJobProgressTracker->start();
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
        if (!id.isEmpty()) {
            ReferencedData::instance(ContactRef)->setReferencedData(id, fullName);
            ReferencedData::instance(AssignedToRef)->setReferencedData(addressee.custom("FATCRM", "X-AssignedUserId"), addressee.custom("FATCRM", "X-AssignedUserName"));
        }
    }
}

QString Page::reportTitle() const
{
    return QString();
}

void Page::slotDeleteJobResult(KJob *job)
{
    if (job->error() != 0) {
        const QString error = i18n("Item could not be deleted! Job error: %1").arg(job->errorText());
        kWarning() << error;
        emit statusMessage(error);
    }
}
