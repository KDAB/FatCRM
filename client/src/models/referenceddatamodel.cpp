#include "referenceddatamodel.h"

#include "referenceddata.h"

class ReferencedDataModel::Private
{
    ReferencedDataModel *const q;

public:
    Private(ReferencedDataModel *parent, ReferencedDataType type)
        : q(parent), mType(type)
    {
    }

public:
    const ReferencedDataType mType;
    ReferencedData *mData;

public: // slots
    void slotDataChanged(ReferencedDataType type);
    void slotCleared(ReferencedDataType type);
};

void ReferencedDataModel::Private::slotDataChanged(ReferencedDataType type)
{
    if (type == mType) {
        // Using beginResetModel/endResetModel only works since 5aa40e5b00e in Qt 5.5.
        emit q->dataChanged(q->index(0,0), q->index(0,0)); // hack to get into QComboBoxPrivate::_q_dataChanged
    }
}

void ReferencedDataModel::Private::slotCleared(ReferencedDataType type)
{
    if (type == mType) {
        // Using beginResetModel/endResetModel only works since 5aa40e5b00e in Qt 5.5.
        q->beginRemoveRows(QModelIndex(), 0, q->rowCount() - 1);
        q->endRemoveRows();
    }
}


ReferencedDataModel::ReferencedDataModel(ReferencedDataType type, QObject *parent)
    : QAbstractListModel(parent), d(new Private(this, type))
{
    d->mData = ReferencedData::instance();
    connect(d->mData, SIGNAL(dataChanged(ReferencedDataType)),
            this, SLOT(slotDataChanged(ReferencedDataType)));
    connect(d->mData, SIGNAL(cleared(ReferencedDataType)),
            this, SLOT(slotCleared(ReferencedDataType)));
}

ReferencedDataModel::~ReferencedDataModel()
{
    delete d;
}

static QString elideText(const QString& text)
{
    // huge text (e.g. account names) makes combos extremely wide
    static const int maxWidth = 50;
    if (text.length() < maxWidth)
        return text;
    return text.left(maxWidth - 3) + "...";
}

QVariant ReferencedDataModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        int row = index.row();
        // first entry is from the model itself (not in the data).
        // used for clearing a field
        if (row == 0) {
            if (role == Qt::DisplayRole) {
                return QLatin1String("");
            }
            return QVariant();
        }
        --row;

        const QMap<QString, QString> map = d->mData->data(d->mType);

        QMap<QString, QString>::const_iterator it = map.constBegin();
        it += row;
        if (it != map.constEnd()) {
            switch (role) {
            case IdRole: return it.key();
            case Qt::DisplayRole: return elideText(it.value());
            default: return QVariant();
            }
        }

        return QVariant();
    }

    return QVariant();
}

int ReferencedDataModel::rowCount(const QModelIndex &index) const
{
    if (!index.isValid()) {
        // if there is data, report one row more for the "N/A" entry (clear field)
        const int count = d->mData->data(d->mType).count();
        if (count > 0) {
            return count + 1;
        }
    }

    return 0;
}

#include "referenceddatamodel.moc"
