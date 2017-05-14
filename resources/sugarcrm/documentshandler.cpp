/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "sugarsoap.h"
#include "sugarjob.h"
using namespace KDSoapGenerated;

#include <AkonadiCore/abstractdifferencesreporter.h> //krazy:exclude=camelcase
#include <AkonadiCore/Collection>

#include <KLocalizedString>

#include <QHash>

DocumentsHandler::DocumentsHandler(SugarSession *session)
    : ModuleHandler(QStringLiteral("Documents"), session),
      mAccessors(SugarDocument::accessorHash())
{
}

DocumentsHandler::~DocumentsHandler()
{
}

Akonadi::Collection DocumentsHandler::handlerCollection() const
{
    Akonadi::Collection documentCollection;
    documentCollection.setRemoteId(moduleName());
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
    return sugarFieldsFromCrmFields(mAccessors.keys());
}

QStringList DocumentsHandler::supportedCRMFields() const
{
    return mAccessors.keys();
}

int DocumentsHandler::expectedContentsVersion() const
{
    return 0;
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
    SugarDocument::AccessorHash::const_iterator it    = mAccessors.constBegin();
    SugarDocument::AccessorHash::const_iterator endIt = mAccessors.constEnd();
    for (; it != endIt; ++it) {
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
    QMap<QString, QString> customFields = document.customFields();
    QMap<QString, QString>::const_iterator cit = customFields.constBegin();
    const QMap<QString, QString>::const_iterator end = customFields.constEnd();
    for ( ; cit != end ; ++cit ) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(customSugarFieldFromCrmField(cit.key()));
        field.setValue(KDCRMUtils::encodeXML(cit.value()));
        itemList << field;
    }

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);

    return mSession->protocol()->setEntry(moduleName(), valueList, newId, errorMessage);
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

        KDSoapGenerated::TNS__Get_relationships_result result = soap()->get_relationships(sessionId(), QStringLiteral("Documents"), document.id(), QStringLiteral("Accounts"), QString(), 0);

        QStringList linkedAccountIds;
        Q_FOREACH (const KDSoapGenerated::TNS__Id_mod &idMod, result.ids().items()) {
            linkedAccountIds.append(idMod.id());
        }

        if (!linkedAccountIds.isEmpty()) {
            document.setLinkedAccountIds(linkedAccountIds);
            update = true;
        }

        QStringList linkedOpportunityIds;
        result = soap()->get_relationships(sessionId(), QStringLiteral("Documents"), document.id(), QStringLiteral("Opportunities"), QString(), 0);
        Q_FOREACH (const KDSoapGenerated::TNS__Id_mod &idMod, result.ids().items()) {
            linkedOpportunityIds.append(idMod.id());
        }

        if (!linkedOpportunityIds.isEmpty()) {
            document.setLinkedOpportunityIds(linkedOpportunityIds);
            update = true;
        }

        if (update)
            item.setPayload<SugarDocument>(document);
    }
}

Akonadi::Item DocumentsHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection)
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
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList) {
        const QString crmFieldName = sugarFieldToCrmField(namedValue.name());
        const QString value = KDCRMUtils::decodeXML(namedValue.value());
        const SugarDocument::AccessorHash::const_iterator accessIt = mAccessors.constFind(crmFieldName);
        if (accessIt == mAccessors.constEnd()) {
            const QString crmCustomFieldName = customSugarFieldToCrmField(namedValue.name());
            document.setCustomField(crmCustomFieldName, value);
            continue;
        }

        (document.*(accessIt.value().setter))(KDCRMUtils::decodeXML(value));
    }
    item.setPayload<SugarDocument>(document);
    item.setRemoteRevision(document.dateModified());

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
    const QString modifiedOn = formatDate(rightDocument.dateModified());

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "Local Document"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Serverside Document: modified by %1 on %2",
              modifiedBy, modifiedOn));

    SugarDocument::AccessorHash::const_iterator it    = mAccessors.constBegin();
    SugarDocument::AccessorHash::const_iterator endIt = mAccessors.constEnd();
    for (; it != endIt; ++it) {
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
