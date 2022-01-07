/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Kevin Funk <kevin.funk@kdab.com>

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

#ifndef CREATELINKSPROXYMODEL_H
#define CREATELINKSPROXYMODEL_H

#include <QIdentityProxyModel>

class ItemsTreeModel;

/**
 * This model attempts to turn the cell with the name of a account/opp/contact into a link
 *
 * It does so by returning a rich-text string (a HTML link) instead of the plain-text string for specific columns
 *
 * Mostly useful as input model for KDReports, as that recognizes rich-text texts in the cells and renders them accordingly.
 */
class CreateLinksProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    explicit CreateLinksProxyModel(const QString &resourceBaseUrl, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;

private:
    QString mResourceBaseUrl;
};

#endif // CREATELINKSPROXYMODEL_H
