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

#include "opportunitieshandler.h"

#include "sugarsession.h"
#include "sugarsoap.h"
#include "kdcrmutils.h"
#include "kdcrmfields.h"

using namespace KDSoapGenerated;
#include <akonadi/collection.h>

#include <akonadi/abstractdifferencesreporter.h>

#include <KLocale>

OpportunitiesHandler::OpportunitiesHandler(SugarSession *session)
    : ModuleHandler(QLatin1String("Opportunities"), session),
      mAccessors(SugarOpportunity::accessorHash())
{
}

OpportunitiesHandler::~OpportunitiesHandler()
{
}

Akonadi::Collection OpportunitiesHandler::handlerCollection() const
{
    Akonadi::Collection myCollection;
    myCollection.setContentMimeTypes(QStringList() << SugarOpportunity::mimeType());
    myCollection.setName(i18nc("@item folder name", "Opportunities"));
    myCollection.setRights(Akonadi::Collection::CanChangeItem |
                                Akonadi::Collection::CanCreateItem |
                                Akonadi::Collection::CanDeleteItem);

    return myCollection;
}

bool OpportunitiesHandler::setEntry(const Akonadi::Item &item)
{
    if (!item.hasPayload<SugarOpportunity>()) {
        kError() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Opportunity payload";
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

    const SugarOpportunity opp = item.payload<SugarOpportunity>();
    SugarOpportunity::AccessorHash::const_iterator it    = mAccessors.constBegin();
    SugarOpportunity::AccessorHash::const_iterator endIt = mAccessors.constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }
        const SugarOpportunity::valueGetter getter = (*it).getter;
        KDSoapGenerated::TNS__Name_value field;
        field.setName(it.key());
        field.setValue(KDCRMUtils::encodeXML((opp.*getter)()));

        itemList << field;
    }

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);
    soap()->asyncSet_entry(sessionId(), moduleName(), valueList);

    return true;
}

QString OpportunitiesHandler::orderByForListing() const
{
    return QLatin1String("opportunities.name");
}

QStringList OpportunitiesHandler::supportedFields() const
{
    return mAccessors.keys();
}

Akonadi::Item OpportunitiesHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection)
{
    Akonadi::Item item;

    const QList<KDSoapGenerated::TNS__Name_value> valueList = entry.name_value_list().items();
    if (valueList.isEmpty()) {
        kWarning() << "Opportunities entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId(entry.id());
    item.setParentCollection(parentCollection);
    item.setMimeType(SugarOpportunity::mimeType());

    SugarOpportunity opportunity;
    opportunity.setId(entry.id());
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList) {
        //qDebug() << namedValue.name() << "=" << namedValue.value();
        const SugarOpportunity::AccessorHash::const_iterator accessIt = mAccessors.constFind(namedValue.name());
        if (accessIt == mAccessors.constEnd()) {
            qDebug() << "skipping field" << namedValue.name();
            // no accessor for field
            continue;
        }

        (opportunity.*(accessIt.value().setter))(KDCRMUtils::decodeXML(namedValue.value()));
    }
    item.setPayload<SugarOpportunity>(opportunity);
    item.setRemoteRevision(opportunity.dateModifiedRaw());

    return item;
}

void OpportunitiesHandler::compare(Akonadi::AbstractDifferencesReporter *reporter,
                                   const Akonadi::Item &leftItem, const Akonadi::Item &rightItem)
{
    Q_ASSERT(leftItem.hasPayload<SugarOpportunity>());
    Q_ASSERT(rightItem.hasPayload<SugarOpportunity>());

    const SugarOpportunity leftOpportunity = leftItem.payload<SugarOpportunity>();
    const SugarOpportunity rightOpportunity = rightItem.payload<SugarOpportunity>();

    const QString modifiedBy = mSession->userName();
    const QString modifiedOn = KDCRMUtils::formatDateTime(rightOpportunity.dateModified());

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "Local Opportunity"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Serverside Opportunity: modified by %1 on %2",
              modifiedBy, modifiedOn));

    SugarOpportunity::AccessorHash::const_iterator it    = mAccessors.constBegin();
    SugarOpportunity::AccessorHash::const_iterator endIt = mAccessors.constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }

        const SugarOpportunity::valueGetter getter = (*it).getter;
        QString leftValue = (leftOpportunity.*getter)();
        QString rightValue = (rightOpportunity.*getter)();

        QString diffName = (*it).diffName;
        if (diffName.isEmpty()) {
            if (it.key() == KDCRMFields::dateClosed()) {
                diffName = i18nc("@item:intable", "Expected Close Date");
                leftValue = formatDate(leftOpportunity.dateClosed());
                rightValue = formatDate(rightOpportunity.dateClosed());
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
