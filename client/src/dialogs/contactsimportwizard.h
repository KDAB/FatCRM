/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Tobias Koenig <tobias.koenig@kdab.com>

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

#ifndef CONTACTSIMPORTWIZARD_H
#define CONTACTSIMPORTWIZARD_H

#include "contactsset.h"
#include "details.h"

#include <AkonadiCore/Collection>
#include <AkonadiCore/Item>

#include <QWizard>

class AccountImportPage;
class ContactsImportPage;
class ItemsTreeModel;

class ContactsImportWizard : public QWizard
{
    Q_OBJECT

public:
    explicit ContactsImportWizard(QWidget *parent = nullptr);

    // intput
    void setAccountCollection(const Akonadi::Collection &collection);
    void setContactsCollection(const Akonadi::Collection &collection);
    void setImportedContacts(const QVector<ContactsSet> &contacts);
    void setContactsModel(ItemsTreeModel *model);

    void accept() override;
    void reject() override;

signals:
    void openFutureContact(Akonadi::Item::Id id);

private slots:
    void adaptWindowSize();
    void importItems(const QVector<Akonadi::Item> &items);
    void slotContactCreated(KJob *job);

private:
    AccountImportPage *mAccountImportPage;
    ContactsImportPage *mContactsImportPage;

    Akonadi::Collection mContactsCollection;
};

#endif
