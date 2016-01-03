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

#include "sugaremailio.h"
#include "sugaremail.h"

#include <KLocalizedString>

#include <QDebug>
#include <QHash>
#include <QIODevice>
#include <QXmlStreamWriter>

SugarEmailIO::SugarEmailIO()
{
}

bool SugarEmailIO::readSugarEmail(QIODevice *device, SugarEmail &email)
{
    if (device == 0 || !device->isReadable()) {
        return false;
    }

    email = SugarEmail();
    xml.setDevice(device);
    if (xml.readNextStartElement()) {
        if (xml.name() == "sugarEmail"
                && xml.attributes().value("version") == "1.0") {
            readEmail(email);
        } else {
            xml.raiseError(i18n("It is not a sugarEmail version 1.0 data."));
        }

    }
    return !xml.error();
}

QString SugarEmailIO::errorString() const
{
    return i18n("%1\nLine %2, column %3")
           .arg(xml.errorString())
           .arg(xml.lineNumber())
           .arg(xml.columnNumber());
}

void SugarEmailIO::readEmail(SugarEmail &email)
{
    const SugarEmail::AccessorHash accessors = SugarEmail::accessorHash();
    Q_ASSERT(xml.isStartElement() && xml.name() == "sugarEmail");

    while (xml.readNextStartElement()) {

        const SugarEmail::AccessorHash::const_iterator accessIt = accessors.constFind(xml.name().toString());
        if (accessIt != accessors.constEnd()) {
            (email.*(accessIt.value().setter))(xml.readElementText());
        } else {
            qDebug() << "Unexpected XML field in email" << xml.name();
            xml.skipCurrentElement();
        }
    }
}

bool SugarEmailIO::writeSugarEmail(const SugarEmail &email, QIODevice *device)
{
    if (device == 0 || !device->isWritable()) {
        return false;
    }

    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeDTD("<!DOCTYPE sugarEmail>");
    writer.writeStartElement("sugarEmail");
    writer.writeAttribute("version", "1.0");

    const SugarEmail::AccessorHash accessors = SugarEmail::accessorHash();
    SugarEmail::AccessorHash::const_iterator it    = accessors.constBegin();
    SugarEmail::AccessorHash::const_iterator endIt = accessors.constEnd();
    for (; it != endIt; ++it) {
        const SugarEmail::valueGetter getter = (*it).getter;
        writer.writeTextElement(it.key(), (email.*getter)());
    }
    writer.writeEndDocument();

    return true;
}
