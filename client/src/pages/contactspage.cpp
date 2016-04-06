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

#include "contactspage.h"
#include "contactdataextractor.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "referenceddata.h"

#include <KContacts/Address>
#include <KContacts/Addressee>

using namespace Akonadi;

ContactsPage::ContactsPage(QWidget *parent)
    : Page(parent, QString(KContacts::Addressee::mimeType()), Contact), mDataExtractor(new ContactDataExtractor(this))
{
    setFilter(new FilterProxyModel(Contact, this));
    treeView()->setSelectionMode(QAbstractItemView::ExtendedSelection);
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
    return mDataExtractor;
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
    //qDebug() << "done," << dt.elapsed() << "ms";
}

void ContactsPage::handleItemChanged(const Item &item)
{
    Q_ASSERT(item.hasPayload<KContacts::Addressee>());
    const KContacts::Addressee addressee = item.payload<KContacts::Addressee>();
    const QString fullName = addressee.givenName() + ' ' + addressee.familyName();
    const QString id = addressee.custom("FATCRM", "X-ContactId");
    if (!id.isEmpty()) {
        const int idx = mPendingContactsToOpen.indexOf(item.id());
        if (idx > -1) {
            mPendingContactsToOpen.remove(idx);
            openDialogForItem(item);
        }
        ReferencedData::instance(ContactRef)->setReferencedData(id, fullName);
        ReferencedData::instance(AssignedToRef)->setReferencedData(addressee.custom("FATCRM", "X-AssignedUserId"), addressee.custom("FATCRM", "X-AssignedUserName"));
    }
}
