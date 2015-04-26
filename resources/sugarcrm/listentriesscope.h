#ifndef LISTENTRIESSCOPE_H
#define LISTENTRIESSCOPE_H

#include <QString>

class ListEntriesScope
{
public:
    ListEntriesScope();
    ListEntriesScope(const QString &timestamp);

    bool isUpdateScope() const;

    void setOffset(int offset);

    int offset() const;

    void fetchDeleted();

    int deleted() const;

    QString query(const QString &module) const;

private:
    int mOffset;
    QString mUpdateTimestamp;
    bool mGetDeleted;
};

#endif // LISTENTRIESSCOPE_H
