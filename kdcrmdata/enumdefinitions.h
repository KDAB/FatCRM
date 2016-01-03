/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef ENUMDEFINITIONS_H
#define ENUMDEFINITIONS_H

#include "kdcrmdata_export.h"

#include <vector>

#include <QMap>
#include <QMetaType>
#include <QString>

/**
 * @brief The EnumDefinitions class lists enum definitions for one type of item.
 * E.g. for Contacts, enum definitions exist for
 * salutation, lead_source, and portal_user_type
 *
 * Each enum definition contains a list of values (ID and display string).
 * E.g. one of the values in the 'lead source' definition is:
 *   ID="QtDevDays", DisplayString="Qt Developer Days"
 */
class KDCRMDATA_EXPORT EnumDefinitions
{
public:
    EnumDefinitions();

    struct KDCRMDATA_EXPORT Enum
    {
        Enum(const QString &name) : mEnumName(name) {}
        QString toString() const;
        static Enum fromString(const QString &str);

        QString mEnumName;
        typedef QMap<QString, QString> Map;
        Map mEnumValues; // ID, display string
    };

    void append(const Enum &e) { mDefinitions.push_back(e); }
    EnumDefinitions &operator<<(const Enum &e) { mDefinitions.push_back(e); return *this; }

    int count() const { return mDefinitions.size(); }
    const Enum & at(int i) const { return mDefinitions.at(i); }
    int indexOf(const QString &enumName) const;

    // serialization
    QString toString() const;
    static EnumDefinitions fromString(const QString &str);

private:
    std::vector<Enum> mDefinitions;
};

Q_DECLARE_METATYPE(EnumDefinitions)

#endif // ENUMDEFINITIONS_H
