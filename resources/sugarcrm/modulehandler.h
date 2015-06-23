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

#ifndef MODULEHANDLER_H
#define MODULEHANDLER_H

#include <akonadi/differencesalgorithminterface.h> //krazy:exclude=camelcase
#include <Akonadi/Item>
#include <Akonadi/Collection>

#include <QStringList>

class SugarSession;
class ListEntriesScope;

namespace KDSoapGenerated
{
class Sugarsoap;
class TNS__Entry_list;
class TNS__Entry_value;
}

class ModuleHandler : public QObject, public Akonadi::DifferencesAlgorithmInterface
{
    Q_OBJECT
    Q_INTERFACES(Akonadi::DifferencesAlgorithmInterface) // just to silence moc
public:
    explicit ModuleHandler(const QString &moduleName, SugarSession *session);

    virtual ~ModuleHandler();

    QString moduleName() const;

    void initialCheck();

    QString latestTimestamp() const;
    void resetLatestTimestamp();

    Akonadi::Collection collection();
    void modifyCollection(const Akonadi::Collection &collection);

    void getEntriesCount(const ListEntriesScope &scope);
    void listEntries(const ListEntriesScope &scope);

    QStringList availableFields() const;
    static QStringList listAvailableFields(SugarSession *session, const QString &module);

    virtual bool setEntry(const Akonadi::Item &item) = 0;
    virtual int expectedContentsVersion() const { return 0; }

    bool getEntry(const Akonadi::Item &item);

    // Return true if the handler wants to fetch extra information on listed items
    // (e.g. email text)
    virtual bool needsExtraInformation() const { return false; }
    virtual void getExtraInformation(Akonadi::Item::List &) {}

    virtual QString queryStringForListing() const { return QString(); }
    virtual QString orderByForListing() const = 0;

    // List of fields to pass the Sugar SOAP interface
    virtual QStringList supportedSugarFields() const = 0;

    // List of fields saved into the QMap data
    // and into the XML. This is then communicated to the client app.
    virtual QStringList supportedCRMFields() const = 0;

    virtual Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry,
                                        const Akonadi::Collection &parentCollection) = 0;

    Akonadi::Item::List itemsFromListEntriesResponse(const KDSoapGenerated::TNS__Entry_list &entryList,
            const Akonadi::Collection &parentCollection);

    virtual bool needBackendChange(const Akonadi::Item &item, const QSet<QByteArray> &modifiedParts) const;

protected:
    SugarSession *mSession;
    QString mModuleName;
    QString mLatestTimestamp;
    Akonadi::Collection mCollection;

protected:
    static QString formatDate(const QString &dateString);
    static QByteArray partIdFromPayloadPart(const char *part);

    virtual Akonadi::Collection handlerCollection() const = 0;

    QString sessionId() const;
    KDSoapGenerated::Sugarsoap *soap() const;

private Q_SLOTS:
    void slotCollectionModifyResult(KJob *);
    void slotCollectionsReceived(const Akonadi::Collection::List &collections);

private:
    mutable QStringList mAvailableFields;

};

#endif
