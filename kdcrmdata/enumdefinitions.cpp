/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "enumdefinitions.h"

#include <QStringList>

QString EnumDefinitions::Enum::toString() const
{
    QString ret = mEnumName;
    ret += '|';
    for (Vector::const_iterator it = mEnumValues.constBegin(); it != mEnumValues.constEnd(); ++it) {
        Q_ASSERT(!it->key.contains(':'));
        Q_ASSERT(!it->key.contains('|'));
        Q_ASSERT(!it->value.contains(':'));
        Q_ASSERT(!it->value.contains('|'));
        ret += it->key + ':' + it->value + '|';
    }
    return ret;
}

EnumDefinitions::Enum EnumDefinitions::Enum::fromString(const QString &str)
{
    const int nameSep = str.indexOf('|');
    Q_ASSERT(nameSep > -1);
    Enum ret(str.left(nameSep));
    int pos = nameSep + 1;
    while (pos < str.length()) {
        const int sep = str.indexOf(':', pos);
        Q_ASSERT(sep > -1);
        const int end = str.indexOf('|', sep + 1);
        Q_ASSERT(end > -1);
        KeyValue keyValue;
        keyValue.key = str.mid(pos, sep - pos);
        keyValue.value = str.mid(sep + 1, end - sep - 1);
        ret.mEnumValues.append(keyValue);
        pos = end + 1;
    }

    return ret;
}


EnumDefinitions::EnumDefinitions()
{
}

int EnumDefinitions::indexOf(const QString &enumName) const
{
    for (size_t i = 0; i < mDefinitions.size(); ++i) {
        if (mDefinitions.at(i).mEnumName == enumName) {
            return i;
        }
    }
    return -1;
}

// E.g. "lead_source|Key1:Value1|Key2:Value2|Key3:Value3|%another_enum|K:V"
QString EnumDefinitions::toString() const
{
    QString ret;
    for (size_t i = 0; i < mDefinitions.size(); ++i) {
        ret += mDefinitions.at(i).toString();
        if (i + 1 < mDefinitions.size()) {
            ret += '%';
        }
    }
    return ret;
}

EnumDefinitions EnumDefinitions::fromString(const QString &str)
{
    EnumDefinitions ret;
    // a bit slow but I'm lazy
    foreach (const QString &s, str.split('%', QString::SkipEmptyParts)) {
        ret.append(EnumDefinitions::Enum::fromString(s));
    }

    return ret;
}


QDebug operator<<(QDebug stream, const EnumDefinitions::Enum &oneEnum)
{
    stream << oneEnum.mEnumName << '[' << oneEnum.mEnumValues << ']';
    return stream;
}

QDebug operator<<(QDebug stream, const EnumDefinitions::KeyValue &keyValue)
{
    stream << keyValue.key << keyValue.value;
    return stream;
}
