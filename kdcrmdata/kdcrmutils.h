#ifndef KDCRMUTILS_H
#define KDCRMUTILS_H

#include "kdcrmdata_export.h"
#include <QDate>

class QString;

namespace KDCRMUtils
{
KDCRMDATA_EXPORT QDateTime dateTimeFromString(const QString &serverTimestamp);
KDCRMDATA_EXPORT QString formatTimestamp(const QString &serverTimestamp);
KDCRMDATA_EXPORT QString formatTimestampLong(const QString &serverTimestamp);
KDCRMDATA_EXPORT QString currentTimestamp();

KDCRMDATA_EXPORT QDate dateFromString(const QString &dateString); // using yyyy-MM-dd
KDCRMDATA_EXPORT QString dateToString(const QDate &date); // using yyyy-MM-dd
KDCRMDATA_EXPORT QString formatDate(const QDate &date); // locale

KDCRMDATA_EXPORT QString encodeXML(const QString &str);
KDCRMDATA_EXPORT QString decodeXML(const QString &str);
}

#endif
