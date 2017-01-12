/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sugaraccountio.h"
#include "sugaraccount.h"

#include <KLocalizedString>
#include <QDebug>
#include <QHash>
#include <QMap>
#include <QIODevice>
#include <QXmlStreamWriter>

SugarAccountIO::SugarAccountIO()
{
}

bool SugarAccountIO::readSugarAccount(QIODevice *device, SugarAccount &account)
{
    if (device == nullptr || !device->isReadable()) {
        return false;
    }

    account = SugarAccount();
    xml.setDevice(device);
    if (xml.readNextStartElement()) {
        if (xml.name() == "sugarAccount"
                && xml.attributes().value(QStringLiteral("version")) == "1.0") {
            readAccount(account);
        } else {
            xml.raiseError(i18n("It is not a sugarAccount version 1.0 data."));
        }

    }
    return !xml.error();
}

QString SugarAccountIO::errorString() const
{
    return i18n("%1\nLine %2, column %3",
           xml.errorString(),
           xml.lineNumber(),
           xml.columnNumber());
}

void SugarAccountIO::readAccount(SugarAccount &account)
{
    const SugarAccount::AccessorHash accessors = SugarAccount::accessorHash();
    Q_ASSERT(xml.isStartElement() && xml.name() == "sugarAccount");

    while (xml.readNextStartElement()) {
        const QString key = xml.name().toString();
        const QString value = xml.readElementText();
        const SugarAccount::AccessorHash::const_iterator accessIt = accessors.constFind(key);
        if (accessIt != accessors.constEnd()) {
            (account.*(accessIt.value().setter))(value);
        } else {
            account.setCustomField(key, value);
        }
    }
}

bool SugarAccountIO::writeSugarAccount(const SugarAccount &account, QIODevice *device)
{
    if (device == nullptr || !device->isWritable()) {
        return false;
    }

    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeDTD(QStringLiteral("<!DOCTYPE sugarAccount>"));
    writer.writeStartElement(QStringLiteral("sugarAccount"));
    writer.writeAttribute(QStringLiteral("version"), QStringLiteral("1.0"));

    const SugarAccount::AccessorHash accessors = SugarAccount::accessorHash();
    SugarAccount::AccessorHash::const_iterator it    = accessors.constBegin();
    SugarAccount::AccessorHash::const_iterator endIt = accessors.constEnd();
    for (; it != endIt; ++it) {
        const SugarAccount::valueGetter getter = (*it).getter;
        writer.writeTextElement(it.key(), (account.*getter)());
    }

    // plus custom fields
    QMap<QString, QString> customFields = account.customFields();
    QMap<QString, QString>::const_iterator cit = customFields.constBegin();
    const QMap<QString, QString>::const_iterator end = customFields.constEnd();
    for ( ; cit != end ; ++cit ) {
        writer.writeTextElement(cit.key(), cit.value());
    }

    writer.writeEndDocument();

    return true;
}
