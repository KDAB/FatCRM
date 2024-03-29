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

#include "accountspage.h"
#include "accountdataextractor.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "referenceddata.h"
#include "fatcrm_client_debug.h"

#include "kdcrmdata/sugaraccount.h"

#include <Akonadi/EntityTreeView>
#include <KLocalizedString>
#include <QMenu>

using namespace Akonadi;

AccountsPage::AccountsPage(QWidget *parent)
    : Page(parent, SugarAccount::mimeType(), DetailsType::Account), mDataExtractor(new AccountDataExtractor)
{
    setFilter(new FilterProxyModel(DetailsType::Account, this));
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
    //qCDebug(FATCRM_CLIENT_LOG) << this << start << end;
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
    //qCDebug(FATCRM_CLIENT_LOG) << "done," << dt.elapsed() << "ms";
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

void AccountsPage::handleItemChanged(const Item &item)
{
    Q_ASSERT(item.hasPayload<SugarAccount>());
    const SugarAccount account = item.payload<SugarAccount>();
    const QString id = account.id();
    if (id.isEmpty()) {
        return;
    }
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
}

QMenu *AccountsPage::createContextMenu(const QPoint &pos)
{
    QMenu *menu = Page::createContextMenu(pos);
    if (!menu) {
        return nullptr;
    }

    if (treeView()->selectionModel()->selectedRows().size() == 1) {
        menu->addSeparator();
        menu->addAction(i18n("Create new Opportunity"), this, &AccountsPage::createNewOpportunityForSelectedAccount);
    }

    return menu;
}

void AccountsPage::createNewOpportunityForSelectedAccount()
{
    QModelIndex selectedAccount = treeView()->selectionModel()->selectedRows().at(0);
    if (selectedAccount.isValid()) {
        const Item item = selectedAccount.data(EntityTreeModel::ItemRole).value<Item>();
        const SugarAccount account = item.payload<SugarAccount>();
        emit requestNewOpportunity(account.id());
    }
}

ItemDataExtractor *AccountsPage::itemDataExtractor() const
{
    return mDataExtractor.get();
}
