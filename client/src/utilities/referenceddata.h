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

#ifndef REFERENCEDDATA_H
#define REFERENCEDDATA_H

#include "enums.h"

#include <QObject>

template <typename K, typename V> struct QPair;
template <typename K, typename V> class QMap;

/**
 * @brief Per-type singleton holding all reference data, for comboboxes
 * (accounts list, assigned-to list, etc.)
 *
 * Used with a ReferencedDataModel on top.
 */
class ReferencedData : public QObject
{
    Q_OBJECT

public:
    static ReferencedData *instance(ReferencedDataType type);

    ~ReferencedData();

//    void clear();

    void setReferencedData(const QString &id, const QString &data);
    void addMap(const QMap<QString, QString> &idDataMap, bool emitChanges);

    QString referencedData(const QString &id) const;

    void removeReferencedData(const QString &id);

    QPair<QString, QString> data(int row) const;
    int count() const;


    ReferencedDataType dataType() const;

    void emitInitialLoadingDone();

    static void emitInitialLoadingDoneForAll();

Q_SIGNALS:
    void dataChanged(int row);
    void rowsAboutToBeInserted(int start, int end);
    void rowsInserted();
    void rowsAboutToBeRemoved(int start, int end);
    void rowsRemoved();

    void initialLoadingDone();

private:
    explicit ReferencedData(ReferencedDataType type, QObject *parent = 0);
    void setReferencedDataInternal(const QString &id, const QString &data, bool emitChanges);

private:
    class Private;
    Private *const d;
};

#endif
