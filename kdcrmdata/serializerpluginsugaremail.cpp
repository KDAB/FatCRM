/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "serializerpluginsugaremail.h"

#include "sugaremail.h"
#include "sugaremailio.h"

#include <Akonadi/Item>

#include <QtCore/qplugin.h>

using namespace Akonadi;

bool SerializerPluginSugarEmail::deserialize(Item &item, const QByteArray &label, QIODevice &data, int version)
{
    Q_UNUSED(version);

    if (label != Item::FullPayload) {
        return false;
    }

    SugarEmail sugarEmail;
    SugarEmailIO io;
    if (!io.readSugarEmail(&data, sugarEmail)) {
        return false;
    }

    item.setPayload<SugarEmail>(sugarEmail);

    return true;
}

void SerializerPluginSugarEmail::serialize(const Item &item, const QByteArray &label, QIODevice &data, int &version)
{
    Q_UNUSED(version);

    if (label != Item::FullPayload || !item.hasPayload<SugarEmail>()) {
        return;
    }

    const SugarEmail sugarEmail = item.payload<SugarEmail>();
    SugarEmailIO io;
    io.writeSugarEmail(sugarEmail, &data);
}

QString SerializerPluginSugarEmail::extractGid(const Item &item) const
{
    if (item.hasPayload<SugarEmail>()) {
        const SugarEmail sugarEmail = item.payload<SugarEmail>();
        return sugarEmail.id();
    }
    return QString();
}

