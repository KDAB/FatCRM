#include "kdcrmutils.h"

#include <KGlobal>
#include <KLocale>

#include <QDateTime>

#define TIMESTAMPFORMAT QLatin1String( "yyyy-MM-dd hh:mm:ss" )

static QDateTime fromString( const QString &serverTimestamp )
{
    QDateTime dt = QDateTime::fromString( serverTimestamp, TIMESTAMPFORMAT );
    dt.setTimeSpec( Qt::UTC );
    return dt;
}

QString KDCRMUtils::formatTimestamp( const QString &serverTimestamp )
{
    const QDateTime dt = fromString( serverTimestamp ).toLocalTime();
    return KGlobal::locale()->formatDateTime( dt, KLocale::ShortDate, true );
}

QString KDCRMUtils::formatTimestampLong( const QString &serverTimestamp )
{
    const QDateTime dt = fromString( serverTimestamp ).toLocalTime();
    return KGlobal::locale()->formatDateTime( dt, KLocale::LongDate, true );
}

QString KDCRMUtils::currentTimestamp()
{
    return QDateTime::currentDateTime().toUTC().toString( TIMESTAMPFORMAT );
}
