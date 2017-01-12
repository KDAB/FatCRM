/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Michel Boyer de la Giroday <michel.giroday@kdab.com>

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

#ifndef MODELREPOSITORY_H
#define MODELREPOSITORY_H

#include "enums.h"

#include <QObject>
#include <QMap>

class ItemsTreeModel;

class ModelRepository : public QObject
{
    Q_OBJECT
public:
    static ModelRepository *instance();
    ~ModelRepository() override;

    void setModel(DetailsType type, ItemsTreeModel *model);
    ItemsTreeModel *model(DetailsType type);
    void removeModel(DetailsType key);

private:
    ModelRepository();

    QMap<DetailsType, ItemsTreeModel*> mModelsMap;
};

#endif // MODELREPOSITORY_H
