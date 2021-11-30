/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef MODULEHANDLER_H
#define MODULEHANDLER_H

#include "kdcrmdata/enumdefinitions.h"

#include <Akonadi/DifferencesAlgorithmInterface>
#include <Akonadi/Item>
#include <Akonadi/Collection>

#include <QStringList>
#include "sugarprotocolbase.h"
#include "modulename.h"

class SugarSession;
class ListEntriesScope;

namespace KDSoapGenerated
{
class TNS__Entry_list;
class TNS__Entry_value;
class TNS__Field_list;
class TNS__Get_entry_list_result;
}

class ModuleHandler : public QObject, public Akonadi::DifferencesAlgorithmInterface
{
    Q_OBJECT
    Q_INTERFACES(Akonadi::DifferencesAlgorithmInterface) // just to silence moc
public:
    explicit ModuleHandler(const Module module, SugarSession *session);

    ~ModuleHandler() override;

    Module module() const;

    void initialCheck();

    QString latestTimestamp() const;

    Akonadi::Collection collection();
    void modifyCollection(const Akonadi::Collection &collection);

    int getEntriesCount(const ListEntriesScope &scope, int &entriesCount, QString &errorMessage);
    int listEntries(const ListEntriesScope &scope, EntriesListResult &entriesListResult, QString &errorMessage);

    QStringList availableFields() const;
    static KDSoapGenerated::TNS__Field_list listAvailableFields(SugarSession *session, const QString &module);
    static QStringList listAvailableFieldNames(SugarSession *session, const QString &module);

    virtual int setEntry(const Akonadi::Item &item, QString &id, QString &errorMessage) = 0;
    virtual int expectedContentsVersion() const { return 0; }

    int getEntry(const Akonadi::Item &item, KDSoapGenerated::TNS__Entry_value &entryValue, QString &errorMessage);

    // Return true if the handler wants to fetch extra information on listed items
    // (e.g. email text, linked doucments, linked contacts...)
    virtual bool needsExtraInformation() const { return false; }
    virtual void getExtraInformation(Akonadi::Item::List &) {}
    virtual int saveExtraInformation(const Akonadi::Item &item, const QString &id, QString &errorMessage) {
        Q_UNUSED(item)
        Q_UNUSED(id)
        Q_UNUSED(errorMessage)
        return KJob::NoError;
    }

    virtual QString queryStringForListing() const { return QString(); }
    virtual QString orderByForListing() const = 0;

    // List of fields to pass the Sugar SOAP interface
    virtual QStringList supportedSugarFields() const = 0;

    // List of fields saved into the QMap data
    // and into the XML. This is then communicated to the client app.
    virtual QStringList supportedCRMFields() const = 0;

    bool hasEnumDefinitions() const;
    EnumDefinitions enumDefinitions() const; // for the unittest

    virtual Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry,
                                        const Akonadi::Collection &parentCollection,
                                        bool &deleted) = 0;

    bool parseFieldList(Akonadi::Collection &collection, const KDSoapGenerated::TNS__Field_list &fields);

    Akonadi::Item::List itemsFromListEntriesResponse(const KDSoapGenerated::TNS__Entry_list &entryList,
            const Akonadi::Collection &parentCollection, Akonadi::Item::List &deletedItems, QString *lastTimestamp);

    virtual bool needBackendChange(const Akonadi::Item &item, const QSet<QByteArray> &modifiedParts) const;

protected:
    SugarSession *mSession;
    Module mModule;
    QString mLatestTimestamp;
    Akonadi::Collection mCollection;

protected:
    static QString formatDate(const QString &dateString);
    static QByteArray partIdFromPayloadPart(const char *part);

    virtual Akonadi::Collection handlerCollection() const = 0;

    static QString sugarFieldToCrmField(const QString &sugarFieldName);
    static QString customSugarFieldToCrmField(const QString &sugarFieldName);
    QStringList sugarFieldsToCrmFields(const QStringList &sugarFieldNames) const;
    static QString sugarFieldFromCrmField(const QString &crmFieldName);
    static QString customSugarFieldFromCrmField(const QString &crmFieldName);
    QStringList sugarFieldsFromCrmFields(const QStringList &crmFieldNames) const;

    QString sessionId() const;

private Q_SLOTS:
    void slotCollectionModifyResult(KJob *);
    void slotCollectionsReceived(const Akonadi::Collection::List &collections);

private:
    static const QMap<QString, QString>& fieldNamesMapping();

    mutable QStringList mAvailableFields;

    EnumDefinitions mEnumDefinitions;
    bool mParsedEnumDefinitions;
    bool mHasEnumDefinitions; // whether present in DB (possibly old, though)
};

#endif
