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

#include "kdcrmutils.h"

#include <KGlobal>
#include <KLocale>

#include <QDateTime>

#define TIMESTAMPFORMAT QLatin1String( "yyyy-MM-dd hh:mm:ss" )

QDateTime KDCRMUtils::dateTimeFromString(const QString &serverTimestamp)
{
    QDateTime dt = QDateTime::fromString(serverTimestamp, TIMESTAMPFORMAT);
    dt.setTimeSpec(Qt::UTC);
    return dt;
}

void KDCRMUtils::incrementTimeStamp(QString &serverTimestamp)
{
    if (!serverTimestamp.isEmpty()) {
        QDateTime dt = QDateTime::fromString(serverTimestamp, TIMESTAMPFORMAT);
        dt = dt.addSecs(1);
        serverTimestamp = dateTimeToString(dt);
    }
}

void KDCRMUtils::decrementTimeStamp(QString &serverTimestamp)
{
    if (!serverTimestamp.isEmpty()) {
        QDateTime dt = QDateTime::fromString(serverTimestamp, TIMESTAMPFORMAT);
        dt = dt.addSecs(-1);
        serverTimestamp = dateTimeToString(dt);
    }
}

QString KDCRMUtils::formatTimestamp(const QString &serverTimestamp)
{
    const QDateTime dt = dateTimeFromString(serverTimestamp).toLocalTime();
    return formatDateTime(dt);
}

QString KDCRMUtils::currentTimestamp()
{
    return QDateTime::currentDateTime().toUTC().toString(TIMESTAMPFORMAT);
}

QDate KDCRMUtils::dateFromString(const QString &dateString)
{
    return QDate::fromString(dateString, QLatin1String("yyyy-MM-dd"));
}

QString KDCRMUtils::dateToString(const QDate &date)
{
    return date.toString(QLatin1String("yyyy-MM-dd"));
}

QString KDCRMUtils::dateTimeToString(const QDateTime &dateTime)
{
    return dateTime.toString(TIMESTAMPFORMAT);
}

QString KDCRMUtils::formatDate(const QDate &date)
{
    return KGlobal::locale()->formatDate(date, KLocale::ShortDate);
}

QString KDCRMUtils::formatDateTime(const QDateTime &dt)
{
    return KGlobal::locale()->formatDateTime(dt, KLocale::ShortDate, true);
}

// Sugar SOAP apparently uses double-escaping
// <value xsi:type="xsd:string">foo &lt;bar&gt;</value> loses the <bar>
// and we get &amp;gt; for '>', etc.
// And strangely enough, it uses &#039; instead of &apos;

QString KDCRMUtils::encodeXML(const QString &str)
{
    QString encoded = str;
    encoded.replace('&', "&amp;");
    encoded.replace('<', "&lt;");
    encoded.replace('>', "&gt;");
    encoded.replace('\'', "&#039;");
    encoded.replace('"', "&quot;");
    return encoded;
}

QString KDCRMUtils::decodeXML(const QString &str)
{
    QString decoded = str;
    decoded.replace("&quot;", QChar('"'));
    decoded.replace("&#039;", QChar('\''));
    decoded.replace("&gt;", QChar('>'));
    decoded.replace("&lt;", QChar('<'));
    decoded.replace("&amp;", QChar('&'));
    // While at it, remove trailing spaces, they can be confusing with e.g. country filtering.
    return decoded.trimmed();
}

QString KDCRMUtils::limitString(const QString &str, int wantedParagraphs)
{
    const QStringList paragraphs = str.split('\n');
    const int paragraphCount = paragraphs.count();

    if (paragraphCount > wantedParagraphs) {
        int collectedParagraphs = 0;
        QString output;

        for (int i = 0; collectedParagraphs < wantedParagraphs && i < paragraphCount; ++i) {
            if (!output.isEmpty()) {
                output.append('\n');
            }
            const QString row = paragraphs.at(i);

            if (!row.isEmpty()) {
                ++collectedParagraphs;
                output.append(row);
            }
        }
        return output;
    }
    return str;
}
