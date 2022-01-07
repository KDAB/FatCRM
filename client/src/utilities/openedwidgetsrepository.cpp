/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "openedwidgetsrepository.h"

OpenedWidgetsRepository *OpenedWidgetsRepository::instance()
{
    static OpenedWidgetsRepository repo;
    return &repo;
}

OpenedWidgetsRepository::~OpenedWidgetsRepository()
{
}

void OpenedWidgetsRepository::registerWidget(ItemEditWidgetBase *widget)
{
    mItemEditWidgets.insert(widget);
}

void OpenedWidgetsRepository::unregisterWidget(ItemEditWidgetBase *widget)
{
    mItemEditWidgets.remove(widget);
}

QSet<ItemEditWidgetBase*> OpenedWidgetsRepository::openedWidgets() const
{
    return mItemEditWidgets;
}

OpenedWidgetsRepository::OpenedWidgetsRepository()
{
}
