/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef REFERENCEDDATAMODEL_H
#define REFERENCEDDATAMODEL_H

#include "enums.h"
#include "fatcrmprivate_export.h"
#include <QAbstractListModel>
class QComboBox;

// List model for filling comboboxes referencing an account/campaign/user/etc.
class FATCRMPRIVATE_EXPORT ReferencedDataModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ReferencedDataModel(ReferencedDataType type, QObject *parent = nullptr);

    ~ReferencedDataModel() override;

    static void setModelForCombo(QComboBox *combo, ReferencedDataType type);

    /* reimpl */ QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /* reimpl */ int rowCount(const QModelIndex &index = QModelIndex()) const override;

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void slotDataChanged(int))
    Q_PRIVATE_SLOT(d, void slotRowsAboutToBeInserted(int, int))
    Q_PRIVATE_SLOT(d, void slotRowsInserted())
    Q_PRIVATE_SLOT(d, void slotRowsAboutToBeRemoved(int, int))
    Q_PRIVATE_SLOT(d, void slotRowsRemoved())
    Q_PRIVATE_SLOT(d, void slotInitialLoadingDone())
    Q_PRIVATE_SLOT(d, void slotCleared())
};

#endif
