#include "listentriesscope.h"

ListEntriesScope::ListEntriesScope()
    : mOffset(0),
      mGetDeleted(false)
{
}

ListEntriesScope::ListEntriesScope(const QString &timestamp)
    : mOffset(0),
      mUpdateTimestamp(timestamp),
      mGetDeleted(false)
{
}

bool ListEntriesScope::isUpdateScope() const
{
    return !mUpdateTimestamp.isEmpty();
}

void ListEntriesScope::setOffset(int offset)
{
    mOffset = offset;
}

int ListEntriesScope::offset() const
{
    return mOffset;
}

void ListEntriesScope::fetchDeleted()
{
    mGetDeleted = true;
    mOffset = 0;
}

int ListEntriesScope::deleted() const
{
    return mGetDeleted ? 1 : 0;
}

QString ListEntriesScope::query(const QString &filter, const QString &moduleName) const
{
    QString queryStr = filter;

    if (!filter.isEmpty()) {
        queryStr = filter;
    }

    if (mUpdateTimestamp.isEmpty()) {
        return queryStr;
    }

    if (!queryStr.isEmpty())
        queryStr += QLatin1String(" AND ");

    return queryStr + moduleName + QLatin1String(".date_modified >= '") + mUpdateTimestamp + QLatin1String("'");
}
