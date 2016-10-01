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

#ifndef CONTACTDETAILS_H
#define CONTACTDETAILS_H

#include "details.h"

namespace KContacts
{
class Addressee;
}

namespace Ui
{
class ContactDetails;
}

class ContactDataExtractor;
class LinkedItemsRepository;

class ContactDetails : public Details
{
    Q_OBJECT
public:
    explicit ContactDetails(QWidget *parent = Q_NULLPTR);

    ~ContactDetails() override;

    void setLinkedItemsRepository(LinkedItemsRepository *repo) Q_DECL_OVERRIDE { mLinkedItemsRepository = repo; }
    ItemDataExtractor *itemDataExtractor() const Q_DECL_OVERRIDE;
    void setItemsTreeModel(ItemsTreeModel *model) Q_DECL_OVERRIDE;


private:
    Ui::ContactDetails *mUi;

private:
    void initialize();
    QMap<QString, QString> data(const Akonadi::Item &item) const Q_DECL_OVERRIDE;
    void updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const Q_DECL_OVERRIDE;
    void setDataInternal(const QMap<QString, QString> &data) Q_DECL_OVERRIDE;

    QMap<QString, QString> contactData(const KContacts::Addressee &contact) const;

private Q_SLOTS:
    void slotSetBirthday();
    void slotClearDate();
    void slotAccountActivated();
    void on_buttonOpenAccount_clicked();
    void on_viewNotesButton_clicked();
    void slotSelectAccount();
    void slotAccountSelected(const QString &accountId);
    void slotPrimaryAddressCountryEditingFinished();
    void slotOtherAddressCountryEditingFinished();
    void slotEnableMailToPrimary();
    void slotEnableMailToOther();
    void slotMailToPrimary();
    void slotMailToOther();

private:
    ContactDataExtractor *mDataExtractor;
    LinkedItemsRepository *mLinkedItemsRepository;
};

#endif /* CONTACTDETAILS_H */

