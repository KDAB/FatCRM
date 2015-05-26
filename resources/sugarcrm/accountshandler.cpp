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

#include "accountshandler.h"

#include "kdcrmutils.h"
#include "sugarsession.h"
#include "sugarsoap.h"

using namespace KDSoapGenerated;
#include <akonadi/abstractdifferencesreporter.h>
#include <akonadi/collection.h>

#include <kabc/address.h>

#include <kdcrmdata/sugaraccount.h>

#include <KLocale>

#include <QHash>

typedef QString(SugarAccount::*valueGetter)() const;
typedef void (SugarAccount::*valueSetter)(const QString &);

class AccountAccessorPair
{
public:
    AccountAccessorPair(valueGetter get, valueSetter set, const QString &name)
        : getter(get), setter(set), diffName(name)
    {}

public:
    valueGetter getter;
    valueSetter setter;
    const QString diffName;
};

AccountsHandler::AccountsHandler(SugarSession *session)
    : ModuleHandler(QLatin1String("Accounts"), session),
      mAccessors(new AccountAccessorHash)
{
    mAccessors->insert(QLatin1String("id"),
                       new AccountAccessorPair(&SugarAccount::id, &SugarAccount::setId, QString()));
    mAccessors->insert(QLatin1String("name"),
                       new AccountAccessorPair(&SugarAccount::name, &SugarAccount::setName,
                                        i18nc("@item:intable account name", "Name")));
    mAccessors->insert(QLatin1String("date_entered"),
                       new AccountAccessorPair(&SugarAccount::dateEntered, &SugarAccount::setDateEntered, QString()));
    mAccessors->insert(QLatin1String("date_modified"),
                       new AccountAccessorPair(&SugarAccount::dateModified, &SugarAccount::setDateModified, QString()));
    mAccessors->insert(QLatin1String("modified_user_id"),
                       new AccountAccessorPair(&SugarAccount::modifiedUserId, &SugarAccount::setModifiedUserId, QString()));
    mAccessors->insert(QLatin1String("modified_by_name"),
                       new AccountAccessorPair(&SugarAccount::modifiedByName, &SugarAccount::setModifiedByName, QString()));
    mAccessors->insert(QLatin1String("created_by"),
                       new AccountAccessorPair(&SugarAccount::createdBy, &SugarAccount::setCreatedBy, QString()));
    mAccessors->insert(QLatin1String("created_by_name"),
                       new AccountAccessorPair(&SugarAccount::createdByName, &SugarAccount::setCreatedByName, QString()));
    mAccessors->insert(QLatin1String("description"),
                       new AccountAccessorPair(&SugarAccount::description, &SugarAccount::setDescription,
                                        i18nc("@item:intable", "Description")));
    mAccessors->insert(QLatin1String("deleted"),
                       new AccountAccessorPair(&SugarAccount::deleted, &SugarAccount::setDeleted, QString()));
    mAccessors->insert(QLatin1String("assigned_user_id"),
                       new AccountAccessorPair(&SugarAccount::assignedUserId, &SugarAccount::setAssignedUserId, QString()));
    mAccessors->insert(QLatin1String("assigned_user_name"),
                       new AccountAccessorPair(&SugarAccount::assignedUserName, &SugarAccount::setAssignedUserName,
                                        i18nc("@item:intable", "Assigned To")));
    mAccessors->insert(QLatin1String("account_type"),
                       new AccountAccessorPair(&SugarAccount::accountType, &SugarAccount::setAccountType,
                                        i18nc("@item:intable", "Type")));
    mAccessors->insert(QLatin1String("industry"),
                       new AccountAccessorPair(&SugarAccount::industry, &SugarAccount::setIndustry,
                                        i18nc("@item:intable", "Industry")));
    mAccessors->insert(QLatin1String("annual_revenue"),
                       new AccountAccessorPair(&SugarAccount::annualRevenue, &SugarAccount::setAnnualRevenue,
                                        i18nc("@item:intable", "Annual Revenue")));
    mAccessors->insert(QLatin1String("phone_fax"),
                       new AccountAccessorPair(&SugarAccount::phoneFax, &SugarAccount::setPhoneFax,
                                        i18nc("@item:intable", "Fax")));
    mAccessors->insert(QLatin1String("billing_address_street"),
                       new AccountAccessorPair(&SugarAccount::billingAddressStreet, &SugarAccount::setBillingAddressStreet, QString()));
    mAccessors->insert(QLatin1String("billing_address_city"),
                       new AccountAccessorPair(&SugarAccount::billingAddressCity, &SugarAccount::setBillingAddressCity, QString()));
    mAccessors->insert(QLatin1String("billing_address_state"),
                       new AccountAccessorPair(&SugarAccount::billingAddressState, &SugarAccount::setBillingAddressState, QString()));
    mAccessors->insert(QLatin1String("billing_address_postalcode"),
                       new AccountAccessorPair(&SugarAccount::billingAddressPostalcode, &SugarAccount::setBillingAddressPostalcode, QString()));
    mAccessors->insert(QLatin1String("billing_address_country"),
                       new AccountAccessorPair(&SugarAccount::billingAddressCountry, &SugarAccount::setBillingAddressCountry, QString()));
    mAccessors->insert(QLatin1String("rating"),
                       new AccountAccessorPair(&SugarAccount::rating, &SugarAccount::setRating,
                                        i18nc("@item:intable", "Rating")));
    mAccessors->insert(QLatin1String("phone_office"),
                       new AccountAccessorPair(&SugarAccount::phoneOffice, &SugarAccount::setPhoneOffice,
                                        i18nc("@item:intable", "Phone (Office)")));
    mAccessors->insert(QLatin1String("phone_alternate"),
                       new AccountAccessorPair(&SugarAccount::phoneAlternate, &SugarAccount::setPhoneAlternate,
                                        i18nc("@item:intable", "Phone (Other)")));
    mAccessors->insert(QLatin1String("website"),
                       new AccountAccessorPair(&SugarAccount::website, &SugarAccount::setWebsite,
                                        i18nc("@item:intable", "Website")));
    mAccessors->insert(QLatin1String("ownership"),
                       new AccountAccessorPair(&SugarAccount::ownership, &SugarAccount::setOwnership,
                                        i18nc("@item:intable", "Ownership")));
    mAccessors->insert(QLatin1String("employees"),
                       new AccountAccessorPair(&SugarAccount::employees, &SugarAccount::setEmployees,
                                        i18nc("@item:intable", "Employees")));
    mAccessors->insert(QLatin1String("ticker_symbol"),
                       new AccountAccessorPair(&SugarAccount::tickerSymbol, &SugarAccount::setTickerSymbol,
                                        i18nc("@item:intable", "Ticker Symbol")));
    mAccessors->insert(QLatin1String("shipping_address_street"),
                       new AccountAccessorPair(&SugarAccount::shippingAddressStreet, &SugarAccount::setShippingAddressStreet, QString()));
    mAccessors->insert(QLatin1String("shipping_address_city"),
                       new AccountAccessorPair(&SugarAccount::shippingAddressCity, &SugarAccount::setShippingAddressCity, QString()));
    mAccessors->insert(QLatin1String("shipping_address_state"),
                       new AccountAccessorPair(&SugarAccount::shippingAddressState, &SugarAccount::setShippingAddressState, QString()));
    mAccessors->insert(QLatin1String("shipping_address_postalcode"),
                       new AccountAccessorPair(&SugarAccount::shippingAddressPostalcode, &SugarAccount::setShippingAddressPostalcode, QString()));
    mAccessors->insert(QLatin1String("shipping_address_country"),
                       new AccountAccessorPair(&SugarAccount::shippingAddressCountry, &SugarAccount::setShippingAddressCountry, QString()));
    mAccessors->insert(QLatin1String("email1"),
                       new AccountAccessorPair(&SugarAccount::email1, &SugarAccount::setEmail1,
                                        i18nc("@item:intable", "Primary Email")));
    mAccessors->insert(QLatin1String("parent_id"),
                       new AccountAccessorPair(&SugarAccount::parentId, &SugarAccount::setParentId, QString()));
    mAccessors->insert(QLatin1String("parent_name"),
                       new AccountAccessorPair(&SugarAccount::parentName, &SugarAccount::setParentName,
                                        i18nc("@item:intable", "Member Of")));
    mAccessors->insert(QLatin1String("sic_code"),
                       new AccountAccessorPair(&SugarAccount::sicCode, &SugarAccount::setSicCode,
                                        i18nc("@item:intable", "SIC Code")));
    mAccessors->insert(QLatin1String("campaign_id"),
                       new AccountAccessorPair(&SugarAccount::campaignId, &SugarAccount::setCampaignId, QString()));
    mAccessors->insert(QLatin1String("campaign_name"),
                       new AccountAccessorPair(&SugarAccount::campaignName, &SugarAccount::setCampaignName,
                                        i18nc("@item:intable", "Campaign")));
}

AccountsHandler::~AccountsHandler()
{
    qDeleteAll(*mAccessors);
    delete mAccessors;
}

QStringList AccountsHandler::supportedFields() const
{
    return mAccessors->keys();
}

Akonadi::Collection AccountsHandler::handlerCollection() const
{
    Akonadi::Collection accountCollection;
    accountCollection.setContentMimeTypes(QStringList() << SugarAccount::mimeType());
    accountCollection.setName(i18nc("@item folder name", "Accounts"));
    accountCollection.setRights(Akonadi::Collection::CanChangeItem |
                                Akonadi::Collection::CanCreateItem |
                                Akonadi::Collection::CanDeleteItem);

    return accountCollection;
}

QString AccountsHandler::orderByForListing() const
{
    return QLatin1String("accounts.name");
}

QStringList AccountsHandler::selectedFieldsForListing() const
{
    return mAccessors->keys();
}

bool AccountsHandler::setEntry(const Akonadi::Item &item)
{
    if (!item.hasPayload<SugarAccount>()) {
        kError() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Account payload";
        return false;
    }

    QList<KDSoapGenerated::TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the account being added
    if (!item.remoteId().isEmpty()) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(QLatin1String("id"));
        field.setValue(item.remoteId());

        itemList << field;
    }

    const SugarAccount account = item.payload<SugarAccount>();
    AccountAccessorHash::const_iterator it    = mAccessors->constBegin();
    AccountAccessorHash::const_iterator endIt = mAccessors->constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }
        const valueGetter getter = (*it)->getter;
        KDSoapGenerated::TNS__Name_value field;
        field.setName(it.key());
        field.setValue(KDCRMUtils::encodeXML((account.*getter)()));

        itemList << field;
    }

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);
    soap()->asyncSet_entry(sessionId(), moduleName(), valueList);

    return true;
}

Akonadi::Item AccountsHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection)
{
    Akonadi::Item item;

    const QList<KDSoapGenerated::TNS__Name_value> valueList = entry.name_value_list().items();
    if (valueList.isEmpty()) {
        kWarning() << "Accounts entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId(entry.id());
    item.setParentCollection(parentCollection);
    item.setMimeType(SugarAccount::mimeType());

    SugarAccount account;
    account.setId(entry.id());
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList) {
        const AccountAccessorHash::const_iterator accessIt = mAccessors->constFind(namedValue.name());
        if (accessIt == mAccessors->constEnd()) {
            // no accessor for field
            continue;
        }

        (account.*(accessIt.value()->setter))(KDCRMUtils::decodeXML(namedValue.value()));
    }
    item.setPayload<SugarAccount>(account);
    item.setRemoteRevision(account.dateModified());

    return item;
}

void AccountsHandler::compare(Akonadi::AbstractDifferencesReporter *reporter,
                              const Akonadi::Item &leftItem, const Akonadi::Item &rightItem)
{
    Q_ASSERT(leftItem.hasPayload<SugarAccount>());
    Q_ASSERT(rightItem.hasPayload<SugarAccount>());

    const SugarAccount leftAccount = leftItem.payload<SugarAccount>();
    const SugarAccount rightAccount = rightItem.payload<SugarAccount>();

    const QString modifiedBy = mSession->userName();
    const QString modifiedOn = formatDate(rightAccount.dateModified());

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "Local Account"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Serverside Account: modified by %1 on %2",
              modifiedBy, modifiedOn));

    bool seenBillingAddress = false;
    bool seenShippingAddress = false;

    AccountAccessorHash::const_iterator it    = mAccessors->constBegin();
    AccountAccessorHash::const_iterator endIt = mAccessors->constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }

        const valueGetter getter = (*it)->getter;
        QString leftValue = (leftAccount.*getter)();
        QString rightValue = (rightAccount.*getter)();

        QString diffName = (*it)->diffName;
        if (diffName.isEmpty()) {
            // check for special fields
            if (it.key().startsWith(QLatin1String("billing"))) {
                if (!seenBillingAddress) {
                    seenBillingAddress = true;
                    diffName = i18nc("@item:intable", "Billing Address");

                    KABC::Address leftAddress;
                    leftAddress.setStreet(leftAccount.billingAddressStreet());
                    leftAddress.setLocality(leftAccount.billingAddressCity());
                    leftAddress.setRegion(leftAccount.billingAddressState());
                    leftAddress.setCountry(leftAccount.billingAddressCountry());
                    leftAddress.setPostalCode(leftAccount.billingAddressPostalcode());

                    KABC::Address rightAddress;
                    rightAddress.setStreet(rightAccount.billingAddressStreet());
                    rightAddress.setLocality(rightAccount.billingAddressCity());
                    rightAddress.setRegion(rightAccount.billingAddressState());
                    rightAddress.setCountry(rightAccount.billingAddressCountry());
                    rightAddress.setPostalCode(rightAccount.billingAddressPostalcode());

                    leftValue = leftAddress.formattedAddress();
                    rightValue = rightAddress.formattedAddress();
                } else {
                    // already printed, skip
                    continue;
                }
            } else if (it.key().startsWith(QLatin1String("shipping"))) {
                if (!seenShippingAddress) {
                    seenShippingAddress = true;
                    diffName = i18nc("@item:intable", "Shipping Address");

                    KABC::Address leftAddress(KABC::Address::Parcel);
                    leftAddress.setStreet(leftAccount.shippingAddressStreet());
                    leftAddress.setLocality(leftAccount.shippingAddressCity());
                    leftAddress.setRegion(leftAccount.shippingAddressState());
                    leftAddress.setCountry(leftAccount.shippingAddressCountry());
                    leftAddress.setPostalCode(leftAccount.shippingAddressPostalcode());

                    KABC::Address rightAddress(KABC::Address::Parcel);
                    rightAddress.setStreet(rightAccount.shippingAddressStreet());
                    rightAddress.setLocality(rightAccount.shippingAddressCity());
                    rightAddress.setRegion(rightAccount.shippingAddressState());
                    rightAddress.setCountry(rightAccount.shippingAddressCountry());
                    rightAddress.setPostalCode(rightAccount.shippingAddressPostalcode());

                    leftValue = leftAddress.formattedAddress();
                    rightValue = rightAddress.formattedAddress();
                } else {
                    // already printed, skip
                    continue;
                }
            } else {
                // internal field, skip
                continue;
            }
        }

        if (leftValue.isEmpty() && rightValue.isEmpty()) {
            continue;
        }

        if (leftValue.isEmpty()) {
            reporter->addProperty(Akonadi::AbstractDifferencesReporter::AdditionalRightMode,
                                  diffName, leftValue, rightValue);
        } else if (rightValue.isEmpty()) {
            reporter->addProperty(Akonadi::AbstractDifferencesReporter::AdditionalLeftMode,
                                  diffName, leftValue, rightValue);
        } else if (leftValue != rightValue) {
            reporter->addProperty(Akonadi::AbstractDifferencesReporter::ConflictMode,
                                  diffName, leftValue, rightValue);
        }
    }
}
