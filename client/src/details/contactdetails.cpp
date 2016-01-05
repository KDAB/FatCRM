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

#include "contactdetails.h"

#include "accountrepository.h"
#include "editcalendarbutton.h"
#include "referenceddatamodel.h"
#include "ui_contactdetails.h"

#include "kdcrmutils.h"
#include "kdcrmfields.h"

#include <KABC/Address>
#include <KABC/Addressee>

#include <QMessageBox>

ContactDetails::ContactDetails(QWidget *parent)
    : Details(Contact, parent), mUi(new Ui::ContactDetails)
{
    mUi->setupUi(this);
    mUi->urllabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    initialize();
}

ContactDetails::~ContactDetails()
{
    delete mUi;
}

void ContactDetails::initialize()
{
    ReferencedDataModel::setModelForCombo(mUi->account_id, AccountRef);
    //ReferencedDataModel::setModelForCombo(mUi->campaign, CampaignRef);
    ReferencedDataModel::setModelForCombo(mUi->reports_to_id, ContactRef);
    ReferencedDataModel::setModelForCombo(mUi->assigned_user_id, AssignedToRef);

    connect(mUi->clearDateButton, SIGNAL(clicked()), this, SLOT(slotClearDate()));
    connect(mUi->calendarButton->calendarWidget(), SIGNAL(clicked(QDate)),
            this, SLOT(slotSetBirthday()));
    connect(mUi->account_id, SIGNAL(activated(int)), this, SLOT(slotAccountActivated()));
}

QUrl ContactDetails::itemUrl() const
{
    const QString baseUrl = resourceBaseUrl();
    if (!baseUrl.isEmpty() && !id().isEmpty()) {
        const QString url = baseUrl + "?action=DetailView&module=Contacts&record=" + id();
        return QUrl(url);
    }
    return QUrl();
}

void ContactDetails::slotSetBirthday()
{
    // TODO FIXME: use QDateEdit [or remove - who cares about birthdays...]
    mUi->birthdate->setText(KDCRMUtils::dateToString(mUi->calendarButton->calendarWidget()->selectedDate()));
    mUi->calendarButton->calendarWidget()->setSelectedDate(QDate::currentDate());
    mUi->calendarButton->calendarDialog()->close();
}

void ContactDetails::slotClearDate()
{
    mUi->birthdate->clear();
}

void ContactDetails::slotAccountActivated()
{
    const QString accountId = currentAccountId();
    if (accountId.isEmpty()) {
        return;
    }
    const SugarAccount account = AccountRepository::instance()->accountById(accountId);
    if (account.isEmpty()) {
        return;
    }

    const bool copyAddressFromAccount = (mUi->primaryAddressStreet->toPlainText().isEmpty() &&
                                        mUi->primaryAddressCity->text().isEmpty() &&
                                        mUi->primaryAddressState->text().isEmpty() &&
                                        mUi->primaryAddressPostalcode->text().isEmpty() &&
                                        mUi->primaryAddressCountry->text().isEmpty());

    const bool copyPhoneWorkFromAccount = mUi->phone_work->text().isEmpty();

    if (!copyAddressFromAccount && !copyPhoneWorkFromAccount) {
        return;
    }

    QString title, message;

    if (copyAddressFromAccount && !copyPhoneWorkFromAccount) {
        title = i18n("Copy address from account?");
        message = i18n("Do you want to copy the address from the account '%1' into this contact?", account.name());
    } else if (!copyAddressFromAccount && copyPhoneWorkFromAccount) {
        title = i18n("Copy phone from account?");
        message = i18n("Do you want to copy the phone number from the account '%1' into this contact?", account.name());
    } else {
        title = i18n("Copy address and phone from account?");
        message = i18n("Do you want to copy the address and phone number from the account '%1' into this contact?", account.name());
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Yes |
                              QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Cancel) {
        return;
    }

    if (copyAddressFromAccount) {
        if (!account.shippingAddressStreet().isEmpty()) {
            mUi->primaryAddressStreet->setPlainText(account.shippingAddressStreet());
            mUi->primaryAddressCity->setText(account.shippingAddressCity());
            mUi->primaryAddressState->setText(account.shippingAddressState());
            mUi->primaryAddressPostalcode->setText(account.shippingAddressPostalcode());
            mUi->primaryAddressCountry->setText(account.shippingAddressCountry());
        } else {
            mUi->primaryAddressStreet->setPlainText(account.billingAddressStreet());
            mUi->primaryAddressCity->setText(account.billingAddressCity());
            mUi->primaryAddressState->setText(account.billingAddressState());
            mUi->primaryAddressPostalcode->setText(account.billingAddressPostalcode());
            mUi->primaryAddressCountry->setText(account.billingAddressCountry());
        }
    }

    if (copyPhoneWorkFromAccount) {
        mUi->phone_work->setText(account.phoneOffice());
    }
}

QMap<QString, QString> ContactDetails::data(const Akonadi::Item &item) const
{
    Q_ASSERT(item.hasPayload<KABC::Addressee>());
    KABC::Addressee contact = item.payload<KABC::Addressee>();
    return contactData(contact);
}

QMap<QString, QString> ContactDetails::contactData(const KABC::Addressee &addressee) const
{
    QMap<QString, QString> data;
    data.insert(KDCRMFields::salutation(), addressee.custom("FATCRM", "X-Salutation"));
    data.insert(KDCRMFields::firstName(), addressee.givenName());
    data.insert(KDCRMFields::lastName(), addressee.familyName());
    data.insert(KDCRMFields::title(), addressee.title());
    data.insert(KDCRMFields::department(), addressee.department());
    data.insert(KDCRMFields::accountName(), addressee.organization());
    data.insert(KDCRMFields::accountId(), addressee.custom("FATCRM", "X-AccountId"));
    data.insert(KDCRMFields::email1(), addressee.preferredEmail());
    QStringList emails = addressee.emails();
    emails.removeAll(addressee.preferredEmail());
    if (!emails.isEmpty()) {
        data.insert(KDCRMFields::email2(), emails.at(0));
    }
    data.insert(KDCRMFields::phoneHome(), addressee.phoneNumber(KABC::PhoneNumber::Home).number());
    data.insert(KDCRMFields::phoneMobile(), addressee.phoneNumber(KABC::PhoneNumber::Cell).number());
    data.insert(KDCRMFields::phoneWork(), addressee.phoneNumber(KABC::PhoneNumber::Work).number());
    data.insert(KDCRMFields::phoneOther(), addressee.phoneNumber(KABC::PhoneNumber::Car).number());
    data.insert(KDCRMFields::phoneFax(), addressee.phoneNumber(KABC::PhoneNumber::Fax).number());

    const KABC::Address address = addressee.address(KABC::Address::Work | KABC::Address::Pref);
    data.insert(KDCRMFields::primaryAddressStreet(), address.street());
    data.insert(KDCRMFields::primaryAddressCity(), address.locality());
    data.insert(KDCRMFields::primaryAddressState(), address.region());
    data.insert(KDCRMFields::primaryAddressPostalcode(), address.postalCode());
    data.insert(KDCRMFields::primaryAddressCountry(), address.country());

    const KABC::Address other = addressee.address(KABC::Address::Home);
    data.insert(KDCRMFields::altAddressStreet(), other.street());
    data.insert(KDCRMFields::altAddressCity(), other.locality());
    data.insert(KDCRMFields::altAddressState(), other.region());
    data.insert(KDCRMFields::altAddressPostalcode(), other.postalCode());
    data.insert(KDCRMFields::altAddressCountry(), other.country());
    data.insert(KDCRMFields::birthdate(), KDCRMUtils::dateToString(addressee.birthday().date()));
    data.insert(KDCRMFields::assistant(), addressee.custom("KADDRESSBOOK", "X-AssistantsName"));
    data.insert(KDCRMFields::phoneAssistant(), addressee.custom("FATCRM", "X-AssistantsPhone"));
    data.insert(KDCRMFields::leadSource(), addressee.custom("FATCRM", "X-LeadSourceName"));
    data.insert(KDCRMFields::campaign(), addressee.custom("FATCRM", "X-CampaignName"));
    data.insert(KDCRMFields::assignedUserId(), addressee.custom("FATCRM", "X-AssignedUserId"));
    data.insert(KDCRMFields::assignedUserName(), addressee.custom("FATCRM", "X-AssignedUserName"));
    data.insert(KDCRMFields::reportsToId(), addressee.custom("FATCRM", "X-ReportsToUserId"));
    data.insert(KDCRMFields::reportsTo(), addressee.custom("FATCRM", "X-ReportsToUserName"));
    data.insert(KDCRMFields::doNotCall(), addressee.custom("FATCRM", "X-DoNotCall"));
    data.insert(KDCRMFields::description(), addressee.note());
    data.insert(KDCRMFields::modifiedByName(), addressee.custom("FATCRM", "X-ModifiedByName"));
    data.insert(KDCRMFields::dateModified(), addressee.custom("FATCRM", "X-DateModified"));
    data.insert(KDCRMFields::dateEntered(), addressee.custom("FATCRM", "X-DateCreated"));
    data.insert(KDCRMFields::createdByName(), addressee.custom("FATCRM", "X-CreatedByName"));
    data.insert(KDCRMFields::modifiedUserId(), addressee.custom("FATCRM", "X-ModifiedUserId"));
    data.insert(KDCRMFields::id(), addressee.custom("FATCRM", "X-ContactId"));
    data.insert(KDCRMFields::opportunityRoleFields(), addressee.custom("FATCRM", "X-OpportunityRoleFields"));
    data.insert(KDCRMFields::cAcceptStatusFields(), addressee.custom("FATCRM", "X-CacceptStatusFields"));
    data.insert(KDCRMFields::mAcceptStatusFields(), addressee.custom("FATCRM", "X-MacceptStatusFields"));
    data.insert(KDCRMFields::deleted(), addressee.custom("FATCRM", "X-Deleted"));
    data.insert(KDCRMFields::createdBy(), addressee.custom("FATCRM", "X-CreatedById"));
    return data;
}

void ContactDetails::updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const
{
    KABC::Addressee addressee;
    if (item.hasPayload<KABC::Addressee>()) {
        // start from the existing item (to keep custom fields not shown in GUI, I suppose)
        addressee = item.payload<KABC::Addressee>();
        // but for anything that uses insert... below, start by clearing the lists.
        foreach (const KABC::Address &addr, addressee.addresses())
            addressee.removeAddress(addr);
        foreach (const KABC::PhoneNumber &nr, addressee.phoneNumbers())
            addressee.removePhoneNumber(nr);
    }

    addressee.setGivenName(data.value(KDCRMFields::firstName()));
    addressee.setFamilyName(data.value(KDCRMFields::lastName()));
    addressee.setTitle(data.value(KDCRMFields::title()));
    addressee.setDepartment(data.value(KDCRMFields::department()));
    addressee.setOrganization(data.value(KDCRMFields::accountName()));
    addressee.insertCustom("FATCRM", "X-AccountId", data.value(KDCRMFields::accountId()));
    QStringList emails;
    emails << data.value(KDCRMFields::email1()) // first one is preferred one
           << data.value(KDCRMFields::email2());
    addressee.setEmails(emails);
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value(KDCRMFields::phoneHome()), KABC::PhoneNumber::Home));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value(KDCRMFields::phoneMobile()), KABC::PhoneNumber::Cell));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value(KDCRMFields::phoneWork()), KABC::PhoneNumber::Work));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value(KDCRMFields::phoneOther()), KABC::PhoneNumber::Car));
    addressee.insertPhoneNumber(KABC::PhoneNumber(data.value(KDCRMFields::phoneFax()), KABC::PhoneNumber::Work | KABC::PhoneNumber::Fax));

    KABC::Address primaryAddress;
    primaryAddress.setType(KABC::Address::Work | KABC::Address::Pref);
    primaryAddress.setStreet(data.value(KDCRMFields::primaryAddressStreet()));
    primaryAddress.setLocality(data.value(KDCRMFields::primaryAddressCity()));
    primaryAddress.setRegion(data.value(KDCRMFields::primaryAddressState()));
    primaryAddress.setPostalCode(data.value(KDCRMFields::primaryAddressPostalcode()));
    primaryAddress.setCountry(data.value(KDCRMFields::primaryAddressCountry()));
    addressee.insertAddress(primaryAddress);

    KABC::Address otherAddress;
    otherAddress.setType(KABC::Address::Home);
    otherAddress.setStreet(data.value(KDCRMFields::altAddressStreet()));
    otherAddress.setLocality(data.value(KDCRMFields::altAddressCity()));
    otherAddress.setRegion(data.value(KDCRMFields::altAddressState()));
    otherAddress.setPostalCode(data.value(KDCRMFields::altAddressPostalcode()));
    otherAddress.setCountry(data.value(KDCRMFields::altAddressCountry()));
    addressee.insertAddress(otherAddress);

    addressee.setBirthday(QDateTime(KDCRMUtils::dateFromString(data.value(KDCRMFields::birthdate()))));

    addressee.setNote(data.value(KDCRMFields::description()));
    addressee.insertCustom("KADDRESSBOOK", "X-AssistantsName", data.value(KDCRMFields::assistant()));
    addressee.insertCustom("FATCRM", "X-AssistantsPhone", data.value(KDCRMFields::phoneAssistant()));
    addressee.insertCustom("FATCRM", "X-LeadSourceName", data.value(KDCRMFields::leadSource()));
    addressee.insertCustom("FATCRM", "X-CampaignName", data.value(KDCRMFields::campaign()));
    addressee.insertCustom("FATCRM", "X-CampaignId", data.value(KDCRMFields::campaignId()));
    addressee.insertCustom("FATCRM", "X-CacceptStatusFields", data.value(KDCRMFields::cAcceptStatusFields()));
    addressee.insertCustom("FATCRM", "X-MacceptStatusFields", data.value(KDCRMFields::mAcceptStatusFields()));
    addressee.insertCustom("FATCRM", "X-AssignedUserId", data.value(KDCRMFields::assignedUserId()));
    addressee.insertCustom("FATCRM", "X-AssignedUserName", data.value(KDCRMFields::assignedUserName()));
    addressee.insertCustom("FATCRM", "X-ReportsToUserName", data.value(KDCRMFields::reportsTo()));
    addressee.insertCustom("FATCRM", "X-ReportsToUserId", data.value(KDCRMFields::reportsToId()));
    addressee.insertCustom("FATCRM", "X-OpportunityRoleFields", data.value(KDCRMFields::opportunityRoleFields()));
    addressee.insertCustom("FATCRM", "X-ModifiedByName", data.value(KDCRMFields::modifiedByName()));
    addressee.insertCustom("FATCRM", "X-DateModified", data.value(KDCRMFields::dateModified()));
    addressee.insertCustom("FATCRM", "X-ModifiedUserId", data.value(KDCRMFields::modifiedUserId()));
    addressee.insertCustom("FATCRM", "X-DateCreated", data.value(KDCRMFields::dateEntered()));
    addressee.insertCustom("FATCRM", "X-CreatedByName", data.value(KDCRMFields::createdByName()));
    addressee.insertCustom("FATCRM", "X-CreatedById", data.value(KDCRMFields::createdBy()));
    addressee.insertCustom("FATCRM", "X-ContactId", data.value(KDCRMFields::id()));
    addressee.insertCustom("FATCRM", "X-Salutation", data.value(KDCRMFields::salutation()));
    addressee.insertCustom("FATCRM", "X-Deleted", data.value(KDCRMFields::deleted()));
    addressee.insertCustom("FATCRM", "X-DoNotCall", data.value(KDCRMFields::doNotCall()));

    item.setMimeType(KABC::Addressee::mimeType());
    item.setPayload<KABC::Addressee>(addressee);
}

void ContactDetails::setDataInternal(const QMap<QString, QString> &) const
{
    fillComboBox(mUi->salutation, KDCRMFields::salutation());
    fillComboBox(mUi->lead_source, KDCRMFields::leadSource());

    const QUrl url = itemUrl();
    if (url.isValid())
        mUi->urllabel->setText(QString("<a href=\"%1\">Open Contact in Web Browser</a>").arg(url.toString()));
}

void ContactDetails::on_buttonOpenAccount_clicked()
{
    const QString accountId = currentAccountId();
    emit openObject(Account, accountId);
}
