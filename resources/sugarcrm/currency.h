/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2019-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef CURRENCY_H
#define CURRENCY_H

#include <QString>
#include <vector>

namespace KDSoapGenerated {
    class TNS__Entry_value;
}

class Currency
{
public:
    explicit Currency(const KDSoapGenerated::TNS__Entry_value &entry);

    QString id() const { return mId; }
    QString symbol() const { return mSymbol; }
    QString iso4217() const { return mIso4217; }

private:
    QString mId;
    QString mSymbol;
    QString mIso4217;
};

QDebug operator<<(QDebug dbg, const Currency &currency);

#endif
