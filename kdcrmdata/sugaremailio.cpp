#include "sugaremailio.h"
#include "sugaremail.h"

#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QDebug>

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
            xml.raiseError(QObject::tr("It is not a sugarEmail version 1.0 data."));
        }

    }
    return !xml.error();
}

QString SugarEmailIO::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
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

