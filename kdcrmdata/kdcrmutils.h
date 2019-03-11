/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KDCRMUTILS_H
#define KDCRMUTILS_H

#include "kdcrmdata_export.h"
#include <QDateTime>

class QString;

namespace KDCRMUtils
{
// convert yyyy-MM-dd to locale. Use dateTimeFromString + formatDateTime instead.
KDCRMDATA_EXPORT QString formatTimestamp(const QString &serverTimestamp);

KDCRMDATA_EXPORT void incrementTimeStamp(QString &serverTimestamp);
KDCRMDATA_EXPORT void decrementTimeStamp(QString &serverTimestamp);

KDCRMDATA_EXPORT QString currentTimestamp();

KDCRMDATA_EXPORT QDate dateFromString(const QString &dateString); // using yyyy-MM-dd
KDCRMDATA_EXPORT QDateTime dateTimeFromString(const QString &dateTimeString); // using yyyy-MM-dd hh:mm:ss
KDCRMDATA_EXPORT QString dateToString(const QDate &date); // using yyyy-MM-dd
KDCRMDATA_EXPORT QString dateTimeToString(const QDateTime &dateTime); // using yyyy-MM-dd hh:mm:ss
KDCRMDATA_EXPORT QString formatDate(const QDate &date); // locale
KDCRMDATA_EXPORT QString formatDateTime(const QDateTime &dt); // locale

KDCRMDATA_EXPORT QString encodeXML(const QString &str);
KDCRMDATA_EXPORT QString decodeXML(const QString &str);

KDCRMDATA_EXPORT QString limitString(const QString &str, int wantedParagraphs);

KDCRMDATA_EXPORT QString canonicalCountryName(const QString &input);

KDCRMDATA_EXPORT void setupIconTheme();
}


#endif
