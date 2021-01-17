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

#include "sugarnoteio.h"
#include "sugarnote.h"

#include <KLocalizedString>
#include <QHash>
#include <QIODevice>
#include <QXmlStreamWriter>

SugarNoteIO::SugarNoteIO()
{
}

bool SugarNoteIO::readSugarNote(QIODevice *device, SugarNote &note)
{
    if (device == nullptr || !device->isReadable()) {
        return false;
    }

    note = SugarNote();
    xml.setDevice(device);
    if (xml.readNextStartElement()) {
        if (xml.name() == "sugarNote"
                && xml.attributes().value(QStringLiteral("version")) == "1.0") {
            readNote(note);
        } else {
            xml.raiseError(i18n("It is not a sugarNote version 1.0 data."));
        }

    }
    return !xml.error();
}

QString SugarNoteIO::errorString() const
{
    return i18n("%1\nLine %2, column %3",
           xml.errorString(),
           xml.lineNumber(),
           xml.columnNumber());
}

void SugarNoteIO::readNote(SugarNote &note)
{
    const SugarNote::AccessorHash accessors = SugarNote::accessorHash();
    Q_ASSERT(xml.isStartElement() && xml.name() == "sugarNote");

    while (xml.readNextStartElement()) {

        const SugarNote::AccessorHash::const_iterator accessIt = accessors.constFind(xml.name().toString());
        if (accessIt != accessors.constEnd()) {
            (note.*(accessIt.value().setter))(xml.readElementText());
        } else {
            xml.skipCurrentElement();
        }
    }
}

bool SugarNoteIO::writeSugarNote(const SugarNote &note, QIODevice *device)
{
    if (device == nullptr || !device->isWritable()) {
        return false;
    }

    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeDTD(QStringLiteral("<!DOCTYPE sugarNote>"));
    writer.writeStartElement(QStringLiteral("sugarNote"));
    writer.writeAttribute(QStringLiteral("version"), QStringLiteral("1.0"));

    const SugarNote::AccessorHash accessors = SugarNote::accessorHash();
    for (auto it = accessors.constBegin(); it != accessors.constEnd(); ++it) {
        const SugarNote::valueGetter getter = (*it).getter;
        writer.writeTextElement(it.key(), (note.*getter)());
    }
    writer.writeEndDocument();

    return true;
}
