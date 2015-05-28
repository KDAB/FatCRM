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

#ifndef PAGE_H
#define PAGE_H

#include "ui_page.h"
#include "enums.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"

#include <akonadi/changerecorder.h>
#include <akonadi/collection.h>

#include <QWidget>

namespace Akonadi
{
class ChangeRecorder;
class Item;
class Collection;
}

class Details;
class DetailsDialog;
class DetailsWidget;
class KJob;
class QAction;
class QModelIndex;
class NotesRepository;

class Page : public QWidget
{
    Q_OBJECT
public:
    explicit Page(QWidget *parent, const QString &mimeType, DetailsType type);

    ~Page();

    QString mimeType() const { return mMimeType; }
    void setCollection(const Akonadi::Collection& collection);
    void setNotesRepository(NotesRepository *repo);
    void setModificationsIgnored(bool b);

    QAction *showDetailsAction(const QString &title) const;

    bool showsDetails() const;
    void printReport();
    void retrieveResourceUrl();

Q_SIGNALS:
    void modelCreated(ItemsTreeModel *model);
    void statusMessage(const QString &);
    void showDetailsChanged(bool on);
    void modelItemChanged(const Akonadi::Item &item);
    void synchronizeCollection(const Akonadi::Collection &collection);
    void ignoreModifications(bool ignore); // emitted while loading reference data

public Q_SLOTS:
    void showDetails(bool on);

protected:
    inline Akonadi::EntityTreeView *treeView()
    {
        return mUi.treeView;
    }
    void setFilter(FilterProxyModel *filter)
    {
        mFilter = filter;
    }

    virtual void setupModel();

    void insertFilterWidget(QWidget *widget);

private Q_SLOTS:
    void slotResourceSelectionChanged(const QByteArray &identifier);
    void slotCurrentItemChanged(const QModelIndex &index);
    void slotNewClicked();
    void slotAddItem();
    void slotModifyItem(const Akonadi::Item &item);
    void slotRemoveItem();
    void slotVisibleRowCountChanged();
    void slotRowsInserted(const QModelIndex &, int, int);
    void slotDataChanged(const QModelIndex &, const QModelIndex &);
    void slotResetSearch();
    void slotReloadCollection();
    void slotCollectionChanged(const Akonadi::Collection &collection);
    void slotEnsureDetailsVisible();
    void slotItemDoubleClicked(const Akonadi::Item &item);
    void slotCreateJobResult(KJob *job);
    void slotModifyJobResult(KJob *job);
    void slotItemSaved(const Akonadi::Item &item);

private:
    virtual QString reportTitle() const = 0;
    Details *details() const;
    virtual QMap<QString, QString> dataForNewObject() { return QMap<QString, QString>(); }
    void initialize();
    bool askSave();
    // manages accounts combo box
    void addAccountsData();
    void addCampaignsData();
    void addContactsData();
    void addLeadsData();
    void addOpportunitiesData();
    void removeAccountsData(Akonadi::Item &item);
    void removeCampaignsData(Akonadi::Item &item);

    QString typeToString(const DetailsType &type) const;
    DetailsDialog *createDetailsDialog();

private:
    QString mMimeType;
    DetailsType mType;
    DetailsWidget *mDetailsWidget;
    FilterProxyModel *mFilter;
    Akonadi::ChangeRecorder *mChangeRecorder;
    ItemsTreeModel *mItemsTreeModel;
    Akonadi::Collection mCollection;
    QModelIndex mCurrentIndex;
    Ui_page mUi;
    QAction *mShowDetailsAction;
    QByteArray mResourceIdentifier;
    QString mResourceBaseUrl;
    NotesRepository *mNotesRepository;
};

#endif
