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

#include "accountspage.h"
#include "accountdataextractor.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"

#include "kdcrmdata/sugaraccount.h"

#include <QDebug>
#include <referenceddata.h>

using namespace Akonadi;

AccountsPage::AccountsPage(QWidget *parent)
    : Page(parent, QString(SugarAccount::mimeType()), Account), mDataExtractor(new AccountDataExtractor(this))
{
    setFilter(new FilterProxyModel(Account, this));
}

AccountsPage::~AccountsPage()
{
}

QString AccountsPage::reportTitle() const
{
    return i18n("List of Accounts");
}

void AccountsPage::handleNewRows(int start, int end, bool emitChanges)
{
    //kDebug() << start << end;
    // QElapsedTimer dt; dt.start();
    ItemsTreeModel *treeModel = itemsTreeModel();
    QMap<QString, QString> accountRefMap, assignedToRefMap;
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = treeModel->index(row, 0);
        const Item item = treeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
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

void AccountsPage::handleRemovedRows(int start, int end, bool initialLoadingDone)
{
    ItemsTreeModel *treeModel = itemsTreeModel();
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = treeModel->index(row, 0);
        const Item item = treeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarAccount>()) {
            const SugarAccount account = item.payload<SugarAccount>();
            ReferencedData::instance(AccountRef)->removeReferencedData(account.id(), initialLoadingDone);

            AccountRepository::instance()->removeAccount(account);
        }
    }
}

ItemDataExtractor *AccountsPage::itemDataExtractor() const
{
    return mDataExtractor;
}
