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

#include "leadshandler.h"
#include "sugarsession.h"
#include "sugarcrmresource_debug.h"
#include "sugarsoap.h"
using namespace KDSoapGenerated;

#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/sugarlead.h"

#include <AkonadiCore/abstractdifferencesreporter.h> //krazy:exclude=camelcase
#include <AkonadiCore/Collection>

#include <KContacts/Address>

#include <KLocalizedString>

LeadsHandler::LeadsHandler(SugarSession *session)
    : ModuleHandler(QLatin1String("Leads"), session),
      mAccessors(SugarLead::accessorHash())
{
}

Akonadi::Collection LeadsHandler::handlerCollection() const
{
    Akonadi::Collection leadCollection;
    leadCollection.setContentMimeTypes(QStringList() << SugarLead::mimeType());
    leadCollection.setName(i18nc("@item folder name", "Leads"));
    leadCollection.setRights(Akonadi::Collection::CanChangeItem |
                             Akonadi::Collection::CanCreateItem |
                             Akonadi::Collection::CanDeleteItem);

    return leadCollection;
}

QString LeadsHandler::orderByForListing() const
{
    return QStringLiteral("leads.last_name");
}

QStringList LeadsHandler::supportedSugarFields() const
{
    return sugarFieldsFromCrmFields(mAccessors.keys());
}

bool LeadsHandler::setEntry(const Akonadi::Item &item)
{
    if (!item.hasPayload<SugarLead>()) {
        qCritical() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Lead payload";
        return false;
    }

    QList<KDSoapGenerated::TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the lead being added
    if (!item.remoteId().isEmpty()) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(QStringLiteral("id"));
        field.setValue(item.remoteId());
        itemList << field;
    }

    const SugarLead lead = item.payload<SugarLead>();
    SugarLead::AccessorHash::const_iterator it    = mAccessors.constBegin();
    SugarLead::AccessorHash::const_iterator endIt = mAccessors.constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }
        const SugarLead::valueGetter getter = (*it).getter;
        KDSoapGenerated::TNS__Name_value field;
        field.setName(sugarFieldFromCrmField(it.key()));
        field.setValue(KDCRMUtils::encodeXML((lead.*getter)()));

        itemList << field;
    }

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);
    soap()->asyncSet_entry(sessionId(), moduleName(), valueList);

    return true;
}

Akonadi::Item LeadsHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection)
{
    Akonadi::Item item;

    const QList<KDSoapGenerated::TNS__Name_value> valueList = entry.name_value_list().items();
    if (valueList.isEmpty()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "Leads entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId(entry.id());
    item.setParentCollection(parentCollection);
    item.setMimeType(SugarLead::mimeType());

    SugarLead lead;
    lead.setId(entry.id());
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList) {
        const QString crmFieldName = sugarFieldToCrmField(namedValue.name());
        const SugarLead::AccessorHash::const_iterator accessIt = mAccessors.constFind(crmFieldName);
        if (accessIt == mAccessors.constEnd()) {
            // no accessor for field
            continue;
        }

        (lead.*(accessIt.value().setter))(KDCRMUtils::decodeXML(namedValue.value()));
    }
    item.setPayload<SugarLead>(lead);
    item.setRemoteRevision(lead.dateModified());

    return item;
}

void LeadsHandler::compare(Akonadi::AbstractDifferencesReporter *reporter,
                           const Akonadi::Item &leftItem, const Akonadi::Item &rightItem)
{
    Q_ASSERT(leftItem.hasPayload<SugarLead>());
    Q_ASSERT(rightItem.hasPayload<SugarLead>());

    const SugarLead leftLead = leftItem.payload<SugarLead>();
    const SugarLead rightLead = rightItem.payload<SugarLead>();

    const QString modifiedBy = mSession->userName();
    const QString modifiedOn = formatDate(rightLead.dateModified());

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "Local Lead"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Serverside Lead: modified by %1 on %2",
              modifiedBy, modifiedOn));

    bool seenPrimaryAddress = false;
    bool seenOtherAddress = false;
    SugarLead::AccessorHash::const_iterator it    = mAccessors.constBegin();
    SugarLead::AccessorHash::const_iterator endIt = mAccessors.constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }

        const SugarLead::valueGetter getter = (*it).getter;
        QString leftValue = (leftLead.*getter)();
        QString rightValue = (rightLead.*getter)();

        QString diffName = (*it).diffName;
        if (diffName.isEmpty()) {
            // check for special fields
            if (isAddressValue(it.key())) {
                if (isPrimaryAddressValue(it.key())) {
                    if (!seenPrimaryAddress) {
                        seenPrimaryAddress = true;
                        diffName = i18nc("@item:intable", "Primary Address");

                        KContacts::Address leftAddress(KContacts::Address::Work | KContacts::Address::Pref);
                        leftAddress.setStreet(leftLead.primaryAddressStreet());
                        leftAddress.setLocality(leftLead.primaryAddressCity());
                        leftAddress.setRegion(leftLead.primaryAddressState());
                        leftAddress.setCountry(leftLead.primaryAddressCountry());
                        leftAddress.setPostalCode(leftLead.primaryAddressPostalcode());

                        KContacts::Address rightAddress(KContacts::Address::Work | KContacts::Address::Pref);
                        rightAddress.setStreet(rightLead.primaryAddressStreet());
                        rightAddress.setLocality(rightLead.primaryAddressCity());
                        rightAddress.setRegion(rightLead.primaryAddressState());
                        rightAddress.setCountry(rightLead.primaryAddressCountry());
                        rightAddress.setPostalCode(rightLead.primaryAddressPostalcode());

                        leftValue = leftAddress.formattedAddress();
                        rightValue = rightAddress.formattedAddress();
                    } else {
                        // already printed, skip
                        continue;
                    }
                } else {
                    if (!seenOtherAddress) {
                        seenOtherAddress = true;
                        diffName = i18nc("@item:intable", "Other Address");

                        KContacts::Address leftAddress(KContacts::Address::Home);
                        leftAddress.setStreet(leftLead.altAddressStreet());
                        leftAddress.setLocality(leftLead.altAddressCity());
                        leftAddress.setRegion(leftLead.altAddressState());
                        leftAddress.setCountry(leftLead.altAddressCountry());
                        leftAddress.setPostalCode(leftLead.altAddressPostalcode());

                        KContacts::Address rightAddress(KContacts::Address::Home);
                        rightAddress.setStreet(rightLead.altAddressStreet());
                        rightAddress.setLocality(rightLead.altAddressCity());
                        rightAddress.setRegion(rightLead.altAddressState());
                        rightAddress.setCountry(rightLead.altAddressCountry());
                        rightAddress.setPostalCode(rightLead.altAddressPostalcode());

                        leftValue = leftAddress.formattedAddress();
                        rightValue = rightAddress.formattedAddress();
                    } else {
                        // already printed, skip
                        continue;
                    }
                }
            } else if (it.key() == "do_not_call") {
                diffName = i18nc("@item:intable", "Do Not Call");
                leftValue = leftLead.doNotCall() == QLatin1String("1")
                            ? QLatin1String("Yes") : QLatin1String("No");
                rightValue = rightLead.doNotCall() == QLatin1String("1")
                             ? QLatin1String("Yes") : QLatin1String("No");
            } else if (it.key() == "converted") {
                diffName = i18nc("@item:intable", "Converted");
                leftValue = leftLead.converted() == QLatin1String("1")
                            ? QLatin1String("Yes") : QLatin1String("No");
                rightValue = rightLead.converted() == QLatin1String("1")
                             ? QLatin1String("Yes") : QLatin1String("No");
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
