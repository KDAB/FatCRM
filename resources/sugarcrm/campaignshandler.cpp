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

#include "campaignshandler.h"

#include "kdcrmdata/kdcrmutils.h"
#include "sugarsession.h"
#include "sugarsoap.h"

using namespace KDSoapGenerated;
#include <akonadi/abstractdifferencesreporter.h>
#include <akonadi/collection.h>

#include <kdcrmdata/sugarcampaign.h>

#include <KLocale>

#include <QHash>

typedef QString(SugarCampaign::*valueGetter)() const;
typedef void (SugarCampaign::*valueSetter)(const QString &);

class CampaignAccessorPair
{
public:
    CampaignAccessorPair(valueGetter get, valueSetter set, const QString &name)
        : getter(get), setter(set), diffName(name)
    {}

public:
    valueGetter getter;
    valueSetter setter;
    const QString diffName;
};

CampaignsHandler::CampaignsHandler(SugarSession *session)
    : ModuleHandler(QLatin1String("Campaigns"), session),
      mAccessors(new CampaignAccessorHash)
{
    mAccessors->insert(QLatin1String("id"),
                       new CampaignAccessorPair(&SugarCampaign::id, &SugarCampaign::setId, QString()));
    mAccessors->insert(QLatin1String("name"),
                       new CampaignAccessorPair(&SugarCampaign::name, &SugarCampaign::setName,
                                        i18nc("@item:intable campaign name", "Name")));
    mAccessors->insert(QLatin1String("date_entered"),
                       new CampaignAccessorPair(&SugarCampaign::dateEntered, &SugarCampaign::setDateEntered, QString()));
    mAccessors->insert(QLatin1String("date_modified"),
                       new CampaignAccessorPair(&SugarCampaign::dateModified, &SugarCampaign::setDateModified, QString()));
    mAccessors->insert(QLatin1String("modified_user_id"),
                       new CampaignAccessorPair(&SugarCampaign::modifiedUserId, &SugarCampaign::setModifiedUserId, QString()));
    mAccessors->insert(QLatin1String("modified_by_name"),
                       new CampaignAccessorPair(&SugarCampaign::modifiedByName, &SugarCampaign::setModifiedByName, QString()));
    mAccessors->insert(QLatin1String("created_by"),
                       new CampaignAccessorPair(&SugarCampaign::createdBy, &SugarCampaign::setCreatedBy, QString()));
    mAccessors->insert(QLatin1String("created_by_name"),
                       new CampaignAccessorPair(&SugarCampaign::createdByName, &SugarCampaign::setCreatedByName, QString()));
    mAccessors->insert(QLatin1String("deleted"),
                       new CampaignAccessorPair(&SugarCampaign::deleted, &SugarCampaign::setDeleted, QString()));
    mAccessors->insert(QLatin1String("assigned_user_id"),
                       new CampaignAccessorPair(&SugarCampaign::assignedUserId, &SugarCampaign::setAssignedUserId, QString()));
    mAccessors->insert(QLatin1String("assigned_user_name"),
                       new CampaignAccessorPair(&SugarCampaign::assignedUserName, &SugarCampaign::setAssignedUserName,
                                        i18nc("@item:intable", "Assigned To")));
    mAccessors->insert(QLatin1String("tracker_key"),
                       new CampaignAccessorPair(&SugarCampaign::trackerKey, &SugarCampaign::setTrackerKey, QString()));
    mAccessors->insert(QLatin1String("tracker_count"),
                       new CampaignAccessorPair(&SugarCampaign::trackerCount, &SugarCampaign::setTrackerCount, QString()));
    mAccessors->insert(QLatin1String("refer_url"),
                       new CampaignAccessorPair(&SugarCampaign::referUrl, &SugarCampaign::setReferUrl,
                                        i18nc("@item:intable", "Referer URL")));
    mAccessors->insert(QLatin1String("tracker_text"),
                       new CampaignAccessorPair(&SugarCampaign::trackerText, &SugarCampaign::setTrackerText,
                                        i18nc("@item:intable", "Tracker")));
    mAccessors->insert(QLatin1String("start_date"),
                       new CampaignAccessorPair(&SugarCampaign::startDate, &SugarCampaign::setStartDate,
                                        i18nc("@item:intable", "Start Date")));
    mAccessors->insert(QLatin1String("end_date"),
                       new CampaignAccessorPair(&SugarCampaign::endDate, &SugarCampaign::setEndDate,
                                        i18nc("@item:intable", "End Date")));
    mAccessors->insert(QLatin1String("status"),
                       new CampaignAccessorPair(&SugarCampaign::status, &SugarCampaign::setStatus,
                                        i18nc("@item:intable", "Status")));
    mAccessors->insert(QLatin1String("impressions"),
                       new CampaignAccessorPair(&SugarCampaign::impressions, &SugarCampaign::setImpressions,
                                        i18nc("@item:intable", "Impressions")));
    mAccessors->insert(QLatin1String("currency_id"),
                       new CampaignAccessorPair(&SugarCampaign::currencyId, &SugarCampaign::setCurrencyId,
                                        i18nc("@item:intable", "Currency")));
    mAccessors->insert(QLatin1String("budget"),
                       new CampaignAccessorPair(&SugarCampaign::budget, &SugarCampaign::setBudget,
                                        i18nc("@item:intable", "Budget")));
    mAccessors->insert(QLatin1String("expected_cost"),
                       new CampaignAccessorPair(&SugarCampaign::expectedCost, &SugarCampaign::setExpectedCost,
                                        i18nc("@item:intable", "Expected Costs")));
    mAccessors->insert(QLatin1String("actual_cost"),
                       new CampaignAccessorPair(&SugarCampaign::actualCost, &SugarCampaign::setActualCost,
                                        i18nc("@item:intable", "Actual Costs")));
    mAccessors->insert(QLatin1String("expected_revenue"),
                       new CampaignAccessorPair(&SugarCampaign::expectedRevenue, &SugarCampaign::setExpectedRevenue,
                                        i18nc("@item:intable", "Expected Revenue")));
    mAccessors->insert(QLatin1String("campaign_type"),
                       new CampaignAccessorPair(&SugarCampaign::campaignType, &SugarCampaign::setCampaignType,
                                        i18nc("@item:intable", "Type")));
    mAccessors->insert(QLatin1String("objective"),
                       new CampaignAccessorPair(&SugarCampaign::objective, &SugarCampaign::setObjective,
                                        i18nc("@item:intable", "Objective")));
    mAccessors->insert(QLatin1String("content"),
                       new CampaignAccessorPair(&SugarCampaign::content, &SugarCampaign::setContent,
                                        i18nc("@item:intable", "Content")));
    mAccessors->insert(QLatin1String("frequency"),
                       new CampaignAccessorPair(&SugarCampaign::frequency, &SugarCampaign::setFrequency,
                                        i18nc("@item:intable", "Frequency")));
}

CampaignsHandler::~CampaignsHandler()
{
    qDeleteAll(*mAccessors);
    delete mAccessors;
}

Akonadi::Collection CampaignsHandler::handlerCollection() const
{
    Akonadi::Collection campaignCollection;
    campaignCollection.setRemoteId(moduleName());
    campaignCollection.setContentMimeTypes(QStringList() << SugarCampaign::mimeType());
    campaignCollection.setName(i18nc("@item folder name", "Campaigns"));
    campaignCollection.setRights(Akonadi::Collection::CanChangeItem |
                                 Akonadi::Collection::CanCreateItem |
                                 Akonadi::Collection::CanDeleteItem);

    return campaignCollection;
}

QString CampaignsHandler::orderByForListing() const
{
    return QLatin1String("campaigns.name");
}

QStringList CampaignsHandler::supportedFields() const
{
    return mAccessors->keys();
}

bool CampaignsHandler::setEntry(const Akonadi::Item &item)
{
    if (!item.hasPayload<SugarCampaign>()) {
        kError() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Campaign payload";
        return false;
    }

    QList<KDSoapGenerated::TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the campaign being added
    if (!item.remoteId().isEmpty()) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(QLatin1String("id"));
        field.setValue(item.remoteId());

        itemList << field;
    }

    const SugarCampaign campaign = item.payload<SugarCampaign>();
    CampaignAccessorHash::const_iterator it    = mAccessors->constBegin();
    CampaignAccessorHash::const_iterator endIt = mAccessors->constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }
        const valueGetter getter = (*it)->getter;
        KDSoapGenerated::TNS__Name_value field;
        field.setName(it.key());
        field.setValue(KDCRMUtils::encodeXML((campaign.*getter)()));

        itemList << field;
    }

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);
    soap()->asyncSet_entry(sessionId(), moduleName(), valueList);

    return true;
}

Akonadi::Item CampaignsHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection)
{
    Akonadi::Item item;

    const QList<KDSoapGenerated::TNS__Name_value> valueList = entry.name_value_list().items();
    if (valueList.isEmpty()) {
        kWarning() << "Campaigns entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId(entry.id());
    item.setParentCollection(parentCollection);
    item.setMimeType(SugarCampaign::mimeType());

    SugarCampaign campaign;
    campaign.setId(entry.id());
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList) {
        const CampaignAccessorHash::const_iterator accessIt = mAccessors->constFind(namedValue.name());
        if (accessIt == mAccessors->constEnd()) {
            // no accessor for field
            continue;
        }

        (campaign.*(accessIt.value()->setter))(KDCRMUtils::decodeXML(namedValue.value()));
    }
    item.setPayload<SugarCampaign>(campaign);
    item.setRemoteRevision(campaign.dateModified());

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

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "Local Campaign"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Serverside Campaign: modified by %1 on %2",
              modifiedBy, modifiedOn));

    CampaignAccessorHash::const_iterator it    = mAccessors->constBegin();
    CampaignAccessorHash::const_iterator endIt = mAccessors->constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }

        const QString diffName = (*it)->diffName;
        if (diffName.isEmpty()) {
            // TODO some fields like currency_id should be handled as special fields instead
            // i.e. currency string, dates formatted with KLocale
            continue;
        }

        const valueGetter getter = (*it)->getter;
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
