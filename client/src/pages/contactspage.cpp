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

#include "contactspage.h"
#include "contactdataextractor.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "referenceddata.h"
#include "linkeditemsrepository.h"
#include "contactfilterwidget.h"

#include <AkonadiWidgets/EntityTreeView>

#include <KContacts/Address>
#include <KContacts/Addressee>

#include <KLocalizedString>

using namespace Akonadi;

ContactsPage::ContactsPage(bool showGDPR, QWidget *parent)
    : Page(parent, KContacts::Addressee::mimeType(), DetailsType::Contact), mDataExtractor(new ContactDataExtractor)
{
    auto *filterProxyModel = new FilterProxyModel(DetailsType::Contact, this);
    setFilter(filterProxyModel);
    treeView()->setSelectionMode(QAbstractItemView::ExtendedSelection);

    mFilterUiWidget = new ContactFilterWidget(filterProxyModel, showGDPR, this);
    insertFilterWidget(mFilterUiWidget);

    //connect(mFilterUiWidget, &ContactFilterWidget::filterUpdated, this, &ContactsPage::slotDefaultOppFilterUpdated);
}

ContactsPage::~ContactsPage()
{
}

void ContactsPage::slotOpenFutureContact(Akonadi::Item::Id id)
{
    mPendingContactsToOpen.append(id);
}

QString ContactsPage::reportTitle() const
{
    return i18n("List of Contacts");
}

ItemDataExtractor *ContactsPage::itemDataExtractor() const
{
    return mDataExtractor.get();
}

void ContactsPage::handleNewRows(int start, int end, bool emitChanges)
{
    //qDebug(); QElapsedTimer dt; dt.start();
    ItemsTreeModel *treeModel = itemsTreeModel();
    QMap<QString, QString> contactRefMap, assignedToRefMap;
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = treeModel->index(row, 0);
        const Item item = treeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<KContacts::Addressee>()) {
            const KContacts::Addressee addressee = item.payload<KContacts::Addressee>();
            const QString id = addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-ContactId"));
            if (id.isEmpty()) { // newly created, not ID yet
                continue;
            }
            const QString fullName = tr("%1 %2 (%3)", "GIVEN_NAME FAMILY_NAME (ORG)").arg(addressee.givenName(), addressee.familyName(), addressee.organization());
            contactRefMap.insert(id, fullName);
            assignedToRefMap.insert(addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AssignedUserId")), addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AssignedUserName")));
            linkedItemsRepository()->addContact(addressee);
        }
    }
    ReferencedData::instance(ContactRef)->addMap(contactRefMap, emitChanges);
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap, emitChanges);
    //qDebug() << "done," << dt.elapsed() << "ms";
}

void ContactsPage::handleItemChanged(const Item &item)
{
    Q_ASSERT(item.hasPayload<KContacts::Addressee>());
    const KContacts::Addressee addressee = item.payload<KContacts::Addressee>();
    const QString fullName = addressee.givenName() + ' ' + addressee.familyName();
    const QString id = addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-ContactId"));
    if (!id.isEmpty()) {
        const int idx = mPendingContactsToOpen.indexOf(item.id());
        if (idx > -1) {
            mPendingContactsToOpen.remove(idx);
            openWidgetForItem(item, DetailsType::Contact);
        }
        ReferencedData::instance(ContactRef)->setReferencedData(id, fullName);
        ReferencedData::instance(AssignedToRef)->setReferencedData(addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AssignedUserId")), addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AssignedUserName")));
        linkedItemsRepository()->updateContact(addressee);
    }
}

void ContactsPage::handleRemovedRows(int start, int end, bool initialLoadingDone)
{
    Q_UNUSED(initialLoadingDone);
    ItemsTreeModel *treeModel = itemsTreeModel();
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = treeModel->index(row, 0);
        const Item item = treeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<KContacts::Addressee>()) {
            const KContacts::Addressee contact = item.payload<KContacts::Addressee>();
            linkedItemsRepository()->removeContact(contact);
        }
    }
}
