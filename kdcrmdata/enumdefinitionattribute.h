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

#ifndef ENUMDEFINITIONATTRIBUTE_H
#define ENUMDEFINITIONATTRIBUTE_H
#include "kdcrmdata_export.h"
#include <AkonadiCore/Attribute>
#include <QMap>
#include <QString>
#include "kdcrmdata_export.h"

/**
 * An attribute for letting the resource store the definition of enums, that FatCRM can use.
 */
class KDCRMDATA_EXPORT EnumDefinitionAttribute : public Akonadi::Attribute
{
public:
    EnumDefinitionAttribute();

    // I tried to make this generic (QString) and with a type enum
    // to reuse the attribute class, but AttributeFactory::registerAttribute expects
    // a default ctor and a constant type()....

    void setValue(const QString &value);
    QString value() const;

    QByteArray type() const override;
    Attribute *clone() const override;
    QByteArray serialized() const override;
    void deserialize(const QByteArray &data) override;

private:
    QString mValue;
};

#endif
