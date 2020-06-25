/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "ui_page.h"

#include "accountrepository.h"
#include "clientsettings.h"
#include "details.h"
#include "fatcrminputdialog.h"
#include "itemdataextractor.h"
#include "itemeditwidgetbase.h"
#include "kjobprogresstracker.h"
#include "modelrepository.h"
#include "openedwidgetsrepository.h"
#include "rearrangecolumnsproxymodel.h"
#include "referenceddata.h"
#include "reportgenerator.h"
#include "simpleitemeditwidget.h"
#include "sugarresourcesettings.h"
#include "tabbeditemeditwidget.h"
#include "collectionmanager.h"
#include "createlinksproxymodel.h"
#include "fatcrm_client_debug.h"

#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/sugaraccount.h"
#include "kdcrmdata/sugaropportunity.h"
#include "kdcrmdata/sugarcampaign.h"
#include "kdcrmdata/sugarlead.h"

#include <KDReportsReport.h>

#include <AkonadiCore/AgentManager>
#include <AkonadiCore/ChangeRecorder>
#include <AkonadiCore/CollectionStatistics>
#include <AkonadiCore/EntityMimeTypeFilterModel>
#include <AkonadiCore/Item>
#include <AkonadiCore/ItemCreateJob>
#include <AkonadiCore/ItemDeleteJob>
#include <AkonadiCore/ItemFetchScope>
#include <AkonadiCore/ItemModifyJob>
#include <AkonadiCore/EntityAnnotationsAttribute>
#include <AkonadiCore/ServerManager>
#include <AkonadiWidgets/EntityTreeView>

#include <KContacts/Address>
#include <KContacts/Addressee>

#include <KLocalizedString>

#include <QClipboard>
#include <QDebug>
#include <QDesktopServices>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QShortcut>
#include <KEmailAddress>

using namespace Akonadi;

namespace {

enum ModifierField
{
    CityField,
    CountryField,
    NextStepDateField,
    AssigneeField
};

static const char s_modifierFieldId[] = "__modifierField";

QStringList preferredMailAddressesForSelectedRows(const QModelIndexList selectedIndexes)
{
    QStringList result;
    Q_FOREACH (const QModelIndex &index, selectedIndexes) {
        const Item item = index.data(EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<KContacts::Addressee>()) {
            const KContacts::Addressee addressee = item.payload<KContacts::Addressee>();
            if (addressee.preferredEmail().isEmpty())
                continue;

            const QString preferredEmail = KEmailAddress::normalizedAddress(addressee.assembledName(), addressee.preferredEmail());
            if (preferredEmail.isEmpty())
                continue;

            result.append(preferredEmail);
        }
    }
    return result;
}

/// Returns a map of action names -> call URLs, e.g. {"Call Work" -> "tel:+49..."}
QVector<QPair<QString, QUrl>> phoneNumberMapForRow(const QModelIndex &index)
{
    using namespace KContacts;

    QVector<QPair<QString, QUrl>> result;
    const Item item = index.data(EntityTreeModel::ItemRole).value<Item>();
    if (item.hasPayload<KContacts::Addressee>()) {
        const KContacts::Addressee addressee = item.payload<KContacts::Addressee>();
        const auto phoneNumbers = addressee.phoneNumbers();
        for (const auto& phoneNumber : phoneNumbers) {
            static const QVector<PhoneNumber::Type> telTypes = {PhoneNumber::Home, PhoneNumber::Work, PhoneNumber::Cell};

            if (telTypes.contains(phoneNumber.type())) {
                result.append({i18n("Call %1", phoneNumber.typeLabel()), QUrl("tel:" + phoneNumber.number())});
            }
        }
    }
    return result;
}

} // namespace

Q_DECLARE_METATYPE(ModifierField);

Page::Page(QWidget *parent, const QString &mimeType, DetailsType type)
    : QWidget(parent),
      mUi(new Ui_Page),
      mMimeType(mimeType),
      mType(type),
      mFilter(nullptr),
      mChangeRecorder(nullptr),
      mItemsTreeModel(nullptr),
      mCollection(),
      mCollectionManager(nullptr),
      mLinkedItemsRepository(nullptr),
      mOnline(false),
      mInitialLoadingDone(false),
      mJobProgressTracker(nullptr)
{
    mUi->setupUi(this);
    mUi->treeView->setViewName(typeToString(type));
    mUi->treeView->setAlternatingRowColors(true);
    mUi->treeView->setUniformRowHeights(true);
    initialize();
}

Page::~Page()
{
    delete mUi;
}

void Page::openWidget(const QString &id)
{
    ItemDataExtractor *dataExtractor = itemDataExtractor();
    if (!dataExtractor) {
        return;
    }
    const int count = mItemsTreeModel->rowCount();
    for (int i = 0; i < count; ++i) {
        const QModelIndex index = mItemsTreeModel->index(i, 0);
        const Item item = mItemsTreeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (dataExtractor->idForItem(item) == id) {
            openWidgetForItem(item, mType);
            return;
        }
    }
    qCWarning(FATCRM_CLIENT_LOG) << this << "(" << typeToString(mType) << ") Object not found:" << id << "among" << count << "rows";
}

void Page::openWidgetForItem(const Item &item, DetailsType itemType)
{
    ItemEditWidgetBase *widget = openedWidgetForItem(item.id());

    if (!widget) {
        widget = createItemEditWidget(item, itemType);
        widget->show();
    } else {
        if (widget->isMinimized()) {
            widget->showNormal();
        }
        widget->raise();
        widget->activateWindow();
    }
}

void Page::setFilter(FilterProxyModel *filter)
{
    mFilter = filter;

    mFilter->setSortRole(Qt::EditRole); // to allow custom formatting for dates in DisplayRole
    connect(mFilter, &QAbstractItemModel::layoutChanged, this, &Page::slotVisibleRowCountChanged);
    connect(mFilter, &QAbstractItemModel::rowsInserted, this, &Page::slotVisibleRowCountChanged);
    connect(mFilter, &QAbstractItemModel::rowsRemoved, this, &Page::slotVisibleRowCountChanged);

    connect(mUi->searchLE, &QLineEdit::textChanged,
            mFilter, &FilterProxyModel::setFilterString);
}

// Connected to signal resourceSelected() from the mainwindow
void Page::slotResourceSelectionChanged(const QByteArray &identifier)
{
    delete mChangeRecorder;
    mChangeRecorder = nullptr;
    mCollection = Collection();
    mResourceIdentifier = identifier;

    // cleanup from last time (useful when switching resources)
    ModelRepository::instance()->removeModel(mType);
    mFilter->setSourceModel(nullptr);
    mUi->treeView->setModel(nullptr);

    delete mItemsTreeModel;
    mItemsTreeModel = nullptr;

    retrieveResourceUrl();
    mUi->reloadPB->setEnabled(false);

    mInitialLoadingDone = false;

    // now we wait for the collection manager to find our collection and tell us
}

void Page::handleRemovedRows(int start, int end, bool initialLoadingDone)
{
    Q_UNUSED(start)
    Q_UNUSED(end)
    Q_UNUSED(initialLoadingDone)
}

void Page::handleItemChanged(const Item &item)
{
    Q_UNUSED(item)
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

    if (mCollection.isValid()) {
        mUi->newPB->setEnabled(true);
        mUi->reloadPB->setEnabled(true);

        mChangeRecorder = new ChangeRecorder(this);
        mChangeRecorder->setCollectionMonitored(mCollection, true);
        // automatically get the full data when items change
        mChangeRecorder->itemFetchScope().fetchFullPayload(true);
        // don't get remote id/rev, to avoid errors in the FATCRM-75 case
        mChangeRecorder->itemFetchScope().setFetchRemoteIdentification(false);
        mChangeRecorder->setMimeTypeMonitored(mMimeType);
        connect(mChangeRecorder, SIGNAL(collectionChanged(Akonadi::Collection,QSet<QByteArray>)),
                this, SLOT(slotCollectionChanged(Akonadi::Collection,QSet<QByteArray>)));
        connect(mChangeRecorder, &Monitor::itemChanged,
                this, &Page::slotItemChanged);

        // if empty, the collection might not have been loaded yet, try synchronizing
        if (mCollection.statistics().count() == 0) {
            AgentManager::self()->synchronizeCollection(mCollection);
        }

        setupModel();
    } else {
        mUi->newPB->setEnabled(false);
        mUi->reloadPB->setEnabled(false);
    }
}

void Page::setCollectionManager(CollectionManager *collectionManager)
{
    mCollectionManager = collectionManager;
}

void Page::setLinkedItemsRepository(LinkedItemsRepository *repo)
{
    mLinkedItemsRepository = repo;
}

LinkedItemsRepository *Page::linkedItemsRepository() const
{
    return mLinkedItemsRepository;
}

bool Page::queryClose()
{
    foreach (ItemEditWidgetBase *widget, OpenedWidgetsRepository::instance()->openedWidgets()) {
        if (widget->isModified()) {
            if (QMessageBox::Yes == QMessageBox::question(this, i18n("Close?"),
                                                          i18n("A currently opened dialog has modifications, are you sure you want to exit without saving?"),
                                                          QMessageBox::Yes|QMessageBox::No))
            {
                widget->close();
            } else {
                widget->raise();
                widget->activateWindow();
                return false;
            }
        } else {
            widget->close();
        }
    }
    return true;
}

void Page::slotNewClicked()
{
    createNewItem();
}

QString Page::reportSubTitle(int count) const
{
    const QString itemsType = typeToTranslatedString(mType);
    const QString desc = mFilter->filterDescription();
    if (desc.isEmpty())
        return i18n("%1 %2", count, itemsType);
    return i18n("%1: %2 %3", desc, count, itemsType);
}

void Page::slotAnonymizeItems()
{
    Item::List items = selectedItems();
    if (items.isEmpty()) { // the action is supposed to be disabled...
        return;
    }

    const Item firstItem = items.at(0);
    Q_ASSERT(firstItem.hasPayload<KContacts::Addressee>());
    const auto firstContact = firstItem.payload<KContacts::Addressee>();
    const QString confirmationStr = i18np("The contact \"%1\" will be anonymized permanently!",
                                          "%2 contacts will be anonymized permanently!",
                                          firstContact.assembledName(), items.count());
    QMessageBox msgBox;
    msgBox.setWindowTitle(i18np("Anonymize record", "Anonymize %1 records", items.count()));
    msgBox.setText(confirmationStr);
    msgBox.setInformativeText(i18n("Are you sure you want to proceed?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    if (msgBox.exec() == QMessageBox::Cancel) {
        return;
    }

    for (Item &item : items) {
        Q_ASSERT(item.hasPayload<KContacts::Addressee>());
        KContacts::Addressee contact = item.payload<KContacts::Addressee>();

        contact.setGivenName("Anonymized");
        contact.setFamilyName("GDPR");
        if (!contact.emails().isEmpty()) {
            // Sugar doesn't let us remove all emails, email1 reappears.
            // So let's set a fake one
            contact.setEmails({QStringLiteral("anonymized@example.org")});
        }
        contact.setPhoneNumbers({});
        // Keep the address, it's useful especially for the country,
        // and it's not tied to an individual anymore (those are office addresses)

        item.setPayload(contact);
    }
    modifyItems(items, i18n("Anonymizing"));
}

void Page::slotDeleteItems()
{
    const Item::List items = selectedItems();
    if (items.isEmpty()) { // the action is supposed to be disabled...
        return;
    }

    const Item firstItem = items.at(0);
    QString deleStr = i18np("The selected item will be deleted permanently!",
                            "%1 items will be deleted permanently!", items.count());
    switch (mType) {
    case DetailsType::Account: {
        Q_ASSERT(firstItem.hasPayload<SugarAccount>());
        SugarAccount acct = firstItem.payload<SugarAccount>();
        deleStr = i18np("The account \"%1\" will be deleted permanently!",
                        "%2 accounts will be deleted permanently!",
                        acct.name(), items.count());
        break;
    }
    case DetailsType::Opportunity: {
        Q_ASSERT(firstItem.hasPayload<SugarOpportunity>());
        SugarOpportunity opp = firstItem.payload<SugarOpportunity>();
        deleStr = i18np("The %1 opportunity \"%2\" will be deleted permanently!",
                        "%3 accounts will be deleted permanently!",
                        opp.tempAccountName(), opp.name(), items.count());
        break;
    }
    case DetailsType::Contact: {
        Q_ASSERT(firstItem.hasPayload<KContacts::Addressee>());
        const auto contact = firstItem.payload<KContacts::Addressee>();
        deleStr = i18np("The contact \"%1\" will be deleted permanently!",
                        "%2 contacts will be deleted permanently!",
                        contact.fullEmail(), items.count());
        break;
    }
    case DetailsType::Lead:
    case DetailsType::Campaign:
        break;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle(i18np("Delete record", "Delete %1 records", items.count()));
    msgBox.setText(deleStr);
    msgBox.setInformativeText(i18n("Are you sure you want to proceed?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    if (msgBox.exec() == QMessageBox::Cancel) {
        return;
    }

    // job starts automatically
    auto *job = new ItemDeleteJob(items, this);
    connect(job, &KJob::result, this, &Page::slotDeleteJobResult);
}

void Page::slotVisibleRowCountChanged()
{
    if (mUi->treeView->model()) {
        mUi->itemCountLB->setText(QStringLiteral("%1 %2").arg(mUi->treeView->model()->rowCount()).arg(typeToTranslatedString(mType)));
    }
}

void Page::slotRowsInserted(const QModelIndex &, int start, int end)
{
    const bool emitChanges = mInitialLoadingDone;

    handleNewRows(start, end, emitChanges);

    if (!mInitialLoadingDone)
        slotCheckCollectionPopulated(mCollection.id());
}

void Page::slotCheckCollectionPopulated(Akonadi::Collection::Id id)
{
    //qCDebug(FATCRM_CLIENT_LOG) << "model has" << mItemsTreeModel->rowCount()
    //         << "rows, we expect" << mCollection.statistics().count();
    if (mItemsTreeModel->rowCount() == 0)
        return;

    const bool done = !mInitialLoadingDone && mItemsTreeModel->isCollectionPopulated(id);
    if (done) {
        //qCDebug(FATCRM_CLIENT_LOG) << "Finished loading" << typeToString(mType);
        // Select the first row (historic reason: embedded details widget)
        if (!mUi->treeView->currentIndex().isValid()) {
            mUi->treeView->setCurrentIndex(mUi->treeView->model()->index(0, 0));
        }
        mInitialLoadingDone = true;
        // Move to the next model
        //emit modelLoaded(mType, i18n("%1 %2 loaded", mItemsTreeModel->rowCount(), typeToString(mType)));
        emit modelLoaded(mType);

        if (mType == DetailsType::Account) {
            AccountRepository::instance()->emitInitialLoadingDone();
        }
    }
}

void Page::slotRowsAboutToBeRemoved(const QModelIndex &, int start, int end)
{
    handleRemovedRows(start, end, mInitialLoadingDone);
}

void Page::initialize()
{
    connect(mUi->treeView, SIGNAL(doubleClicked(Akonadi::Item)), this, SLOT(slotItemDoubleClicked(Akonadi::Item)));
    connect(mUi->treeView, &ItemsTreeView::returnPressed, this, &Page::slotItemDoubleClicked);
    connect(mUi->treeView, &QWidget::customContextMenuRequested, this, &Page::slotItemContextMenuRequested);

    const QIcon icon = (style() != nullptr ? style()->standardIcon(QStyle::SP_BrowserReload, nullptr, mUi->reloadPB) : QIcon());
    if (!icon.isNull()) {
        mUi->reloadPB->setIcon(icon);
    }
    mUi->reloadPB->setEnabled(false);

    // Reloading is already available in the toolbar (and using F5 for just one collection)
    // so unclutter the GUI a bit
    mUi->reloadPB->hide();

    connect(mUi->newPB, &QAbstractButton::clicked,
            this, &Page::slotNewClicked);
    connect(mUi->reloadPB, &QAbstractButton::clicked,
            this, &Page::slotReloadCollection);

    QShortcut* reloadShortcut = new QShortcut(QKeySequence::Refresh, this);
    connect(reloadShortcut, &QShortcut::activated, this, &Page::slotReloadCollection);
}

void Page::slotItemContextMenuRequested(const QPoint &pos)
{
    QMenu *menu = createContextMenu(pos);
    if (menu) {
        menu->exec(mUi->treeView->mapToGlobal(pos));
        menu->deleteLater();
    }
}

QMenu *Page::createContextMenu(const QPoint &)
{
    const QModelIndex idx = treeView()->selectionModel()->currentIndex();
    if (idx.isValid()) {
        const Item item = treeView()->model()->data(idx, EntityTreeModel::ItemRole).value<Item>();
        ItemDataExtractor *dataExtractor = itemDataExtractor();
        if (!dataExtractor) {
            mCurrentItemUrl = QUrl();
            return nullptr;
        }
        mCurrentItemUrl = dataExtractor->itemUrl(mResourceBaseUrl, item);
    }

    const QModelIndexList selectedIndexes = treeView()->selectionModel()->selectedRows();

    auto *contextMenu = new QMenu(this);

    if (mCurrentItemUrl.isValid()) {
        contextMenu->addSeparator();
        contextMenu->addAction(QIcon::fromTheme("internet-web-browser"), i18n("Open in &Web Browser"), this, &Page::slotOpenUrl);
        contextMenu->addAction(QIcon::fromTheme("edit-copy"), i18n("Copy &Link Location"), this, &Page::slotCopyLink);
    }

    const QStringList selectedEmails = preferredMailAddressesForSelectedRows(selectedIndexes);
    if (!selectedEmails.isEmpty()) {
        contextMenu->addSeparator();
        contextMenu->addAction(QIcon::fromTheme("mail-send"), i18n("Send &Email"), this, [selectedEmails]() {
            QDesktopServices::openUrl("mailto:" + selectedEmails.join(QStringLiteral(",")));
        });
    }

    const auto phoneNumbers = phoneNumberMapForRow(idx);
    if (!phoneNumbers.isEmpty()) {
        if (selectedEmails.isEmpty()) {
            contextMenu->addSeparator();
        }
        for (const auto &phoneNumber : phoneNumbers) {
            contextMenu->addAction(QIcon::fromTheme("call-start"), phoneNumber.first, this, [phoneNumber]() {
                QDesktopServices::openUrl(phoneNumber.second);
            });
        }
    }

    if (!selectedIndexes.isEmpty() && (mType == DetailsType::Account || mType == DetailsType::Opportunity || mType == DetailsType::Contact)) {
        contextMenu->addSeparator();
        QMenu *changeMenu = contextMenu->addMenu(i18n("Change..."));
        changeMenu->setIcon(QIcon::fromTheme("edit-entry"));
        if (mType == DetailsType::Account || mType == DetailsType::Contact) {
            QAction *cityAction = changeMenu->addAction(i18n("City"), this, &Page::slotChangeFields);
            cityAction->setProperty(s_modifierFieldId, QVariant::fromValue(CityField));

            QAction *countryAction = changeMenu->addAction(i18n("Country"), this, &Page::slotChangeFields);
            countryAction->setProperty(s_modifierFieldId, QVariant::fromValue(CountryField));
        } else if (mType == DetailsType::Opportunity) {
            QAction *dateAction = changeMenu->addAction(i18n("Next Step Date"), this, &Page::slotChangeFields);
            dateAction->setProperty(s_modifierFieldId, QVariant::fromValue(NextStepDateField));

            QAction *assigneeAction = changeMenu->addAction(i18n("Assignee"), this, &Page::slotChangeFields);
            assigneeAction->setProperty(s_modifierFieldId, QVariant::fromValue(AssigneeField));
        }
    }

    if (!selectedIndexes.isEmpty()) {
        if (mType == DetailsType::Contact) {
            contextMenu->addAction(QIcon::fromTheme("edit-rename"), i18n("Anonymize..."), this, &Page::slotAnonymizeItems);
        }
        contextMenu->addAction(QIcon::fromTheme("list-remove"), i18n("Delete..."), this, &Page::slotDeleteItems);
    }

    if (contextMenu->actions().isEmpty()) {
        delete contextMenu;
        return nullptr;
    }

    return contextMenu;
}

EntityTreeView *Page::treeView() const
{
    return mUi->treeView;
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
    Q_ASSERT(mLinkedItemsRepository);
    mItemsTreeModel->setLinkedItemsRepository(mLinkedItemsRepository);
    mItemsTreeModel->setCollectionManager(mCollectionManager); // for enum definitions

    connect(mItemsTreeModel, &QAbstractItemModel::rowsInserted, this, &Page::slotRowsInserted);
    connect(mItemsTreeModel, &EntityTreeModel::collectionPopulated, this, &Page::slotCheckCollectionPopulated);
    connect(mItemsTreeModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &Page::slotRowsAboutToBeRemoved);
    connect(mItemsTreeModel, &QAbstractItemModel::dataChanged, this, &Page::slotDataChanged);

    mFilter->setSourceModel(mItemsTreeModel);
    mFilter->setLinkedItemsRepository(mLinkedItemsRepository);
    mUi->treeView->setModels(mFilter, mItemsTreeModel, mItemsTreeModel->defaultVisibleColumns());

    ModelRepository::instance()->setModel(mType, mItemsTreeModel);

    emit modelCreated(mItemsTreeModel); // give it to the reports page
}

void Page::insertFilterWidget(QWidget *widget)
{
    mUi->verticalLayout->insertWidget(0, widget);
}

void Page::createNewItem(const QMap<QString, QString> &data)
{
    QMap<QString, QString> moreData = dataForNewObject();

    moreData.unite(data);

    Item item;
    ItemEditWidgetBase *widget = createItemEditWidget(item, mType, true); // create a simple widget type
    item.setParentCollection(mCollection);
    widget->showNewItem(moreData, mCollection);
    widget->show();
}

void Page::slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    //qCDebug(FATCRM_CLIENT_LOG) << typeToString(mType) << topLeft << bottomRight;
    const int start = topLeft.row();
    const int end = bottomRight.row();
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = mItemsTreeModel->index(row, 0, QModelIndex());
        if (!index.isValid()) {
            qCWarning(FATCRM_CLIENT_LOG) << "Invalid index:" << "row=" << row << "/" << mItemsTreeModel->rowCount();
            return;
        }
        const Item item = index.data(EntityTreeModel::ItemRole).value<Item>();
        Q_ASSERT(item.isValid());
        emit modelItemChanged(item); // update details dialog
    }
}


void Page::slotReloadCollection()
{
    if (mCollection.isValid()) {
        emit synchronizeCollection(mCollection);
    }
}

void Page::slotCollectionChanged(const Akonadi::Collection &collection, const QSet<QByteArray> &attributeNames)
{
    qDebug() << collection.id() << attributeNames;
    if (mCollection.isValid() && collection == mCollection) {
        mCollection = collection;
        mCollectionManager->slotCollectionChanged(collection, attributeNames);
    }
}

// triggered on double-click and Key_Return
void Page::slotItemDoubleClicked(const Akonadi::Item &item)
{
    openWidgetForItem(item, mType);
}

void Page::slotItemSaved()
{
    emit statusMessage(QStringLiteral("Item successfully saved"));
}

QVector<int> Page::sourceColumns() const
{
    QHeaderView *headerView = mUi->treeView->header();
    QVector<int> sourceColumns;
    sourceColumns.reserve(headerView->count());
    for (int col = 0; col < headerView->count(); ++col) {
        const int logicalColumn = headerView->logicalIndex(col);
        if (!headerView->isSectionHidden(logicalColumn)) {
            sourceColumns.append(logicalColumn);
        }
    }

    return sourceColumns;
}

std::unique_ptr<KDReports::Report> Page::generateReport(bool warnOnLongReport) const
{
    QAbstractItemModel *model = mUi->treeView->model();
    if (!model)
        return {};

    const int count = model->rowCount();
    if (warnOnLongReport && count > 1000) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(i18n("Long report warning"));
        msgBox.setText(i18n("The generated report will contain %1 rows, which might be long to generate and print.", count));
        msgBox.setInformativeText(i18n("Are you sure you want to proceed?"));
        msgBox.setStandardButtons(QMessageBox::Yes |
                                  QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Cancel) {
            return {};
        }
    }

    CreateLinksProxyModel createLinksProxy(mResourceBaseUrl);
    createLinksProxy.setSourceModel(model);

    RearrangeColumnsProxyModel rearrangeColumnsProxy;
    rearrangeColumnsProxy.setSourceColumns(sourceColumns()); // take care of hidden and reordered columns
    rearrangeColumnsProxy.setSourceModel(&createLinksProxy);

    ReportGenerator generator;
    return generator.generateListReport(&rearrangeColumnsProxy, reportTitle(), reportSubTitle(count));
}

void Page::exportToCSV(const QString &fileName) const
{
    QAbstractItemModel *model = mUi->treeView->model();
    if (!model)
        return;
    RearrangeColumnsProxyModel rearrangeColumnsProxy;
    rearrangeColumnsProxy.setSourceColumns(sourceColumns()); // take care of hidden and reordered columns
    rearrangeColumnsProxy.setSourceModel(model);

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open" << fileName << "for writing:" << file.errorString();
        return;
    }
    const int rows = rearrangeColumnsProxy.rowCount();
    const int columns = rearrangeColumnsProxy.columnCount();
    for (int row = 0 ; row < rows ; ++row) {
        QString line;
        for (int column = 0 ; column < columns ; ++column) {
            QString str = rearrangeColumnsProxy.index(row, column).data().toString();
            if (str.contains(',')) {
                str = '"' + str + '"';
            }
            line += str + ",";
        }
        line += '\n';
        file.write(line.toUtf8());
    }
}

ItemEditWidgetBase *Page::createItemEditWidget(const Akonadi::Item &item, DetailsType itemType, bool forceSimpleWidget)
{
    Details* details = ItemEditWidgetBase::createDetailsForType(itemType);
    details->setItemsTreeModel(ModelRepository::instance()->model(itemType)); // done here because virtual
    details->setResourceIdentifier(mResourceIdentifier, mResourceBaseUrl);
    details->setCollectionManager(mCollectionManager);
    details->setLinkedItemsRepository(mLinkedItemsRepository);
    // warning, do not use any type-dependent member variable here. We could be creating a widget for another type.

    connect(details, &Details::openObject,
            this, &Page::openObject);
    connect(details, &Details::syncRequired, this, &Page::syncRequired);
    // Don't set a parent, so that the widgets can be minimized/restored independently
    auto *widget = new SimpleItemEditWidget(details);
    widget->setOnline(mOnline);
    if (item.isValid()) // no need to call setItem for "New <Item>" widget
        widget->setItem(item);

    // in case of changes while the widget is up
    connect(this, &Page::modelItemChanged,
            widget, &SimpleItemEditWidget::updateItem);
    connect(this, &Page::onlineStatusChanged,
            widget, &SimpleItemEditWidget::setOnline);
    connect(widget, &ItemEditWidgetBase::itemSaved,
            this, &Page::slotItemSaved);

    ItemEditWidgetType widgetType;
    if (forceSimpleWidget) {
        widgetType = Simple;
    } else {
        if (item.hasPayload<SugarAccount>() || item.hasPayload<SugarOpportunity>())
            widgetType = TabWidget;
        else
            widgetType = Simple;
    }

    if (widgetType == Simple) {
        widget->setAttribute(Qt::WA_DeleteOnClose);
        OpenedWidgetsRepository::instance()->registerWidget(widget);
        connect (widget, &ItemEditWidgetBase::closing,
                 this, &Page::slotUnregisterItemEditWidget);
        return widget;
    } else {
        auto *tabbedWidget = new TabbedItemEditWidget(widget, itemType, this);
        tabbedWidget->setAttribute(Qt::WA_DeleteOnClose);
        connect (tabbedWidget, SIGNAL(openWidgetForItem(Akonadi::Item,DetailsType)),
                 this, SLOT(openWidgetForItem(Akonadi::Item,DetailsType)));
        connect (tabbedWidget, &ItemEditWidgetBase::closing,
                 this, &Page::slotUnregisterItemEditWidget);
        OpenedWidgetsRepository::instance()->registerWidget(tabbedWidget);
        return tabbedWidget;
    }
}

ItemEditWidgetBase *Page::openedWidgetForItem(Item::Id id)
{
    QSet<ItemEditWidgetBase*> itemEditWidgets = OpenedWidgetsRepository::instance()->openedWidgets();
    auto it = std::find_if(itemEditWidgets.constBegin(), itemEditWidgets.constEnd(),
                           [&](ItemEditWidgetBase *widget) { return id == widget->item().id(); });
    if (it == itemEditWidgets.constEnd()) {
        return nullptr;
    }
    return *it;
}

Item::List Page::selectedItems() const
{
    Item::List items;
    const QModelIndexList selectedIndexes = treeView()->selectionModel()->selectedRows();
    items.resize(selectedIndexes.count());
    std::transform(selectedIndexes.begin(), selectedIndexes.end(), items.begin(), [this](const QModelIndex &index) {
        return mUi->treeView->model()->data(index, EntityTreeModel::ItemRole).value<Item>();
    });
    return items;
}

void Page::slotUnregisterItemEditWidget()
{
    auto *widget = qobject_cast<ItemEditWidgetBase*>(sender());
    OpenedWidgetsRepository::instance()->unregisterWidget(widget);
}

void Page::slotChangeFields()
{
    const QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    const auto field = action->property(s_modifierFieldId).value<ModifierField>();

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
        replacementString = QInputDialog::getText(this, dialogTitle, dialogText, QLineEdit::Normal, QString(), &ok);
    }

    if (!ok)
        return;

    const QModelIndexList selectedIndexes = treeView()->selectionModel()->selectedRows();

    QVector<Item> modifiedItems;
    modifiedItems.reserve(selectedIndexes.count());

    Q_FOREACH (const QModelIndex &index, selectedIndexes) {
        Item item = index.data(EntityTreeModel::ItemRole).value<Item>();

        if (mType == DetailsType::Account) {
            Q_ASSERT(item.hasPayload<SugarAccount>());
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
        } else if (mType == DetailsType::Opportunity) {
            Q_ASSERT(item.hasPayload<SugarOpportunity>());
            SugarOpportunity opportunity = item.payload<SugarOpportunity>();

            if (field == NextStepDateField) {
                opportunity.setNextCallDate(replacementDate);
            } else if (field == AssigneeField) {
                opportunity.setAssignedUserId(replacementAssignee.first);
                opportunity.setAssignedUserName(replacementAssignee.second);
            }

            item.setPayload(opportunity);
        } else if (mType == DetailsType::Contact) {
            Q_ASSERT(item.hasPayload<KContacts::Addressee>());
            KContacts::Addressee addressee = item.payload<KContacts::Addressee>();

            if (field == CityField) {
                KContacts::Address workAddress = addressee.address(KContacts::Address::Work | KContacts::Address::Pref);
                workAddress.setLocality(replacementString);
                addressee.insertAddress(workAddress);
            } else if (field == CountryField) {
                KContacts::Address workAddress = addressee.address(KContacts::Address::Work | KContacts::Address::Pref);
                workAddress.setCountry(replacementString);
                addressee.insertAddress(workAddress);
            }

            item.setPayload(addressee);
        }

        modifiedItems << item;
    }
    modifyItems(modifiedItems, dialogTitle);
}

void Page::modifyItems(const QVector<Item> &modifiedItems, const QString &dialogTitle)
{
    mJobProgressTracker = new KJobProgressTracker(this, this);
    mJobProgressTracker->setCaption(dialogTitle);
    mJobProgressTracker->setLabel(i18n("Please wait..."));
    connect(mJobProgressTracker, &KJobProgressTracker::finished, mJobProgressTracker, &QObject::deleteLater);

    QString errorMessage;
    switch (mType) {
    case DetailsType::Account:
        errorMessage = i18n("Failed to change account:");
        break;
    case DetailsType::Opportunity:
        errorMessage = i18n("Failed to change opportunity:");
        break;
    case DetailsType::Contact:
        errorMessage = i18n("Failed to change contact:");
        break;
    default:
        break;
    };

    foreach (const Akonadi::Item &item, modifiedItems) {
        auto *job = new Akonadi::ItemModifyJob(item, this);
        mJobProgressTracker->addJob(job, errorMessage);
    }

    mJobProgressTracker->start();
}

void Page::retrieveResourceUrl()
{
    if (mResourceIdentifier.isEmpty()) { // happens when creating the first resource, before selecting it
        return;
    }

    const auto service = Akonadi::ServerManager::agentServiceName(Akonadi::ServerManager::Resource, mResourceIdentifier);

    OrgKdeAkonadiSugarCRMSettingsInterface iface(service, QStringLiteral("/Settings"), QDBusConnection::sessionBus());
    QDBusPendingReply<QString> reply = iface.host();
    reply.waitForFinished();
    if (reply.isValid()) {
        mResourceBaseUrl = iface.host();
    }
}

void Page::setSearchText(const QString &searchText)
{
    mUi->searchLE->setText(searchText);
}

QString Page::searchText() const
{
    return mUi->searchLE->text();
}

void Page::slotItemChanged(const Item &item, const QSet<QByteArray> &partIdentifiers)
{
    // partIdentifiers is "REMOTEREVISION" or "PLD:RFC822"
    //qDebug() << "slotItemChanged" << partIdentifiers;
    Q_UNUSED(partIdentifiers);
    handleItemChanged(item);
}

QString Page::reportTitle() const
{
    return QString();
}

void Page::slotDeleteJobResult(KJob *job)
{
    if (job->error() != 0) {
        const QString error = i18n("Item could not be deleted! Job error: %1").arg(job->errorText());
        qWarning() << error;
        emit statusMessage(error);
    }
}
