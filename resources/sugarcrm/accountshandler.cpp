/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>
           Jeremy Entressangle <jeremy.entressangle@kdab.com>

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
#include "sugarcrmresource_debug.h"
#include "kdcrmutils.h"
#include "sugarsession.h"
#include "sugarsoap.h"
#include "sugarjob.h"
using namespace KDSoapGenerated;

#include "kdcrmdata/sugaraccount.h"

#include <AkonadiCore/abstractdifferencesreporter.h> //krazy:exclude=camelcase
#include <AkonadiCore/Collection>
#include <AkonadiCore/ItemFetchJob>
#include <AkonadiCore/ItemFetchScope>

#include <KContacts/Address>

#include <KLocalizedString>

#include <QHash>

AccountsHandler::AccountsHandler(SugarSession *session)
    : ModuleHandler(Module::Accounts, session)
{

}

AccountsHandler::~AccountsHandler()
{
}

void AccountsHandler::fillAccountsCache()
{
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
    return QStringLiteral("accounts.name");
}

QStringList AccountsHandler::supportedSugarFields() const
{
    // get everything, given that SugarAccount has support for custom fields
    return availableFields();
}

QStringList AccountsHandler::supportedCRMFields() const
{
    return sugarFieldsToCrmFields(availableFields());
}

KDSoapGenerated::TNS__Name_value_list AccountsHandler::sugarAccountToNameValueList(const SugarAccount &account, QList<KDSoapGenerated::TNS__Name_value> itemList)
{
    const SugarAccount::AccessorHash accessors = SugarAccount::accessorHash();
    for (auto it = accessors.constBegin(); it != accessors.constEnd(); ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }
        const SugarAccount::valueGetter getter = (*it).getter;
        KDSoapGenerated::TNS__Name_value field;
        field.setName(ModuleHandler::sugarFieldFromCrmField(it.key()));
        field.setValue(KDCRMUtils::encodeXML((account.*getter)()));

        itemList << field;
    }
    // plus custom fields
    const QMap<QString, QString> customFields = account.customFields();
    for (auto cit = customFields.constBegin(); cit != customFields.constEnd(); ++cit ) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(ModuleHandler::customSugarFieldFromCrmField(cit.key()));
        field.setValue(KDCRMUtils::encodeXML(cit.value()));
        itemList << field;
    }
    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);

    return valueList;
}

int AccountsHandler::setEntry(const Akonadi::Item &item, QString &newId, QString &errorMessage)
{
    if (!item.hasPayload<SugarAccount>()) {
        qCCritical(FATCRM_SUGARCRMRESOURCE_LOG) << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Account payload";
        return SugarJob::InvalidContextError;
    }

    QList<KDSoapGenerated::TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the account being added
    if (!item.remoteId().isEmpty()) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(QStringLiteral("id"));
        field.setValue(item.remoteId());

        itemList << field;
    }

    const SugarAccount account = item.payload<SugarAccount>();

    KDSoapGenerated::TNS__Name_value_list valueList = sugarAccountToNameValueList(account, itemList);

    return mSession->protocol()->setEntry(module(), valueList, newId, errorMessage);
}

int AccountsHandler::expectedContentsVersion() const
{
    // version 1 = accounts now store all fields, including custom fields
    // version 2 = ensure we get trimmed names (no trailing spaces)
    // version 3 = renaming of custom fields
    return 3;
}

SugarAccount AccountsHandler::nameValueListToSugarAccount(const KDSoapGenerated::TNS__Name_value_list & valueList, const QString &id)
{
    SugarAccount account;
    account.setId(id);
    const SugarAccount::AccessorHash accessors = SugarAccount::accessorHash();
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList.items()) {
        const QString crmFieldName = ModuleHandler::sugarFieldToCrmField(namedValue.name());
        const QString value = KDCRMUtils::decodeXML(namedValue.value());
        const SugarAccount::AccessorHash::const_iterator accessIt = accessors.constFind(crmFieldName);
        if (accessIt == accessors.constEnd()) {
            const QString crmCustomFieldName = customSugarFieldToCrmField(namedValue.name());
            account.setCustomField(crmCustomFieldName, value);
            continue;
        }

        (account.*(accessIt.value().setter))(value);
    }
    return account;
}

Akonadi::Item AccountsHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection, bool &deleted)
{
    Akonadi::Item item;

    const QList<KDSoapGenerated::TNS__Name_value> valueList = entry.name_value_list().items();
    if (valueList.isEmpty()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "Accounts entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId(entry.id());
    item.setParentCollection(parentCollection);
    item.setMimeType(SugarAccount::mimeType());

    const SugarAccount account = nameValueListToSugarAccount(entry.name_value_list(), entry.id());
    item.setPayload<SugarAccount>(account);
    item.setRemoteRevision(account.dateModifiedRaw());

    deleted = account.deleted() == QLatin1String("1");

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
    const QString modifiedOn = formatDate(rightAccount.dateModifiedRaw());

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "My Account"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Their Account: modified by %1 on %2",
              modifiedBy, modifiedOn));

    bool seenBillingAddress = false;
    bool seenShippingAddress = false;

    const SugarAccount::AccessorHash accessors = SugarAccount::accessorHash();
    for (auto it = accessors.constBegin(); it != accessors.constEnd(); ++it) {
        // check if this is a read-only field
        if (it.key() == QLatin1String("id")) {
            continue;
        }

        const SugarAccount::valueGetter getter = (*it).getter;
        QString leftValue = (leftAccount.*getter)();
        QString rightValue = (rightAccount.*getter)();

        QString diffName = (*it).diffName;
        if (diffName.isEmpty()) {
            // check for special fields
            if (it.key().startsWith(QStringLiteral("billing"))) {
                if (!seenBillingAddress) {
                    seenBillingAddress = true;
                    diffName = i18nc("@item:intable", "Billing Address");

                    KContacts::Address leftAddress;
                    leftAddress.setStreet(leftAccount.billingAddressStreet());
                    leftAddress.setLocality(leftAccount.billingAddressCity());
                    leftAddress.setRegion(leftAccount.billingAddressState());
                    leftAddress.setCountry(leftAccount.billingAddressCountry());
                    leftAddress.setPostalCode(leftAccount.billingAddressPostalcode());

                    KContacts::Address rightAddress;
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
            } else if (it.key().startsWith(QStringLiteral("shipping"))) {
                if (!seenShippingAddress) {
                    seenShippingAddress = true;
                    diffName = i18nc("@item:intable", "Office Address");

                    KContacts::Address leftAddress(KContacts::Address::Parcel);
                    leftAddress.setStreet(leftAccount.shippingAddressStreet());
                    leftAddress.setLocality(leftAccount.shippingAddressCity());
                    leftAddress.setRegion(leftAccount.shippingAddressState());
                    leftAddress.setCountry(leftAccount.shippingAddressCountry());
                    leftAddress.setPostalCode(leftAccount.shippingAddressPostalcode());

                    KContacts::Address rightAddress(KContacts::Address::Parcel);
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
