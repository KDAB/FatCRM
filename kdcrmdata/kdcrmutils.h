#ifndef KDCRMUTILS_H
#define KDCRMUTILS_H

#include "kdcrmdata_export.h"

class QString;

namespace KDCRMUtils
{
KDCRMDATA_EXPORT QString formatTimestamp(const QString &serverTimestamp);
KDCRMDATA_EXPORT QString formatTimestampLong(const QString &serverTimestamp);
KDCRMDATA_EXPORT QString currentTimestamp();
}

#endif
