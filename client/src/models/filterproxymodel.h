/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef FILTERPROXYMODEL_H
#define FILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "enums.h"

/**
 * A proxy model for sugar tree models.
 *
 * This class provides a filter proxy model for a sugar tree models.
 * The list of shown items can be limited by settings a filter pattern.
 * Only items that contain this pattern as part of their data will be
 * listed.
 *
 */
class FilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    /**
     * Creates a new filter proxy model.
     *
     * @param DetailsType the type of CRM object
     * @param parent The parent object.
     */
    explicit FilterProxyModel(DetailsType type, QObject *parent = Q_NULLPTR);

    /**
     * Destroys the filter proxy model.
     */
    ~FilterProxyModel();

    QString filterString() const;

    /**
     * For showing in reports
     */
    virtual QString filterDescription() const;

public Q_SLOTS:
    /**
     * Sets the filter that is used to filter for matching items
     */
    void setFilterString(const QString &filter);

protected:
    virtual bool filterAcceptsRow(int row, const QModelIndex &parent) const;

private:
    class Private;
    Private *const d;
};

#endif /* FILTERPROXYMODEL_H */

