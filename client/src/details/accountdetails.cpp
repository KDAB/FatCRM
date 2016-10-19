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

#include "accountdetails.h"
#include "ui_accountdetails.h"
#include "fatcrm_client_debug.h"

#include "accountdataextractor.h"
#include "documentswindow.h"
#include "linkeditemsrepository.h"
#include "noteswindow.h"
#include "referenceddatamodel.h"

#include "kdcrmdata/sugaraccount.h"
#include "kdcrmdata/kdcrmfields.h"
#include "kdcrmdata/kdcrmutils.h"

#include <QDesktopServices>

AccountDetails::AccountDetails(QWidget *parent)
    : Details(Account, parent), mUi(new Ui::AccountDetails), mDataExtractor(new AccountDataExtractor(this)), mLinkedItemsRepository(Q_NULLPTR)
{
    mUi->setupUi(this);
    mUi->urllabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

    mUi->name->setObjectName(KDCRMFields::name());
    mUi->website->setObjectName(KDCRMFields::website());
    mUi->ticker_symbol->setObjectName(KDCRMFields::tickerSymbol());
    mUi->parent_id->setObjectName(KDCRMFields::parentId());
    mUi->ownership->setObjectName(KDCRMFields::ownership());
    mUi->industry->setObjectName(KDCRMFields::industry());
    mUi->account_type->setObjectName(KDCRMFields::accountType());
    mUi->assigned_user_id->setObjectName(KDCRMFields::assignedUserId());
    mUi->phone_office->setObjectName(KDCRMFields::accountPhoneWork());
    mUi->phone_fax->setObjectName(KDCRMFields::phoneFax());
    mUi->phone_alternate->setObjectName(KDCRMFields::accountPhoneOther());
    mUi->employees->setObjectName(KDCRMFields::employees());
    mUi->rating->setObjectName(KDCRMFields::rating());
    mUi->sic_code->setObjectName(KDCRMFields::sicCode());
    mUi->annual_revenue->setObjectName(KDCRMFields::annualRevenue());
    mUi->email1->setObjectName(KDCRMFields::email1());
    mUi->visma_id_c->setObjectName(KDCRMFields::vismaId());
    mUi->accounting_c->setObjectName(KDCRMFields::accounting());
    mUi->vat_no_c->setObjectName(KDCRMFields::vatNo());
    mUi->billing_address_street->setObjectName(KDCRMFields::billingAddressStreet());
    mUi->billing_address_city->setObjectName(KDCRMFields::billingAddressCity());
    mUi->billing_address_state->setObjectName(KDCRMFields::billingAddressState());
    mUi->billing_address_postalcode->setObjectName(KDCRMFields::billingAddressPostalcode());
    mUi->billing_address_country->setObjectName(KDCRMFields::billingAddressCountry());
    mUi->shipping_address_street->setObjectName(KDCRMFields::shippingAddressStreet());
    mUi->shipping_address_city->setObjectName(KDCRMFields::shippingAddressCity());
    mUi->shipping_address_state->setObjectName(KDCRMFields::shippingAddressState());
    mUi->shipping_address_postalcode->setObjectName(KDCRMFields::shippingAddressPostalcode());
    mUi->shipping_address_country->setObjectName(KDCRMFields::shippingAddressCountry());

    QCompleter *countriesCompleter = createCountriesCompleter();
    mUi->billing_address_country->setCompleter(countriesCompleter);
    mUi->shipping_address_country->setCompleter(countriesCompleter);

    initialize();
}

AccountDetails::~AccountDetails()
{
    delete mUi;
}

ItemDataExtractor *AccountDetails::itemDataExtractor() const
{
    return mDataExtractor;
}

void AccountDetails::slotBillingAddressCountryEditingFinished()
{
    mUi->billing_address_country->setText(KDCRMUtils::canonicalCountryName(mUi->billing_address_country->text()));
}

void AccountDetails::slotShippingAddressCountryEditingFinished()
{
    mUi->shipping_address_country->setText(KDCRMUtils::canonicalCountryName(mUi->shipping_address_country->text()));
}

void AccountDetails::on_viewNotesButton_clicked()
{
    const QString accountId = id();
    const QVector<SugarNote> notes = mLinkedItemsRepository->notesForAccount(accountId);
    qCDebug(FATCRM_CLIENT_LOG) << notes.count() << "notes found for account" << accountId;
    const QVector<SugarEmail> emails = mLinkedItemsRepository->emailsForAccount(accountId);
    qCDebug(FATCRM_CLIENT_LOG) << emails.count() << "emails found for account" << accountId;
    NotesWindow *dlg = new NotesWindow(nullptr);
    dlg->setWindowTitle(i18n("Notes for account %1", name()));
    foreach(const SugarNote &note, notes) {
        dlg->addNote(note);
    }
    foreach(const SugarEmail &email, emails) {
        dlg->addEmail(email);
    }
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void AccountDetails::on_viewDocumentsButton_clicked()
{
    const QString accountId = id();

    DocumentsWindow *dlg = new DocumentsWindow(nullptr);
    dlg->setWindowTitle(i18n("Documents for account %1", name()));

    dlg->setResourceIdentifier(resourceIdentifier());
    dlg->setLinkedItemsRepository(mLinkedItemsRepository);
    dlg->loadDocumentsFor(accountId, DocumentsWindow::Account);

    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void AccountDetails::initialize()
{
    setObjectName(QStringLiteral("accountDetails"));

    ReferencedDataModel::setModelForCombo(mUi->parent_id, AccountRef);
    //ReferencedDataModel::setModelForCombo(mUi->campaign_id, CampaignRef);
    ReferencedDataModel::setModelForCombo(mUi->assigned_user_id, AssignedToRef);

    connect(mUi->billing_address_country, SIGNAL(editingFinished()), SLOT(slotBillingAddressCountryEditingFinished()));
    connect(mUi->shipping_address_country, SIGNAL(editingFinished()), SLOT(slotShippingAddressCountryEditingFinished()));
    connect(mUi->visitWebsiteButton, &QPushButton::clicked, this, [this](){
        QDesktopServices::openUrl(QUrl(mUi->website->text()));
    });
}

QMap<QString, QString> AccountDetails::data(const Akonadi::Item &item) const
{
    Q_ASSERT(item.hasPayload<SugarAccount>());
    SugarAccount account = item.payload<SugarAccount>();
    return account.data();
}

void AccountDetails::updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const
{
    QMap<QString, QString> canonicalData(data);

    canonicalData.insert(KDCRMFields::billingAddressCountry(),
                         KDCRMUtils::canonicalCountryName(data.value(KDCRMFields::billingAddressCountry())));
    canonicalData.insert(KDCRMFields::shippingAddressCountry(),
                         KDCRMUtils::canonicalCountryName(data.value(KDCRMFields::shippingAddressCountry())));

    SugarAccount account;
    if (item.hasPayload<SugarAccount>()) {
        account = item.payload<SugarAccount>();
    }
    account.setData(canonicalData);

    item.setMimeType(SugarAccount::mimeType());
    item.setPayload<SugarAccount>(account);
}

void AccountDetails::setDataInternal(const QMap<QString, QString> &)
{
    fillComboBox(mUi->industry, KDCRMFields::industry());
    fillComboBox(mUi->account_type, KDCRMFields::accountType());
    const QUrl url = itemDataExtractor()->itemUrl(resourceBaseUrl(), id());
    if (url.isValid())
        mUi->urllabel->setText(QStringLiteral("<a href=\"%1\">Open Account in Web Browser</a>").arg(url.toString()));

    const QString accountId = id();
    {
        const int notes = accountId.isEmpty() ? 0 : mLinkedItemsRepository->notesForAccount(accountId).count() + mLinkedItemsRepository->emailsForAccount(accountId).count();
        mUi->viewNotesButton->setEnabled(notes > 0);
        const QString buttonText = (notes == 0) ? i18n("View Notes") : i18np("View 1 Note", "View %1 Notes", notes);
        mUi->viewNotesButton->setText(buttonText);
    }
    {
        const int documents = accountId.isEmpty() ? 0 : mLinkedItemsRepository->documentsForAccount(accountId).count();
        const QString buttonText = (documents == 0) ? i18n("Attach Document") : i18np("View 1 Document", "View %1 Documents", documents);
        mUi->viewDocumentsButton->setText(buttonText);
    }
}
