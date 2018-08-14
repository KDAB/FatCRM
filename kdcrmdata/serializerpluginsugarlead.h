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

#ifndef SERIALIZERPLUGINSUGARLEAD_H
#define SERIALIZERPLUGINSUGARLEAD_H

#include <AkonadiCore/ItemSerializerPlugin>
#include <AkonadiCore/GidExtractorInterface>

#include <QObject>

namespace Akonadi
{

class SerializerPluginSugarLead : public QObject, public ItemSerializerPlugin, public GidExtractorInterface
{
    Q_OBJECT
    Q_INTERFACES(Akonadi::ItemSerializerPlugin)
    Q_PLUGIN_METADATA(IID "org.kde.akonadi.SerializerPluginSugarLead")
public:
    bool deserialize(Item &item, const QByteArray &label, QIODevice &data, int version) override;
    void serialize(const Item &item, const QByteArray &label, QIODevice &data, int &version) override;
    QString extractGid(const Item &item) const override;
};

}

#endif /* SERIALIZERPLUGINSUGARLEAD_H */
