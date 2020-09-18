/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Tobias Koenig <tobias.koenig@kdab.com>

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

#include "documentshandler.h"

#include "kdcrmutils.h"
#include "sugarsession.h"
#include "sugarcrmresource_debug.h"
#include "wsdl_sugar41.h"
#include "sugarjob.h"
using namespace KDSoapGenerated;

#include <AkonadiCore/abstractdifferencesreporter.h> //krazy:exclude=camelcase
#include <AkonadiCore/Collection>

#include <KLocalizedString>

#include <QHash>

DocumentsHandler::DocumentsHandler(SugarSession *session)
    : ModuleHandler(Module::Documents, session)
{
}

DocumentsHandler::~DocumentsHandler()
{
}

Akonadi::Collection DocumentsHandler::handlerCollection() const
{
    Akonadi::Collection documentCollection;
    documentCollection.setRemoteId(moduleToName(module()));
    documentCollection.setContentMimeTypes(QStringList() << SugarDocument::mimeType());
    documentCollection.setName(i18nc("@item folder name", "Documents"));
    documentCollection.setRights(Akonadi::Collection::CanChangeItem |
                                 Akonadi::Collection::CanCreateItem |
                                 Akonadi::Collection::CanDeleteItem);

    return documentCollection;
}

QString DocumentsHandler::orderByForListing() const
{
    return QStringLiteral("documents.document_name");
}

QStringList DocumentsHandler::supportedSugarFields() const
{
    return sugarFieldsFromCrmFields(SugarDocument::accessorHash().keys());
}

QStringList DocumentsHandler::supportedCRMFields() const
{
    return SugarDocument::accessorHash().keys();
}

int DocumentsHandler::expectedContentsVersion() const
{
    // 0: initial version
    // 1: after fixing getExtraInformation to fetch linked accounts/opps correctly, upgraded to force a full refetch
    return 1;
}

int DocumentsHandler::setEntry(const Akonadi::Item &item, QString &newId, QString &errorMessage)
{
    if (!item.hasPayload<SugarDocument>()) {
        qCCritical(FATCRM_SUGARCRMRESOURCE_LOG) << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Document payload";
        return SugarJob::InvalidContextError;
    }

    QList<KDSoapGenerated::TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the email being added
    if (!item.remoteId().isEmpty()) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(QStringLiteral("id"));
        field.setValue(item.remoteId());

        itemList << field;
    }

    const SugarDocument document = item.payload<SugarDocument>();
    const SugarDocument::AccessorHash accessors = SugarDocument::accessorHash();
    for (auto it = accessors.constBegin(); it != accessors.constEnd(); ++it) {
        // check if this is a read-only field
        if (it.key() == QLatin1String("id")) {
            continue;
        }

        const SugarDocument::valueGetter getter = (*it).getter;
        KDSoapGenerated::TNS__Name_value field;
        field.setName(sugarFieldFromCrmField(it.key()));
        field.setValue(KDCRMUtils::encodeXML((document.*getter)()));

        itemList << field;
    }

    // plus custom fields
    const QMap<QString, QString> customFields = document.customFields();
    for (auto cit = customFields.constBegin(); cit != customFields.constEnd(); ++cit ) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(customSugarFieldFromCrmField(cit.key()));
        field.setValue(KDCRMUtils::encodeXML(cit.value()));
        itemList << field;
    }

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);

    return mSession->protocol()->setEntry(module(), valueList, newId, errorMessage);
}

bool DocumentsHandler::needsExtraInformation() const
{
    return true;
}

void DocumentsHandler::getExtraInformation(Akonadi::Item::List &items)
{
    for (int pos = 0; pos < items.count(); ++pos) {
        Akonadi::Item &item = items[pos];

        if (!item.hasPayload<SugarDocument>()) {
            qCCritical(FATCRM_SUGARCRMRESOURCE_LOG) << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                     << ", mime=" << item.mimeType() << ") is missing Document payload";
            continue;
        }

        SugarDocument document = item.payload<SugarDocument>();
        bool update = false;

        KDSoapGenerated::TNS__Select_fields selectedFields;
        selectedFields.setItems({QStringLiteral("id")});

        // Get the Account(s) related to this document
        // https://support.sugarcrm.com/Documentation/Sugar_Developer/Sugar_Developer_Guide_6.5/Application_Framework/Web_Services/Method_Calls/get_relationships/
        // https://support.sugarcrm.com/Documentation/Sugar_Developer/Sugar_Developer_Guide_6.5/Application_Framework/Web_Services/Examples/REST/PHP/Retrieving_Related_Records/
        KDSoapGenerated::TNS__Get_entry_result_version2 result = mSession->soap()->get_relationships(sessionId(), QStringLiteral("Documents"), document.id(), moduleToName(Module::Accounts).toLower(), {}, selectedFields,
                                                                   {}, 0 /*deleted*/, QString(), 0 /*offset*/, 0 /*limit*/);

        QStringList linkedAccountIds;
        Q_FOREACH (const KDSoapGenerated::TNS__Entry_value& entry, result.entry_list().items()) {
            linkedAccountIds.append(entry.id());
        }

        if (!linkedAccountIds.isEmpty()) {
            document.setLinkedAccountIds(linkedAccountIds);
            update = true;
        }

        QStringList linkedOpportunityIds;
        result = mSession->soap()->get_relationships(sessionId(), QStringLiteral("Documents"), document.id(), moduleToName(Module::Opportunities).toLower(), {}, selectedFields,
                                           {}, 0 /*deleted*/, QString(), 0 /*offset*/, 0 /*limit*/);

        Q_FOREACH (const KDSoapGenerated::TNS__Entry_value &entry, result.entry_list().items()) {
            linkedOpportunityIds.append(entry.id());
        }

        if (!linkedOpportunityIds.isEmpty()) {
            document.setLinkedOpportunityIds(linkedOpportunityIds);
            update = true;
        }

        if (update) {
            item.setPayload<SugarDocument>(document);
        }
    }
}

Akonadi::Item DocumentsHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection, bool &deleted)
{
    Akonadi::Item item;

    const QList<KDSoapGenerated::TNS__Name_value> valueList = entry.name_value_list().items();
    if (valueList.isEmpty()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "Documents entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId(entry.id());
    item.setParentCollection(parentCollection);
    item.setMimeType(SugarDocument::mimeType());

    SugarDocument document;
    document.setId(entry.id());
    const SugarDocument::AccessorHash accessors = SugarDocument::accessorHash();
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList) {
        const QString crmFieldName = sugarFieldToCrmField(namedValue.name());
        const QString value = KDCRMUtils::decodeXML(namedValue.value());
        const SugarDocument::AccessorHash::const_iterator accessIt = accessors.constFind(crmFieldName);
        if (accessIt == accessors.constEnd()) {
            const QString crmCustomFieldName = customSugarFieldToCrmField(namedValue.name());
            document.setCustomField(crmCustomFieldName, value);
            continue;
        }

        (document.*(accessIt.value().setter))(KDCRMUtils::decodeXML(value));
    }
    item.setPayload<SugarDocument>(document);
    item.setRemoteRevision(document.dateModifiedRaw());

    deleted = document.deleted() == QLatin1String("1");

    return item;
}

void DocumentsHandler::compare(Akonadi::AbstractDifferencesReporter *reporter,
                               const Akonadi::Item &leftItem, const Akonadi::Item &rightItem)
{
    Q_ASSERT(leftItem.hasPayload<SugarDocument>());
    Q_ASSERT(rightItem.hasPayload<SugarDocument>());

    const SugarDocument leftDocument = leftItem.payload<SugarDocument>();
    const SugarDocument rightDocument = rightItem.payload<SugarDocument>();

    const QString modifiedBy = mSession->userName();
    const QString modifiedOn = KDCRMUtils::formatDateTime(rightDocument.dateModified());

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "My Document"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Their Document: modified by %1 on %2",
              modifiedBy, modifiedOn));

    const SugarDocument::AccessorHash accessors = SugarDocument::accessorHash();
    for (auto it = accessors.constBegin(); it != accessors.constEnd(); ++it) {
        const QString diffName = (*it).diffName;
        if (diffName.isEmpty()) {
            continue;
        }

        const SugarDocument::valueGetter getter = (*it).getter;
        const QString leftValue = (leftDocument.*getter)();
        const QString rightValue = (rightDocument.*getter)();

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
