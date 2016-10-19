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

#include "sugarcampaignio.h"
#include "sugarcampaign.h"
#include "kdcrmfields.h"

#include <KLocalizedString>
#include <QHash>
#include <QIODevice>
#include <QXmlStreamWriter>

SugarCampaignIO::SugarCampaignIO()
{
}

bool SugarCampaignIO::readSugarCampaign(QIODevice *device, SugarCampaign &campaign)
{
    if (device == nullptr || !device->isReadable()) {
        return false;
    }

    campaign = SugarCampaign();
    xml.setDevice(device);
    if (xml.readNextStartElement()) {
        if (xml.name() == "sugarCampaign"
                && xml.attributes().value("version") == "1.0") {
            readCampaign(campaign);
        } else {
            xml.raiseError(i18n("It is not a sugarCampaign version 1.0 data."));
        }

    }
    return !xml.error();
}

QString SugarCampaignIO::errorString() const
{
    return i18n("%1\nLine %2, column %3",
           xml.errorString(),
           xml.lineNumber(),
           xml.columnNumber());
}

void SugarCampaignIO::readCampaign(SugarCampaign &campaign)
{
    const SugarCampaign::AccessorHash accessors = SugarCampaign::accessorHash();
    Q_ASSERT(xml.isStartElement() && xml.name() == "sugarCampaign");

    while (xml.readNextStartElement()) {
        const QString key = xml.name().toString();
        const QString value = xml.readElementText();
        const SugarCampaign::AccessorHash::const_iterator accessIt = accessors.constFind(key);
        if (accessIt != accessors.constEnd()) {
            (campaign.*(accessIt.value().setter))(value);
        } else {
            //TODO: add custom field support
            //campaign.setCustomField(key, value);
        }
    }
}

bool SugarCampaignIO::writeSugarCampaign(const SugarCampaign &campaign, QIODevice *device)
{
    if (device == nullptr || !device->isWritable()) {
        return false;
    }

    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeDTD("<!DOCTYPE sugarCampaign>");
    writer.writeStartElement("sugarCampaign");
    writer.writeAttribute("version", "1.0");

    const SugarCampaign::AccessorHash accessors = SugarCampaign::accessorHash();
    SugarCampaign::AccessorHash::const_iterator it    = accessors.constBegin();
    const SugarCampaign::AccessorHash::const_iterator endIt = accessors.constEnd();
    for (; it != endIt; ++it) {
        const SugarCampaign::valueGetter getter = (*it).getter;
        writer.writeTextElement(it.key(), (campaign.*getter)());
    }

/* TODO: add custom field support
    // plus custom fields
    QMap<QString, QString> customFields = campaign.customFields();
    QMap<QString, QString>::const_iterator cit = customFields.constBegin();
    const QMap<QString, QString>::const_iterator end = customFields.constEnd();
    for ( ; cit != end ; ++cit ) {
        writer.writeTextElement(cit.key(), cit.value());
    }
*/

    writer.writeEndDocument();

    return true;
}
