#include "referenceddatamodel.h"

#include "referenceddata.h"

class ReferencedDataModel::Private
{
    ReferencedDataModel *const q;

public:
    Private(ReferencedDataModel *parent)
        : q(parent)
    {
    }

public:
    ReferencedData *mData;

public: // slots
    void slotDataChanged(int row);
};

void ReferencedDataModel::Private::slotDataChanged(int row)
{
    // Using beginResetModel/endResetModel only works since 5aa40e5b00e in Qt 5.5.
    const QModelIndex idx = q->index(row, 0);
    emit q->dataChanged(idx, idx);
}



ReferencedDataModel::ReferencedDataModel(ReferencedDataType type, QObject *parent)
    : QAbstractListModel(parent), d(new Private(this))
{
    d->mData = ReferencedData::instance(type);
    connect(d->mData, SIGNAL(dataChanged()),
            this, SLOT(slotDataChanged()));
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

        const QPair<QString, QString> pair = d->mData->data(row);

        switch (role) {
        case IdRole: return pair.first;
        case Qt::DisplayRole: return elideText(pair.second);
        }

        return QVariant();
    }

    return QVariant();
}

int ReferencedDataModel::rowCount(const QModelIndex &index) const
{
    if (!index.isValid()) {
        // if there is data, report one row more for the "N/A" entry (clear field)
        const int count = d->mData->count();
        if (count > 0) {
            return count + 1;
        }
    }

    return 0;
}

#include "referenceddatamodel.moc"
