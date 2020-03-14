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

#include "sugardocumentio.h"
#include "sugardocument.h"

#include <KLocalizedString>
#include <QDebug>
#include <QHash>
#include <QMap>
#include <QIODevice>
#include <QXmlStreamWriter>

static const char s_linkedAccountIdsKey[] = "linked_account_ids";
static const char s_linkedOpportunityIdsKey[] = "linked_opportunity_ids";

SugarDocumentIO::SugarDocumentIO()
{
}

bool SugarDocumentIO::readSugarDocument(QIODevice *device, SugarDocument &document)
{
    if (device == nullptr || !device->isReadable()) {
        return false;
    }

    document = SugarDocument();
    xml.setDevice(device);
    if (xml.readNextStartElement()) {
        if (xml.name() == "sugarDocument"
                && xml.attributes().value(QStringLiteral("version")) == "1.0") {
            readDocument(document);
        } else {
            xml.raiseError(i18n("It is not a sugarDocument version 1.0 data."));
        }

    }
    return !xml.error();
}

QString SugarDocumentIO::errorString() const
{
    return i18n("%1\nLine %2, column %3",
           xml.errorString(),
           xml.lineNumber(),
           xml.columnNumber());
}

void SugarDocumentIO::readDocument(SugarDocument &document)
{
    const SugarDocument::AccessorHash accessors = SugarDocument::accessorHash();
    Q_ASSERT(xml.isStartElement() && xml.name() == "sugarDocument");

    while (xml.readNextStartElement()) {
        const QString key = xml.name().toString();
        const QString value = xml.readElementText();
        const SugarDocument::AccessorHash::const_iterator accessIt = accessors.constFind(key);
        if (accessIt != accessors.constEnd()) {
            (document.*(accessIt.value().setter))(value);
        } else {
            if (key == QLatin1String(s_linkedAccountIdsKey)) {
                const QStringList ids = value.split(QLatin1Char(','));
                document.setLinkedAccountIds(ids);
            } else if (key == QLatin1String(s_linkedOpportunityIdsKey)) {
                const QStringList ids = value.split(QLatin1Char(','));
                document.setLinkedOpportunityIds(ids);
            } else {
                document.setCustomField(key, value);
            }
        }
    }
}

bool SugarDocumentIO::writeSugarDocument(const SugarDocument &document, QIODevice *device)
{
    if (device == nullptr || !device->isWritable()) {
        return false;
    }

    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeDTD(QStringLiteral("<!DOCTYPE sugarDocument>"));
    writer.writeStartElement(QStringLiteral("sugarDocument"));
    writer.writeAttribute(QStringLiteral("version"), QStringLiteral("1.0"));

    const SugarDocument::AccessorHash accessors = SugarDocument::accessorHash();
    SugarDocument::AccessorHash::const_iterator it    = accessors.constBegin();
    SugarDocument::AccessorHash::const_iterator endIt = accessors.constEnd();
    for (; it != endIt; ++it) {
        const SugarDocument::valueGetter getter = (*it).getter;
        writer.writeTextElement(it.key(), (document.*getter)());
    }

    if (!document.linkedAccountIds().isEmpty())
        writer.writeTextElement(s_linkedAccountIdsKey, document.linkedAccountIds().join(QStringLiteral(",")));

    if (!document.linkedOpportunityIds().isEmpty())
        writer.writeTextElement(s_linkedOpportunityIdsKey, document.linkedOpportunityIds().join(QStringLiteral(",")));

    // plus custom fields
    QMap<QString, QString> customFields = document.customFields();
    QMap<QString, QString>::const_iterator cit = customFields.constBegin();
    const QMap<QString, QString>::const_iterator end = customFields.constEnd();
    for ( ; cit != end ; ++cit ) {
        writer.writeTextElement(cit.key(), cit.value());
    }

    writer.writeEndDocument();

    return true;
}
