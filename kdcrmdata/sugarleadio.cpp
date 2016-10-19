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

#include "sugarleadio.h"
#include "sugarlead.h"
#include "kdcrmfields.h"

#include <KLocalizedString>
#include <QHash>
#include <QIODevice>
#include <QXmlStreamWriter>

SugarLeadIO::SugarLeadIO()
{
}

bool SugarLeadIO::readSugarLead(QIODevice *device, SugarLead &lead)
{
    if (device == nullptr || !device->isReadable()) {
        return false;
    }

    lead = SugarLead();
    xml.setDevice(device);
    if (xml.readNextStartElement()) {
        if (xml.name() == "sugarLead"
                && xml.attributes().value(QStringLiteral("version")) == "1.0") {
            readLead(lead);
        } else {
            xml.raiseError(i18n("It is not a sugarLead version 1.0 data."));
        }

    }
    return !xml.error();
}

QString SugarLeadIO::errorString() const
{
    return i18n("%1\nLine %2, column %3",
           xml.errorString(),
           xml.lineNumber(),
           xml.columnNumber());
}

void SugarLeadIO::readLead(SugarLead &lead)
{
    const SugarLead::AccessorHash accessors = SugarLead::accessorHash();
    Q_ASSERT(xml.isStartElement() && xml.name() == "sugarLead");

    while (xml.readNextStartElement()) {
        const QString key = xml.name().toString();
        const QString value = xml.readElementText();
        const SugarLead::AccessorHash::const_iterator accessIt = accessors.constFind(key);
        if (accessIt != accessors.constEnd()) {
            (lead.*(accessIt.value().setter))(value);
        } else {
            //TODO: add custom field support
            //lead.setCustomField(key, value);
        }
    }
}

bool SugarLeadIO::writeSugarLead(const SugarLead &lead, QIODevice *device)
{
    if (device == nullptr || !device->isWritable()) {
        return false;
    }

    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeDTD(QStringLiteral("<!DOCTYPE sugarLead>"));
    writer.writeStartElement(QStringLiteral("sugarLead"));
    writer.writeAttribute(QStringLiteral("version"), QStringLiteral("1.0"));

    const SugarLead::AccessorHash accessors = SugarLead::accessorHash();
    SugarLead::AccessorHash::const_iterator it = accessors.constBegin();
    const SugarLead::AccessorHash::const_iterator endIt = accessors.constEnd();
    for (; it != endIt; ++it) {
        const SugarLead::valueGetter getter = (*it).getter;
        writer.writeTextElement(it.key(), (lead.*getter)());
    }

/* TODO: add custom field support
    // plus custom fields
    QMap<QString, QString> customFields = lead.customFields();
    QMap<QString, QString>::const_iterator cit = customFields.constBegin();
    const QMap<QString, QString>::const_iterator end = customFields.constEnd();
    for ( ; cit != end ; ++cit ) {
        writer.writeTextElement(cit.key(), cit.value());
    }
*/

    writer.writeEndDocument();

    return true;
}
