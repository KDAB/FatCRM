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

#include "sugaropportunityio.h"
#include "sugaropportunity.h"
#include "kdcrmutils.h"
#include "kdcrmfields.h"

#include <KLocalizedString>

#include <QDate>
#include <QDebug>
#include <QHash>
#include <QIODevice>
#include <QXmlStreamWriter>

SugarOpportunityIO::SugarOpportunityIO()
{
}

bool SugarOpportunityIO::readSugarOpportunity(QIODevice *device, SugarOpportunity &opportunity)
{
    if (device == 0 || !device->isReadable()) {
        return false;
    }

    opportunity = SugarOpportunity();
    xml.setDevice(device);
    if (xml.readNextStartElement()) {
        if (xml.name() == "sugarOpportunity"
                && xml.attributes().value("version") == "1.0") {
            readOpportunity(opportunity);
        } else {
            xml.raiseError(i18n("It is not a sugarOpportunity version 1.0 data."));
        }

    }
    return !xml.error();
}

QString SugarOpportunityIO::errorString() const
{
    return i18n("%1\nLine %2, column %3")
           .arg(xml.errorString())
           .arg(xml.lineNumber())
           .arg(xml.columnNumber());
}

void SugarOpportunityIO::readOpportunity(SugarOpportunity &opportunity)
{
    const SugarOpportunity::AccessorHash accessors = SugarOpportunity::accessorHash();
    Q_ASSERT(xml.isStartElement() && xml.name() == "sugarOpportunity");

    while (xml.readNextStartElement()) {

        const SugarOpportunity::AccessorHash::const_iterator accessIt = accessors.constFind(xml.name().toString());
        if (accessIt != accessors.constEnd()) {
            (opportunity.*(accessIt.value().setter))(xml.readElementText());
        } else {
            // compat code, fixing previous mistake
            if (xml.name() == "nextCallDate") {
                opportunity.setNextCallDateRaw(xml.readElementText());
            } else {
                qDebug() << "Unexpected XML field in opportunity" << xml.name();
                xml.skipCurrentElement();
            }
        }
    }
}

bool SugarOpportunityIO::writeSugarOpportunity(const SugarOpportunity &opportunity, QIODevice *device)
{
    if (device == 0 || !device->isWritable()) {
        return false;
    }

    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeDTD("<!DOCTYPE sugarOpportunity>");
    writer.writeStartElement("sugarOpportunity");
    writer.writeAttribute("version", "1.0");

    const SugarOpportunity::AccessorHash accessors = SugarOpportunity::accessorHash();
    SugarOpportunity::AccessorHash::const_iterator it    = accessors.constBegin();
    SugarOpportunity::AccessorHash::const_iterator endIt = accessors.constEnd();
    for (; it != endIt; ++it) {
        const SugarOpportunity::valueGetter getter = (*it).getter;
        writer.writeTextElement(it.key(), (opportunity.*getter)());
    }

    writer.writeEndDocument();

    return true;
}
