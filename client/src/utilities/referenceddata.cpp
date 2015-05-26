#include "referenceddata.h"

#include <QVector>
#include <QMap>
#include <QPair>
#include <kdebug.h>

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
    inline const_iterator constBinaryFind(const QString& key) const {
        return qBinaryFind(constBegin(), constEnd(), KeyValue(key, QString()));
    }
};

class ReferencedData::Private
{
public:
    explicit Private()
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
        const int row = findIt - d->mVector.begin();
        emit rowsAboutToBeInserted(row, row);
        d->mVector.insert(findIt, KeyValue(id, data));
        emit rowsInserted();
    }
}

void ReferencedData::addMap(const QMap<QString, QString> &idDataMap)
{
    QMap<QString, QString>::const_iterator it = idDataMap.constBegin();
    const QMap<QString, QString>::const_iterator end = idDataMap.constEnd();
    if (d->mVector.isEmpty()) {
        d->mVector.reserve(idDataMap.count());
        // The vector is currently empty -> fast path
        // The map is already sorted, we can just copy right away
        // and emit the signals only once, which is the whole point of this method.
        emit rowsAboutToBeInserted(0, idDataMap.count() - 1);
        for ( ; it != end ; ++it) {
            d->mVector.append(KeyValue(it.key(), it.value()));
        }
        emit rowsInserted();
    } else {
        // Append to existing data -> slower code path
        for ( ; it != end ; ++it) {
            setReferencedData(it.key(), it.value());
        }
    }
}

QString ReferencedData::referencedData(const QString &id) const
{
    KeyValueVector::const_iterator findIt = d->mVector.constBinaryFind(id);
    if (findIt != d->mVector.constEnd()) {
        return findIt->value;
    }
    return QString();
}

void ReferencedData::removeReferencedData(const QString &id)
{
    KeyValueVector::iterator findIt = d->mVector.binaryFind(id);
    if (findIt != d->mVector.end()) {
        const int row = findIt - d->mVector.begin();
        emit rowsAboutToBeRemoved(row, row);
        d->mVector.remove(row);
        emit rowsRemoved();
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
    : QObject(parent), d(new Private())
{
    d->mType = type;
}

#include "referenceddata.moc"
