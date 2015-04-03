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
    return KGlobal::locale()->formatDateTime(dt, KLocale::ShortDate, true);
}

QString KDCRMUtils::formatTimestampLong(const QString &serverTimestamp)
{
    const QDateTime dt = dateTimeFromString(serverTimestamp).toLocalTime();
    return KGlobal::locale()->formatDateTime(dt, KLocale::LongDate, true);
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
