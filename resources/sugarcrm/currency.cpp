/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>

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

#include "currency.h"
#include "wsdl_sugar41.h"

#include <QDebug>

Currency::Currency(const KDSoapGenerated::TNS__Entry_value &entry)
{
    const auto values = entry.name_value_list().items();
    for (const KDSoapGenerated::TNS__Name_value &nameValue : values) {
        const QString name = nameValue.name();
        const QString value = nameValue.value();
        if (name == "id") {
            mId = value;
        } else if (name == "symbol") {
            mSymbol = value;
            // In a combobox USD would be OK, but in the Amount column it's just ugly
            // We don't have other kinds of dollars anyway, so let's use $
            if (mSymbol == "USD")
                mSymbol = "$";
        } else if (name == "iso4217") {
            mIso4217 = value;
        }
    }
}

QDebug operator<<(QDebug dbg, const Currency &currency)
{
    return dbg << currency.id() << currency.symbol() << currency.iso4217();
}
