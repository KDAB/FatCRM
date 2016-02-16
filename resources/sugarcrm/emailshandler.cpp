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

#include "emailshandler.h"
#include "sugarcrmresource_debug.h"
#include "kdcrmutils.h"
#include "sugarsession.h"
#include "sugarsoap.h"
using namespace KDSoapGenerated;

#include <AkonadiCore/abstractdifferencesreporter.h> //krazy:exclude=camelcase
#include <AkonadiCore/Collection>

#include <KLocalizedString>

#include <QHash>

EmailsHandler::EmailsHandler(SugarSession *session)
    : ModuleHandler(QStringLiteral("Emails"), session),
      mAccessors(SugarEmail::accessorHash())
{
}

EmailsHandler::~EmailsHandler()
{
}

Akonadi::Collection EmailsHandler::handlerCollection() const
{
    Akonadi::Collection emailCollection;
    emailCollection.setRemoteId(moduleName());
    emailCollection.setContentMimeTypes(QStringList() << SugarEmail::mimeType());
    emailCollection.setName(i18nc("@item folder name", "Emails"));
    emailCollection.setRights(Akonadi::Collection::CanChangeItem |
                                 Akonadi::Collection::CanCreateItem |
                                 Akonadi::Collection::CanDeleteItem);

    return emailCollection;
}

QString EmailsHandler::queryStringForListing() const
{
    return QStringLiteral("emails.parent_type='Opportunities'");
}

QString EmailsHandler::orderByForListing() const
{
    return QStringLiteral("emails.name");
}

QStringList EmailsHandler::supportedSugarFields() const
{
    return sugarFieldsFromCrmFields(mAccessors.keys());
}

QStringList EmailsHandler::supportedCRMFields() const
{
    return mAccessors.keys();
}

void EmailsHandler::getExtraInformation(Akonadi::Item::List &items)
{
    /* EmailText contains e.g.
"email_id" = "286898c4-d48f-cd01-e620-4a1d3ad0428e"
"from_addr" = "Mirko Boehm &lt;mirko@kdab.net&gt;"
"reply_to_addr" = ""
"to_addrs" = "mirko@kdab.net"
"cc_addrs" = ""
"bcc_addrs" = ""
"description" = "Test. So."
"description_html" = "&lt;em&gt;Test.&lt;u&gt; So.&lt;/u&gt;&lt;br /&gt;&lt;/em&gt;"
"raw_source" = ""
"deleted" = "0"
*/

    QHash<QString, int> itemIndexById; // remoteId --> position in item list
    QString query;
    for (int i = 0; i < items.count(); ++i) {
        const Akonadi::Item &item = items.at(i);
        if (!query.isEmpty())
            query += " or ";
        query += "email_id='" + item.remoteId() + '\'';
        itemIndexById.insert(item.remoteId(), i);
    }
    KDSoapGenerated::TNS__Select_fields selectedFields;
    selectedFields.setItems(QStringList() << "email_id" << "description");
    // Blocking call
    KDSoapGenerated::TNS__Get_entry_list_result result =
            soap()->get_entry_list(sessionId(), "EmailText", query, QString() /*orderBy*/,
                                   0 /*offset*/, selectedFields, items.count() /*maxResults*/, 0 /*fetchDeleted*/);

    foreach(const KDSoapGenerated::TNS__Entry_value &entry, result.entry_list().items()) {
        QString email_id, description;
        foreach(const KDSoapGenerated::TNS__Name_value &val, entry.name_value_list().items()) {
            if (val.name() == "email_id") {
                email_id = val.value();
            } else if (val.name() == "description") {
                description = KDCRMUtils::decodeXML(val.value());
            }
        }
        if (email_id.isEmpty()) {
            qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "No email_id found in entry";
        } else {
            const int pos = itemIndexById.value(email_id, -1);
            if (pos == -1) {
                qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "Email not found:" << email_id;
            } else {
                SugarEmail email = items[pos].payload<SugarEmail>();
                email.setDescription(description);
                items[pos].setPayload<SugarEmail>(email);
            }
        }
    }
}

bool EmailsHandler::setEntry(const Akonadi::Item &item)
{
    if (!item.hasPayload<SugarEmail>()) {
        qCritical() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Email payload";
        return false;
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

    const SugarEmail email = item.payload<SugarEmail>();
    SugarEmail::AccessorHash::const_iterator it    = mAccessors.constBegin();
    SugarEmail::AccessorHash::const_iterator endIt = mAccessors.constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }
        const SugarEmail::valueGetter getter = (*it).getter;
        KDSoapGenerated::TNS__Name_value field;
        field.setName(sugarFieldFromCrmField(it.key()));
        field.setValue(KDCRMUtils::encodeXML((email.*getter)()));

        itemList << field;
    }

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);
    soap()->asyncSet_entry(sessionId(), moduleName(), valueList);

    return true;
}

Akonadi::Item EmailsHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection)
{
    Akonadi::Item item;

    const QList<KDSoapGenerated::TNS__Name_value> valueList = entry.name_value_list().items();
    if (valueList.isEmpty()) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "Emails entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId(entry.id());
    item.setParentCollection(parentCollection);
    item.setMimeType(SugarEmail::mimeType());

    SugarEmail email;
    email.setId(entry.id());
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList) {
        const QString crmFieldName = sugarFieldToCrmField(namedValue.name());
        const SugarEmail::AccessorHash::const_iterator accessIt = mAccessors.constFind(crmFieldName);
        if (accessIt == mAccessors.constEnd()) {
            // no accessor for field
            continue;
        }

        (email.*(accessIt.value().setter))(KDCRMUtils::decodeXML(namedValue.value()));
    }
    item.setPayload<SugarEmail>(email);
    item.setRemoteRevision(email.dateModified());

    return item;
}

void EmailsHandler::compare(Akonadi::AbstractDifferencesReporter *reporter,
                               const Akonadi::Item &leftItem, const Akonadi::Item &rightItem)
{
    Q_ASSERT(leftItem.hasPayload<SugarEmail>());
    Q_ASSERT(rightItem.hasPayload<SugarEmail>());

    const SugarEmail leftEmail = leftItem.payload<SugarEmail>();
    const SugarEmail rightEmail = rightItem.payload<SugarEmail>();

    const QString modifiedBy = mSession->userName();
    const QString modifiedOn = formatDate(rightEmail.dateModified());

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "Local Email"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Serverside Email: modified by %1 on %2",
              modifiedBy, modifiedOn));

    SugarEmail::AccessorHash::const_iterator it    = mAccessors.constBegin();
    SugarEmail::AccessorHash::const_iterator endIt = mAccessors.constEnd();
    for (; it != endIt; ++it) {
        const QString diffName = (*it).diffName;
        if (diffName.isEmpty()) {
            // TODO some fields like currency_id should be handled as special fields instead
            // i.e. currency string, dates formatted with KLocale
            continue;
        }

        const SugarEmail::valueGetter getter = (*it).getter;
        const QString leftValue = (leftEmail.*getter)();
        const QString rightValue = (rightEmail.*getter)();

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
