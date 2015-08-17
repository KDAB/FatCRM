/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "referenceddata.h"

#include <KDebug>

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
    inline const_iterator constBinaryFind(const QString& key) const {
        return qBinaryFind(constBegin(), constEnd(), KeyValue(key, QString()));
    }
};

class ReferencedData::Private
{
public:
    explicit Private(ReferencedDataType type)
        : mType(type)
    {
    }

public:
    KeyValueVector mVector;
    const ReferencedDataType mType;
};


class ReferenceDataMap
{
public:
    ~ReferenceDataMap() { qDeleteAll(map); }
    QMap<ReferencedDataType, ReferencedData *> map;
};

Q_GLOBAL_STATIC(ReferenceDataMap, s_instances)

ReferencedData *ReferencedData::instance(ReferencedDataType type)
{
    ReferencedData* instance = s_instances()->map.value(type);
    if (!instance) {
        instance = new ReferencedData(type);
        s_instances()->map.insert(type, instance);
    }
    return instance;
}

void ReferencedData::clearAll()
{
    foreach(ReferencedData *data, s_instances()->map) {
        data->clear();
    }
}

ReferencedData::~ReferencedData()
{
    delete d;
}

void ReferencedData::clear()
{
    if (!d->mVector.isEmpty()) {
        d->mVector.clear();
        emit cleared();
    }
}

void ReferencedData::setReferencedData(const QString &id, const QString &data)
{
    if (id.isEmpty()) {
        return;
    }

    setReferencedDataInternal(id, data, true);
}

void ReferencedData::setReferencedDataInternal(const QString &id, const QString &data, bool emitChanges)
{
    KeyValueVector::iterator findIt = d->mVector.binaryFind(id);
    if (findIt != d->mVector.end()) {
        if (data != findIt->value) {
            findIt->value = data;
            if (emitChanges) {
                emit dataChanged(findIt - d->mVector.begin());
            }
        }
    } else {
        findIt = qLowerBound(d->mVector.begin(), d->mVector.end(), KeyValue(id), KeyValue::lessThan);
        const int row = findIt - d->mVector.begin();
        if (emitChanges) {
            emit rowsAboutToBeInserted(row, row);
        }
        d->mVector.insert(findIt, KeyValue(id, data));
        if (emitChanges) {
            emit rowsInserted();
        }
    }
}

void ReferencedData::addMap(const QMap<QString, QString> &idDataMap, bool emitChanges)
{
    QMap<QString, QString>::const_iterator it = idDataMap.constBegin();
    const QMap<QString, QString>::const_iterator end = idDataMap.constEnd();
    if (d->mVector.isEmpty()) {
        d->mVector.reserve(idDataMap.count());
        // The vector is currently empty -> fast path
        // The map is already sorted, we can just copy right away
        // and emit the signals only once, which is the whole point of this method.
        if (emitChanges) {
            emit rowsAboutToBeInserted(0, idDataMap.count() - 1);
        }
        for ( ; it != end ; ++it) {
            d->mVector.append(KeyValue(it.key(), it.value()));
        }
        if (emitChanges) {
            emit rowsInserted();
        }
    } else {
        // Append to existing data -> slower code path
        for ( ; it != end ; ++it) {
            setReferencedDataInternal(it.key(), it.value(), emitChanges);
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

void ReferencedData::removeReferencedData(const QString &id, bool emitChanges)
{
    KeyValueVector::iterator findIt = d->mVector.binaryFind(id);
    if (findIt != d->mVector.end()) {
        const int row = findIt - d->mVector.begin();
        if (emitChanges) {
            emit rowsAboutToBeRemoved(row, row);
        }
        d->mVector.remove(row);
        if (emitChanges) {
            emit rowsRemoved();
        }
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

ReferencedDataType ReferencedData::dataType() const
{
    return d->mType;
}

void ReferencedData::emitInitialLoadingDoneForAll()
{
    foreach(ReferencedData *data, s_instances()->map) {
        if (data->dataType() != AccountCountryRef) { // that one was already done earlier
            emit data->initialLoadingDone();
        }
    }
}

void ReferencedData::emitInitialLoadingDone()
{
    emit initialLoadingDone();
}

ReferencedData::ReferencedData(ReferencedDataType type, QObject *parent)
    : QObject(parent), d(new Private(type))
{
}

#include "referenceddata.moc"
