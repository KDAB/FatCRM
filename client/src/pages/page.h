/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef PAGE_H
#define PAGE_H

#include "fatcrmprivate_export.h"
#include "enums.h"
#include "filterproxymodel.h"
#include "reportgenerator.h"

#include "kdcrmdata/enumdefinitions.h"

#include <Akonadi/Collection>
#include <Akonadi/Item>

#include <QWidget>

namespace KDReports {
class Report;
}

namespace Akonadi
{
class ChangeRecorder;
class EntityTreeView;
class Item;
}

class CollectionManager;
class ItemDataExtractor;
class ItemEditWidgetBase;
class ItemsTreeModel;
class KJob;
class KJobProgressTracker;
class LinkedItemsRepository;
class QMenu;
class QPoint;
class Ui_Page;

class FATCRMPRIVATE_EXPORT Page : public QWidget
{
    Q_OBJECT
public:
    explicit Page(QWidget *parent, const QString &mimeType, DetailsType type);

    ~Page() override;

    QString mimeType() const { return mMimeType; }
    DetailsType detailsType() const { return mType; }
    void setCollection(const Akonadi::Collection& collection);
    Akonadi::Collection collection() const { return mCollection; }
    void setCollectionManager(CollectionManager *collectionManager);
    void setLinkedItemsRepository(LinkedItemsRepository *repo);
    LinkedItemsRepository *linkedItemsRepository() const;
    bool queryClose();
    void openWidget(const QString &id);
    Q_REQUIRED_RESULT std::unique_ptr<KDReports::Report> generateReport(bool warnOnLongReport = true) const;
    void exportToCSV(const QString &fileName) const;
    void createNewItem(const QMap<QString, QString> &data = QMap<QString, QString>());
    void setSearchText(const QString &searchText);
    QString searchText() const;

Q_SIGNALS:
    void modelCreated(ItemsTreeModel *model);
    void statusMessage(const QString &);
    void modelLoaded(DetailsType type);
    void modelItemChanged(const Akonadi::Item &item);
    void synchronizeCollection(const Akonadi::Collection &collection);
    void syncRequired();
    void openObject(DetailsType type, const QString &id);
    void onlineStatusChanged(bool online);

public Q_SLOTS:
    void slotOnlineStatusChanged(bool online);
    void slotResourceSelectionChanged(const QByteArray &identifier);
    void openWidgetForItem(const Akonadi::Item &item, DetailsType itemType);

protected:
    virtual ItemDataExtractor *itemDataExtractor() const = 0;
    virtual void handleNewRows(int start, int end, bool emitChanges) = 0;
    virtual void handleRemovedRows(int start, int end, bool initialLoadingDone);
    virtual void handleItemChanged(const Akonadi::Item &item);
    virtual QMenu *createContextMenu(const QPoint &pos);

    Akonadi::EntityTreeView *treeView() const;

    ItemsTreeModel *itemsTreeModel() const {
        return mItemsTreeModel;
    }
    void setFilter(FilterProxyModel *filter);

    virtual void setupModel();

    void insertFilterWidget(QWidget *widget);

private Q_SLOTS:
    void slotNewClicked();
    void slotAnonymizeItems();
    void slotDeleteItems();
    void slotVisibleRowCountChanged();
    void slotRowsInserted(const QModelIndex &, int start, int end);
    void slotCheckCollectionPopulated(Akonadi::Collection::Id);
    void slotRowsAboutToBeRemoved(const QModelIndex &, int start, int end);
    void slotDataChanged(const QModelIndex &, const QModelIndex &);
    void slotReloadCollection();
    void slotCollectionChanged(const Akonadi::Collection &collection, const QSet<QByteArray> &attributeNames);
    void slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray> &partIdentifiers);
    void slotItemDoubleClicked(const Akonadi::Item &item);
    void slotItemSaved();
    void slotItemContextMenuRequested(const QPoint &pos);
    void slotOpenUrl();
    void slotCopyLink();
    void slotUnregisterItemEditWidget();
    void slotChangeFields();
    void slotDeleteJobResult(KJob *job);

private:
    virtual QString reportTitle() const = 0;
    QString reportSubTitle(int count) const;
    virtual QMap<QString, QString> dataForNewObject() { return QMap<QString, QString>(); }
    void initialize();
    void retrieveResourceUrl();

    enum ItemEditWidgetType { Simple, TabWidget };
    ItemEditWidgetBase *createItemEditWidget(const Akonadi::Item &item, DetailsType itemType, bool forceSimpleWidget = false);
    ItemEditWidgetBase *openedWidgetForItem(Akonadi::Item::Id id);
    void modifyItems(const QVector<Akonadi::Item> &modifiedItems, const QString &dialogTitle);

    Akonadi::Item::List selectedItems() const;

private:
    Ui_Page *mUi = nullptr;

    QString mMimeType;
    DetailsType mType;
    FilterProxyModel *mFilter;
    Akonadi::ChangeRecorder *mChangeRecorder;
    ItemsTreeModel *mItemsTreeModel;
    Akonadi::Collection mCollection;
    QByteArray mResourceIdentifier;

    // Things we keep around so we can set them on the details dialog when creating it
    QString mResourceBaseUrl;
    QUrl mCurrentItemUrl;
    CollectionManager *mCollectionManager;
    LinkedItemsRepository *mLinkedItemsRepository;
    bool mOnline;
    bool mInitialLoadingDone;

    KJobProgressTracker *mJobProgressTracker;
    QVector<int> sourceColumns() const;
};

#endif
