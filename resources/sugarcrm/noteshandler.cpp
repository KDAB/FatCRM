/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "noteshandler.h"

#include "kdcrmutils.h"
#include "sugarsession.h"
#include "sugarsoap.h"
#include "sugarjob.h"

using namespace KDSoapGenerated;
#include <akonadi/abstractdifferencesreporter.h> //krazy:exclude=camelcase
#include <Akonadi/Collection>

#include <KLocale>

#include <QHash>

NotesHandler::NotesHandler(SugarSession *session)
    : ModuleHandler(Module::Notes, session),
      mAccessors(SugarNote::accessorHash())
{
}

NotesHandler::~NotesHandler()
{
}

Akonadi::Collection NotesHandler::handlerCollection() const
{
    Akonadi::Collection noteCollection;
    noteCollection.setRemoteId(moduleToName(module()));
    noteCollection.setContentMimeTypes(QStringList() << SugarNote::mimeType());
    noteCollection.setName(i18nc("@item folder name", "Notes"));
    noteCollection.setRights(Akonadi::Collection::CanChangeItem |
                                 Akonadi::Collection::CanCreateItem |
                                 Akonadi::Collection::CanDeleteItem);

    return noteCollection;
}

QString NotesHandler::queryStringForListing() const
{
    return QLatin1String("notes.parent_type='Opportunities' or notes.parent_type='Accounts' or notes.parent_type='Contacts'");
}

QString NotesHandler::orderByForListing() const
{
    return QLatin1String("notes.name");
}

QStringList NotesHandler::supportedSugarFields() const
{
    return sugarFieldsFromCrmFields(mAccessors.keys());
}

QStringList NotesHandler::supportedCRMFields() const
{
    return mAccessors.keys();
}

int NotesHandler::expectedContentsVersion() const
{
    // version 1 = query notes for accounts and contacts as well
    // version 2 = fixed the wrong query, full download required
    return 2;
}

int NotesHandler::setEntry(const Akonadi::Item &item, QString &newId, QString &errorMessage)
{
    if (!item.hasPayload<SugarNote>()) {
        kError() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Note payload";
        return SugarJob::InvalidContextError;
    }

    QList<KDSoapGenerated::TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the note being added
    if (!item.remoteId().isEmpty()) {
        KDSoapGenerated::TNS__Name_value field;
        field.setName(QLatin1String("id"));
        field.setValue(item.remoteId());

        itemList << field;
    }

    const SugarNote note = item.payload<SugarNote>();
    SugarNote::AccessorHash::const_iterator it    = mAccessors.constBegin();
    SugarNote::AccessorHash::const_iterator endIt = mAccessors.constEnd();
    for (; it != endIt; ++it) {
        // check if this is a read-only field
        if (it.key() == "id") {
            continue;
        }
        const SugarNote::valueGetter getter = (*it).getter;
        KDSoapGenerated::TNS__Name_value field;
        field.setName(sugarFieldFromCrmField(it.key()));
        field.setValue(KDCRMUtils::encodeXML((note.*getter)()));

        itemList << field;
    }

    KDSoapGenerated::TNS__Name_value_list valueList;
    valueList.setItems(itemList);

    return mSession->protocol()->setEntry(module(), valueList, newId, errorMessage);
}

Akonadi::Item NotesHandler::itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection)
{
    Akonadi::Item item;

    const QList<KDSoapGenerated::TNS__Name_value> valueList = entry.name_value_list().items();
    if (valueList.isEmpty()) {
        kWarning() << "Notes entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId(entry.id());
    item.setParentCollection(parentCollection);
    item.setMimeType(SugarNote::mimeType());

    SugarNote note;
    note.setId(entry.id());
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &namedValue, valueList) {
        const QString crmFieldName = sugarFieldToCrmField(namedValue.name());
        const SugarNote::AccessorHash::const_iterator accessIt = mAccessors.constFind(crmFieldName);
        if (accessIt == mAccessors.constEnd()) {
            // no accessor for field
            continue;
        }

        (note.*(accessIt.value().setter))(KDCRMUtils::decodeXML(namedValue.value()));
    }
    item.setPayload<SugarNote>(note);
    item.setRemoteRevision(note.dateModified());

    return item;
}

void NotesHandler::compare(Akonadi::AbstractDifferencesReporter *reporter,
                               const Akonadi::Item &leftItem, const Akonadi::Item &rightItem)
{
    Q_ASSERT(leftItem.hasPayload<SugarNote>());
    Q_ASSERT(rightItem.hasPayload<SugarNote>());

    const SugarNote leftNote = leftItem.payload<SugarNote>();
    const SugarNote rightNote = rightItem.payload<SugarNote>();

    const QString modifiedBy = mSession->userName();
    const QString modifiedOn = formatDate(rightNote.dateModified());

    reporter->setLeftPropertyValueTitle(i18nc("@title:column", "Local Note"));
    reporter->setRightPropertyValueTitle(
        i18nc("@title:column", "Serverside Note: modified by %1 on %2",
              modifiedBy, modifiedOn));

    SugarNote::AccessorHash::const_iterator it    = mAccessors.constBegin();
    SugarNote::AccessorHash::const_iterator endIt = mAccessors.constEnd();
    for (; it != endIt; ++it) {
        const QString diffName = (*it).diffName;
        if (diffName.isEmpty()) {
            // TODO some fields like currency_id should be handled as special fields instead
            // i.e. currency string, dates formatted with KLocale
            continue;
        }

        const SugarNote::valueGetter getter = (*it).getter;
        const QString leftValue = (leftNote.*getter)();
        const QString rightValue = (rightNote.*getter)();

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
