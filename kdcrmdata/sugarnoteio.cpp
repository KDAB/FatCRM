#include "sugarnoteio.h"
#include "sugarnote.h"

#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QDebug>

SugarNoteIO::SugarNoteIO()
{
}

bool SugarNoteIO::readSugarNote(QIODevice *device, SugarNote &note)
{
    if (device == 0 || !device->isReadable()) {
        return false;
    }

    note = SugarNote();
    xml.setDevice(device);
    if (xml.readNextStartElement()) {
        if (xml.name() == "sugarNote"
                && xml.attributes().value("version") == "1.0") {
            readNote(note);
        } else {
            xml.raiseError(QObject::tr("It is not a sugarNote version 1.0 data."));
        }

    }
    return !xml.error();
}

QString SugarNoteIO::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
           .arg(xml.errorString())
           .arg(xml.lineNumber())
           .arg(xml.columnNumber());
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
    if (device == 0 || !device->isWritable()) {
        return false;
    }

    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeDTD("<!DOCTYPE sugarNote>");
    writer.writeStartElement("sugarNote");
    writer.writeAttribute("version", "1.0");

    const SugarNote::AccessorHash accessors = SugarNote::accessorHash();
    SugarNote::AccessorHash::const_iterator it    = accessors.constBegin();
    SugarNote::AccessorHash::const_iterator endIt = accessors.constEnd();
    for (; it != endIt; ++it) {
        const SugarNote::valueGetter getter = (*it).getter;
        writer.writeTextElement(it.key(), (note.*getter)());
    }
    writer.writeEndDocument();

    return true;
}

