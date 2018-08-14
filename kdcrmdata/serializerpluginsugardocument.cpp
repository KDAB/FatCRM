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

#include "serializerpluginsugardocument.h"

#include "sugardocument.h"
#include "sugardocumentio.h"

#include <AkonadiCore/Item>

#include <QtCore/qplugin.h>

using namespace Akonadi;

bool SerializerPluginSugarDocument::deserialize(Item &item, const QByteArray &label, QIODevice &data, int version)
{
    Q_UNUSED(version);

    if (label != Item::FullPayload) {
        return false;
    }

    SugarDocument sugarDocument;
    SugarDocumentIO io;
    if (!io.readSugarDocument(&data, sugarDocument)) {
        return false;
    }

    item.setPayload<SugarDocument>(sugarDocument);

    return true;
}

void SerializerPluginSugarDocument::serialize(const Item &item, const QByteArray &label, QIODevice &data, int &version)
{
    Q_UNUSED(version);

    if (label != Item::FullPayload || !item.hasPayload<SugarDocument>()) {
        return;
    }

    const SugarDocument sugarDocument = item.payload<SugarDocument>();
    SugarDocumentIO io;
    io.writeSugarDocument(sugarDocument, &data);
}

QString SerializerPluginSugarDocument::extractGid(const Item &item) const
{
    if (item.hasPayload<SugarDocument>()) {
        const SugarDocument sugarDocument = item.payload<SugarDocument>();
        return sugarDocument.id();
    }
    return QString();
}

//Q_EXPORT_PLUGIN2(akonadi_serializer_sugardocument, Akonadi::SerializerPluginSugarDocument)

