/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "campaignshandler.h"
#include "sugarcrmresource_debug.h"
#include "kdcrmdata/kdcrmutils.h"
#include "sugarsession.h"
#include "wsdl_sugar41.h"
#include "sugarjob.h"

using namespace KDSoapGenerated;
#include <Akonadi/AbstractDifferencesReporter>
#include <Akonadi/Collection>

#include <KLocalizedString>

CampaignsHandler::CampaignsHandler(SugarSession *session)
    : ModuleHandler(Module::Campaigns, session)
{
}

Akonadi::Collection CampaignsHandler::handlerCollection() const
{
    Akonadi::Collection campaignCollection;
    campaignCollection.setRemoteId(moduleToName(module()));
    campaignCollection.setContentMimeTypes(QStringList() << SugarCampaign::mimeType());
    campaignCollection.setName(i18nc("@item folder name", "Campaigns"));
    campaignCollection.setRights(Akonadi::Collection::CanChangeItem |
                                 Akonadi::Collection::CanCreateItem |
                                 Akonadi::Collection::CanDeleteItem);

    return campaignCollection;
}

QString CampaignsHandler::orderByForListing() const
{
    return QStringLiteral("campaigns.name");
}

QStringList CampaignsHandler::supportedSugarFields() const
{
    return sugarFieldsFromCrmFields(SugarCampaign::accessorHash().keys());
}

QStringList CampaignsHandler::supportedCRMFields() const
{
    return sugarFieldsToCrmFields(availableFields());
}

KDSoapGenerated::TNS__Name_value_list CampaignsHandler::sugarCampaignToNameValueList(const SugarCampaign &campaign, QList<KDSoapGenerated::TNS__Name_value> itemList)
{
    const SugarCampaign::AccessorHash accessors = SugarCampaign::accessorHash();
    for (auto it = accessors.constBegin(); it != accessors.constEnd(); ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }
        const SugarCampaign::valueGetter getter = (*it).getter;
        KDSoapGenerated::TNS__Name_value field;
        field.setName(sugarFieldFromCrmField(it.key()));
        field.setValue(KDCRMUtils::encodeXML((campaign.*getter)()));

        itemList << field;
    }

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);
    return valueList;
}

int CampaignsHandler::setEntry(const Akonadi::Item &item, QString &newId, QString &errorMessage)
{
    if (!item.hasPayload<SugarCampaign>()) {
        qCCritical(FATCRM_SUGARCRMRESOURCE_LOG) << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Campaign payload";
        return SugarJob::InvalidContextError;
    }

    QList<KDSoapGenerated::TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the campaign being added
    if (!item.remoteId().isEmpty()) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(QStringLiteral("id"));
        field.setValue(item.remoteId());

        itemList << field;
    }

    const SugarCampaign campaign = item.payload<SugarCampaign>();

    KDSoapGenerated::TNS__Name_value_list valueList = sugarCampaignToNameValueList(campaign, itemList);

    return mSession->protocol()->setEntry(module(), valueList, newId, errorMessage);
}

Akonadi::Item CampaignsHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection, bool &deleted)
{
    Akonadi::Item item;

    const QList<KDSoapGenerated::TNS__Name_value> valueList = entry.name_value_list().items();
    if (valueList.isEmpty()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "Campaigns entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId(entry.id());
    item.setParentCollection(parentCollection);
    item.setMimeType(SugarCampaign::mimeType());

    SugarCampaign campaign;
    campaign.setId(entry.id());
    const SugarCampaign::AccessorHash accessors = SugarCampaign::accessorHash();
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList) {
        const QString crmFieldName = sugarFieldToCrmField(namedValue.name());
        const SugarCampaign::AccessorHash::const_iterator accessIt = accessors.constFind(crmFieldName);
        if (accessIt == accessors.constEnd()) {
            // no accessor for field
            continue;
        }

        (campaign.*(accessIt.value().setter))(KDCRMUtils::decodeXML(namedValue.value()));
    }
    item.setPayload<SugarCampaign>(campaign);
    item.setRemoteRevision(campaign.dateModified());

    deleted = campaign.deleted() == QLatin1String("1");

    return item;
}

void CampaignsHandler::compare(Akonadi::AbstractDifferencesReporter *reporter,
                               const Akonadi::Item &leftItem, const Akonadi::Item &rightItem)
{
    Q_ASSERT(leftItem.hasPayload<SugarCampaign>());
    Q_ASSERT(rightItem.hasPayload<SugarCampaign>());

    const SugarCampaign leftCampaign = leftItem.payload<SugarCampaign>();
    const SugarCampaign rightCampaign = rightItem.payload<SugarCampaign>();

    const QString modifiedBy = mSession->userName();
    const QString modifiedOn = formatDate(rightCampaign.dateModified());

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "My Campaign"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Their Campaign: modified by %1 on %2",
              modifiedBy, modifiedOn));

    const SugarCampaign::AccessorHash accessors = SugarCampaign::accessorHash();
    for (auto it = accessors.constBegin(); it != accessors.constEnd(); ++it) {
        // check if this is a read-only field
        if (it.key() == QLatin1String("id")) {
            continue;
        }

        const QString diffName = (*it).diffName;
        if (diffName.isEmpty()) {
            // TODO some fields like currency_id should be handled as special fields instead
            // i.e. currency string, dates formatted with KLocale
            continue;
        }

        const SugarCampaign::valueGetter getter = (*it).getter;
        const QString leftValue = (leftCampaign.*getter)();
        const QString rightValue = (rightCampaign.*getter)();

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
