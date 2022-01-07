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

#include "enumdefinitionattribute.h"

#include <Akonadi/Collection>
#include <QByteArray>
#include <QString>

EnumDefinitionAttribute::EnumDefinitionAttribute()
{
}

void EnumDefinitionAttribute::setValue(const QString &value)
{
    mValue = value;
}

QString EnumDefinitionAttribute::value() const
{
    return mValue;
}

QByteArray EnumDefinitionAttribute::type() const
{
    return "CRM-enumdefinitions";
}

Akonadi::Attribute *EnumDefinitionAttribute::clone() const
{
    auto *attr = new EnumDefinitionAttribute;
    attr->setValue(mValue);
    return attr;
}

QByteArray EnumDefinitionAttribute::serialized() const
{
    const QByteArray result = mValue.toUtf8();
    return result;
}

void EnumDefinitionAttribute::deserialize(const QByteArray &data)
{
    mValue = QString::fromUtf8(data);
}
