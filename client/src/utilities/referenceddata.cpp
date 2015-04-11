#include "referenceddata.h"

#include <QVector>
#include <QMap>
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
    KeyValueVector mVector;
    ReferencedDataType mType;
};

ReferencedData *ReferencedData::instance(ReferencedDataType type)
{
    static QMap<ReferencedDataType, ReferencedData *> s_instances;
    ReferencedData* instance = s_instances.value(type);
    if (!instance) {
        instance = new ReferencedData(type);
        s_instances.insert(type, instance);
    }
    return instance;
}

ReferencedData::~ReferencedData()
{
    delete d;
}

#if 0
void ReferencedData::clear()
{
    if (!mVector.isEmpty()) {
        mVector.clear();
        emit cleared(type);
    }
}
#endif

void ReferencedData::setReferencedData(const QString &id, const QString &data)
{
    if (id.isEmpty()) {
        return;
    }

    KeyValueVector::iterator findIt = d->mVector.binaryFind(id);
    if (findIt != d->mVector.end()) {
        if (data != findIt->value) {
            findIt->value = data;
            emit dataChanged(findIt - d->mVector.begin());
        }
    } else {
        findIt = qLowerBound(d->mVector.begin(), d->mVector.end(), KeyValue(id), KeyValue::lessThan);
        d->mVector.insert(findIt, KeyValue(id, data));
        emit dataChanged(findIt - d->mVector.begin()); // ## TODO rowsAboutToBeInserted, rowsInserted
    }
}

void ReferencedData::removeReferencedData(const QString &id)
{
    KeyValueVector::iterator findIt = d->mVector.binaryFind(id);
    if (findIt != d->mVector.end()) {
        const int row = findIt - d->mVector.begin();
        d->mVector.remove(row);
        emit dataChanged(row); // ### TODO proper signals
    }
}

QPair<QString, QString> ReferencedData::data(int row) const
{
    if (row >= 0 && row < d->mVector.count()) {
        const KeyValue &it = d->mVector.at(row);
        return qMakePair(it.key, it.value);
    }
    return qMakePair(QString(), QString());
}

int ReferencedData::count() const
{
    return d->mVector.count();
}

ReferencedData::ReferencedData(ReferencedDataType type, QObject *parent)
    : QObject(parent), d(new Private(this))
{
    d->mType = type;
}

#include "referenceddata.moc"
