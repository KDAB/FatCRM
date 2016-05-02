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

#ifndef PAGE_H
#define PAGE_H

#include "enums.h"
#include "filterproxymodel.h"
#include "itemstreemodel.h"
#include "ui_page.h"

#include "kdcrmdata/enumdefinitions.h"

#include <Akonadi/Collection>
#include <Akonadi/Item>

#include <QWidget>

namespace Akonadi
{
class ChangeRecorder;
class EntityMimeTypeFilterModel;
class Item;
}

class DetailsDialog;
class ItemDataExtractor;
class KJob;
class KJobProgressTracker;
class LinkedItemsRepository;
class QPoint;

class Page : public QWidget
{
    Q_OBJECT
public:
    explicit Page(QWidget *parent, const QString &mimeType, DetailsType type);

    ~Page();

    QString mimeType() const { return mMimeType; }
    DetailsType detailsType() const { return mType; }
    void setCollection(const Akonadi::Collection& collection);
    Akonadi::Collection collection() const { return mCollection; }
    void setLinkedItemsRepository(LinkedItemsRepository *repo);
    bool queryClose();
    void openDialog(const QString &id);
    void openDialogForItem(const Akonadi::Item &item);
    void printReport();
    KJob *clearTimestamp();
Q_SIGNALS:

    void modelCreated(ItemsTreeModel *model);
    void statusMessage(const QString &);
    void modelLoaded(DetailsType type);
    void modelItemChanged(const Akonadi::Item &item);
    void synchronizeCollection(const Akonadi::Collection &collection);
    void openObject(DetailsType type, const QString &id);
    void onlineStatusChanged(bool online);

public Q_SLOTS:
    void slotOnlineStatusChanged(bool online);
    void slotResourceSelectionChanged(const QByteArray &identifier);

protected:
    virtual ItemDataExtractor *itemDataExtractor() const = 0;
    virtual void handleNewRows(int start, int end, bool emitChanges) = 0;
    virtual void handleRemovedRows(int start, int end, bool initialLoadingDone);
    virtual void handleItemChanged(const Akonadi::Item &item);

    inline Akonadi::EntityTreeView *treeView() {
        return mUi.treeView;
    }
    ItemsTreeModel *itemsTreeModel() const {
        return mItemsTreeModel;
    }
    void setFilter(FilterProxyModel *filter);

    virtual void setupModel();

    void insertFilterWidget(QWidget *widget);

private Q_SLOTS:
    void slotNewClicked();
    void slotRemoveItem();
    void slotVisibleRowCountChanged();
    void slotRowsInserted(const QModelIndex &, int start, int end);
    void slotRowsAboutToBeRemoved(const QModelIndex &, int start, int end);
    void slotDataChanged(const QModelIndex &, const QModelIndex &);
    void slotResetSearch();
    void slotReloadCollection();
    void slotCollectionChanged(const Akonadi::Collection &collection, const QSet<QByteArray> &attributeNames);
    void slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray> &partIdentifiers);
    void slotItemDoubleClicked(const Akonadi::Item &item);
    void slotItemSaved();
    void slotItemContextMenuRequested(const QPoint &pos);
    void slotOpenUrl();
    void slotCopyLink();
    void slotOpenEmailClient();
    void slotUnregisterDetailsDialog();
    void slotChangeFields();
    void slotDeleteJobResult(KJob *job);

private:
    virtual QString reportTitle() const = 0;
    QString reportSubTitle(int count) const;
    virtual QMap<QString, QString> dataForNewObject() { return QMap<QString, QString>(); }
    void initialize();
    void readSupportedFields();
    void readEnumDefinitionAttributes();
    void retrieveResourceUrl();

    DetailsDialog *createDetailsDialog();
    DetailsDialog *openedDialogForItem(const Akonadi::Item &item);

private:
    QString mMimeType;
    DetailsType mType;
    FilterProxyModel *mFilter;
    Akonadi::ChangeRecorder *mChangeRecorder;
    ItemsTreeModel *mItemsTreeModel;
    Akonadi::Collection mCollection;
    Ui_page mUi;
    QByteArray mResourceIdentifier;
    QSet<DetailsDialog*> mDetailsDialogs;

    // Things we keep around so we can set them on the details dialog when creating it
    QString mResourceBaseUrl;
    QStringList mSupportedFields;
    QUrl mCurrentItemUrl;
    LinkedItemsRepository *mLinkedItemsRepository;
    EnumDefinitions mEnumDefinitions;
    bool mOnline;

    bool mInitialLoadingDone;
    Akonadi::EntityMimeTypeFilterModel *mFilterModel;

    QStringList mSelectedEmails;

    KJobProgressTracker *mJobProgressTracker;
};

#endif
