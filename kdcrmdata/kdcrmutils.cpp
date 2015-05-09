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
    return decoded;
}
