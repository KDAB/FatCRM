#include "referenceddata.h"

#include <QVector>
#include <QPair>

struct KeyValue
{
    explicit KeyValue(const QString &k = QString(), const QString &v = QString())
        : key(k), value(v) {}
    QString key;
    QString value;
    bool operator<(const KeyValue &other) const { return key < other.key; }
    static bool lessThan(const KeyValue &first, const KeyValue &other) { return first.key < other.key; }
};


class KeyValueVector : public QVector<KeyValue>
{
public:
    inline iterator binaryFind(const QString& key) {
        return qBinaryFind(begin(), end(), KeyValue(key, QString()));
    }
};

class ReferencedData::Private
{
    ReferencedData *const q;

public:
    explicit Private(ReferencedData *parent) : q(parent)
    {
    }

public:
    static ReferencedData *sInstance;

    KeyValueVector mAccounts;
    KeyValueVector mAssignedTo;
    KeyValueVector mCampaigns;
    KeyValueVector mReportsTo;

public:
    KeyValueVector &vectorForType(ReferencedDataType type);
};

KeyValueVector &ReferencedData::Private::vectorForType(ReferencedDataType type)
{
    switch (type) {
    case AccountRef: return mAccounts;
    case AssignedToRef: return mAssignedTo;
    case CampaignRef: return mCampaigns;
    case ReportsToRef: return mReportsTo;
    }

    Q_ASSERT(false);
    static KeyValueVector dummy;
    return dummy;
}

ReferencedData *ReferencedData::Private::sInstance = 0;

ReferencedData *ReferencedData::instance()
{
    if (Private::sInstance == 0) {
        Private::sInstance = new ReferencedData();
    }

    return Private::sInstance;
}

ReferencedData::~ReferencedData()
{
    delete d;
}

void ReferencedData::clear(ReferencedDataType type)
{
    KeyValueVector &map = d->vectorForType(type);
    if (!map.isEmpty()) {
        map.clear();
        emit cleared(type);
    }
}

void ReferencedData::clearAll()
{
    clear(AccountRef);
    clear(AssignedToRef);
    clear(CampaignRef);
    clear(ReportsToRef);
}

void ReferencedData::setReferencedData(ReferencedDataType type, const QString &id, const QString &data)
{
    if (id.isEmpty()) {
        return;
    }

    KeyValueVector &vector = d->vectorForType(type);

    KeyValueVector::iterator findIt = vector.binaryFind(id);
    if (findIt != vector.end()) {
        if (data != findIt->value) {
            findIt->value = data;
            // TODO emit a proper row number here
            emit dataChanged(type);
        }
    } else {
        findIt = qLowerBound(vector.begin(), vector.end(), KeyValue(id), KeyValue::lessThan);
        vector.insert(findIt, KeyValue(id, data));
        emit dataChanged(type);
    }
}

void ReferencedData::removeReferencedData(ReferencedDataType type, const QString &id)
{
    KeyValueVector &vector = d->vectorForType(type);
    KeyValueVector::iterator findIt = vector.binaryFind(id);
    if (findIt != vector.end()) {
        const int row = findIt - vector.begin();
        vector.remove(row);
        emit dataChanged(type);
    }
}

QPair<QString, QString> ReferencedData::data(ReferencedDataType type, int row) const
{
    KeyValueVector &vector = d->vectorForType(type);
    KeyValueVector::const_iterator it = vector.constBegin();
    it += row;
    if (it != vector.constEnd())
        return qMakePair(it->key, it->value);
    return qMakePair(QString(), QString());
}

int ReferencedData::count(ReferencedDataType type) const
{
    return d->vectorForType(type).count();
}

ReferencedData::ReferencedData(QObject *parent)
    : QObject(parent), d(new Private(this))
{
}

#include "referenceddata.moc"
