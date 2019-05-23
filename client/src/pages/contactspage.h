/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef CONTACTSPAGE_H
#define CONTACTSPAGE_H

#include "page.h"

class ContactDataExtractor;

class ContactsPage : public Page
{
    Q_OBJECT
public:
    explicit ContactsPage(QWidget *parent = nullptr);

    ~ContactsPage() override;

public slots:
    void slotOpenFutureContact(Akonadi::Item::Id id);

protected:
    QString reportTitle() const override;
    ItemDataExtractor *itemDataExtractor() const override;

    void handleNewRows(int start, int end, bool emitChanges) override;
    void handleItemChanged(const Akonadi::Item &item) override;
    void handleRemovedRows(int start, int end, bool initialLoadingDone) override;

private:
    std::unique_ptr<ContactDataExtractor> mDataExtractor;
    QVector<Akonadi::Item::Id> mPendingContactsToOpen;
};

#endif /* CONTACTSPAGE_H */
