/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <KLocalizedString>

#include <QLocale>
#include <QDateTime>
#include <QDir>
#include <QIcon>

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
    const QDateTime dt = dateTimeFromString(serverTimestamp);
    return formatDateTime(dt);
}

QString KDCRMUtils::currentTimestamp()
{
    return QDateTime::currentDateTimeUtc().toString(TIMESTAMPFORMAT);
}

QDate KDCRMUtils::dateFromString(const QString &dateString)
{
    return QDate::fromString(dateString, QStringLiteral("yyyy-MM-dd"));
}

QString KDCRMUtils::dateToString(const QDate &date)
{
    return date.toString(QStringLiteral("yyyy-MM-dd"));
}

QString KDCRMUtils::dateTimeToString(const QDateTime &dateTime)
{
    return dateTime.toString(TIMESTAMPFORMAT);
}

QString KDCRMUtils::formatDate(const QDate &date)
{
    return QLocale().toString(date, QLocale::ShortFormat);
}

QString KDCRMUtils::formatDateTime(const QDateTime &dt)
{
    return QLocale().toString(dt.toLocalTime(), QLocale::ShortFormat);
}

// Sugar SOAP apparently uses double-escaping
// <value xsi:type="xsd:string">foo &lt;bar&gt;</value> loses the <bar>
// and we get &amp;gt; for '>', etc.
// And strangely enough, it uses &#039; instead of &apos;

QString KDCRMUtils::encodeXML(const QString &str)
{
    QString encoded = str;
    encoded.replace('&', QLatin1String("&amp;"));
    encoded.replace('<', QLatin1String("&lt;"));
    encoded.replace('>', QLatin1String("&gt;"));
    encoded.replace('\'', QLatin1String("&#039;"));
    encoded.replace('"', QLatin1String("&quot;"));
    return encoded;
}

QString KDCRMUtils::decodeXML(const QString &str)
{
    QString decoded = str;
    decoded.replace(QLatin1String("&quot;"), QChar('"'));
    decoded.replace(QLatin1String("&#039;"), QChar('\''));
    decoded.replace(QLatin1String("&gt;"), QChar('>'));
    decoded.replace(QLatin1String("&lt;"), QChar('<'));
    decoded.replace(QLatin1String("&amp;"), QChar('&'));
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

QString KDCRMUtils::canonicalCountryName(const QString &input)
{
    const QString c = input.toLower();
    if (c.contains(QLatin1String("netherlands"))) {
        return QStringLiteral("The Netherlands");
    } else if (c == QLatin1String("united kingdom") || c == QLatin1String("great britain") || c == QLatin1String("u.k.")) {
        return QStringLiteral("UK");
    } else if (c == QLatin1String("u.s.a") || c == QLatin1String("us") || c == QLatin1String("united states")) {
        return QStringLiteral("USA");
    } else if (c == QLatin1String("Czechoslovakia")) {
        return QStringLiteral("Czech Republic");
    }
    return input;
}

void KDCRMUtils::setupIconTheme()
{
#if defined(Q_OS_MAC)
    if (qEnvironmentVariableIsSet("XDG_DATA_DIRS")) {
        const QStringList dirs = qEnvironmentVariable("XDG_DATA_DIRS").split(QDir::listSeparator());
        QStringList themes(QIcon::themeSearchPaths());
        QStringList fallbacks(QIcon::fallbackSearchPaths());

        for (const QString &dir: dirs) {
            themes << QString::fromLatin1("%1/icons").arg(dir);
            fallbacks << QString::fromLatin1("%1/icons").arg(dir);
        }

        QIcon::setThemeSearchPaths(themes);
        QIcon::setFallbackSearchPaths(fallbacks);

        QIcon::setThemeName("breeze");
        QIcon::setFallbackThemeName("hicolor");
    }
#endif
}
