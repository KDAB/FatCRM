#include "referenceddata.h"

#include <QMap>

class ReferencedData::Private
{
    ReferencedData *const q;

public:
    explicit Private(ReferencedData *parent) : q(parent)
    {
    }

public:
    static ReferencedData *sInstance;

    QMap<QString, QString> mAccounts;
    QMap<QString, QString> mAssignedTo;
    QMap<QString, QString> mCampaigns;
    QMap<QString, QString> mReportsTo;

public:
    QMap<QString, QString> &mapForType(ReferencedDataType type);
};

QMap<QString, QString> &ReferencedData::Private::mapForType(ReferencedDataType type)
{
    switch (type) {
    case AccountRef: return mAccounts;
    case AssignedToRef: return mAssignedTo;
    case CampaignRef: return mCampaigns;
    case ReportsToRef: return mReportsTo;
    }

    Q_ASSERT(false);
    static QMap<QString, QString> dummy;
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
    QMap<QString, QString> &map = d->mapForType(type);
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

    QMap<QString, QString> &map = d->mapForType(type);

    QMap<QString, QString>::iterator findIt = map.find(id);
    if (findIt != map.end()) {
        if (data != findIt.value()) {
            *findIt = data;
            // TODO if we used a (sorted?) vector we could emit a proper row number here
            emit dataChanged(type);
        }
    } else {
        map.insert(id, data);
        emit dataChanged(type);
    }
}

void ReferencedData::removeReferencedData(ReferencedDataType type, const QString &id)
{
    QMap<QString, QString> &map = d->mapForType(type);

    if (map.remove(id) != 0) {
        emit dataChanged(type);
    }
}

QMap<QString, QString> ReferencedData::data(ReferencedDataType type) const
{
    return d->mapForType(type);
}

ReferencedData::ReferencedData(QObject *parent)
    : QObject(parent), d(new Private(this))
{
}

#include "referenceddata.moc"
