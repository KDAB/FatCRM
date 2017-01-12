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

#ifndef COLLECTIONMANAGER_H
#define COLLECTIONMANAGER_H

#include <QObject>
#include <QSet>
#include <QStringList>

#include "enums.h"
#include "enumdefinitions.h"

#include <Akonadi/Collection>

class KJob;

/**
 * The CollectionManager lists the collections initially, and emits them, in the desired order for loading.
 */
class CollectionManager : public QObject
{
    Q_OBJECT
public:
    explicit CollectionManager(QObject *parent = 0);

    /// This is what triggers the collection finding
    void setResource(const QByteArray &identifier);

    /// Returns the collection ID for the given type of main object (account, contact, opp)
    /// Note that the collection manager also knows about other collections (emails, documents etc.)
    /// but you can use LinkedItemsRepository to get those.
    Akonadi::Collection::Id collectionIdForType(DetailsType detailsType) const;

    QStringList supportedFields(Akonadi::Collection::Id collectionId) const;
    EnumDefinitions enumDefinitions(Akonadi::Collection::Id collectionId) const;

    QList<KJob *> clearTimestamps();

public slots:
    /// Update cached data when a collection changes.
    void slotCollectionChanged(const Akonadi::Collection &collection, const QSet<QByteArray> &attributeNames);

signals:
    void collectionResult(const QString &mimeType, const Akonadi::Collection &collection);

private slots:
    void slotCollectionFetchResult(KJob *job);

private:
    void readSupportedFields(const Akonadi::Collection &collection);
    void readEnumDefinitionAttributes(const Akonadi::Collection &collection);

    struct CollectionData
    {
        QStringList supportedFields;
        EnumDefinitions enumDefinitions;
        Akonadi::Collection mCollection; // to keep a copy of the current attributes
    };

    QHash<Akonadi::Collection::Id, CollectionData> mCollectionData;

    QVector<Akonadi::Collection::Id> mMainCollectionIds; // index = DetailsType
};

#endif // COLLECTIONMANAGER_H
