/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "referenceddatamodel.h"
#include "referenceddata.h"

#include <QComboBox>
#include <QSortFilterProxyModel>

class ReferencedDataModel::Private
{
    ReferencedDataModel *const q;

public:
    Private(ReferencedDataModel *parent)
        : q(parent), mData(nullptr)
    {
    }

public:
    ReferencedData *mData;

public: // slots
    void slotDataChanged(int row);
    void slotRowsAboutToBeInserted(int, int);
    void slotRowsInserted();
    void slotRowsAboutToBeRemoved(int, int);
    void slotRowsRemoved();
    void slotInitialLoadingDone();
    void slotCleared();
};

void ReferencedDataModel::Private::slotDataChanged(int row)
{
    const QModelIndex idx = q->index(row + 1, 0); // +1 for the empty item at the top
    emit q->dataChanged(idx, idx);
}

void ReferencedDataModel::Private::slotRowsAboutToBeInserted(int start, int end)
{
    q->beginInsertRows(QModelIndex(), start + 1, end + 1); // +1 for the empty item at the top
}

void ReferencedDataModel::Private::slotRowsInserted()
{
    q->endInsertRows();
}

void ReferencedDataModel::Private::slotRowsAboutToBeRemoved(int start, int end)
{
    q->beginRemoveRows(QModelIndex(), start + 1, end + 1); // +1 for the empty item at the top
}

void ReferencedDataModel::Private::slotRowsRemoved()
{
    q->endRemoveRows();
}

void ReferencedDataModel::Private::slotInitialLoadingDone()
{
    // workaround QComboBox not resizing itself on layoutChanged or modelReset (only works since 5aa40e5b00e in Qt 5.5)
    q->beginInsertRows(QModelIndex(), 0, q->rowCount() - 1);
    q->endInsertRows();
    // we cheated a bit, so force the sorting to actually happen
    emit q->layoutChanged();
}

void ReferencedDataModel::Private::slotCleared()
{
    // do we need aboutToClear()?
    q->beginRemoveRows(QModelIndex(), 0, q->rowCount() - 1);
    q->endRemoveRows();
}

ReferencedDataModel::ReferencedDataModel(ReferencedDataType type, QObject *parent)
    : QAbstractListModel(parent), d(new Private(this))
{
    d->mData = ReferencedData::instance(type);
    connect(d->mData, SIGNAL(dataChanged(int)), this, SLOT(slotDataChanged(int)));
    connect(d->mData, SIGNAL(rowsAboutToBeInserted(int,int)), this, SLOT(slotRowsAboutToBeInserted(int,int)));
    connect(d->mData, SIGNAL(rowsInserted()), this, SLOT(slotRowsInserted()));
    connect(d->mData, SIGNAL(rowsAboutToBeRemoved(int,int)), this, SLOT(slotRowsAboutToBeRemoved(int,int)));
    connect(d->mData, SIGNAL(rowsRemoved()), this, SLOT(slotRowsRemoved()));
    connect(d->mData, SIGNAL(initialLoadingDone()), this, SLOT(slotInitialLoadingDone()));
    connect(d->mData, SIGNAL(cleared()), this, SLOT(slotCleared()));
}

ReferencedDataModel::~ReferencedDataModel()
{
    delete d;
}

void ReferencedDataModel::setModelForCombo(QComboBox *combo, ReferencedDataType type)
{
    auto *proxy = new QSortFilterProxyModel(combo);
    proxy->setDynamicSortFilter(true);
    proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxy->setSortLocaleAware(true);
    auto *model = new ReferencedDataModel(type, combo);
    proxy->setSourceModel(model);
    proxy->sort(0);
    combo->setModel(proxy);
    combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
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
            if (role == Qt::DisplayRole || role == Qt::UserRole) {
                return QString();
            }
            return QVariant();
        }
        --row;

        const QPair<QString, QString> pair = d->mData->data(row);

        switch (role) {
        case Qt::UserRole: return pair.first;
        case Qt::DisplayRole: return elideText(pair.second);
        }

        return QVariant();
    }

    return QVariant();
}

int ReferencedDataModel::rowCount(const QModelIndex &index) const
{
    if (!index.isValid()) {
        // report one row more for the "N/A" entry (clear field)
        const int count = d->mData->count();
        return count + 1;
    }

    return 0;
}
#include "moc_referenceddatamodel.cpp"
