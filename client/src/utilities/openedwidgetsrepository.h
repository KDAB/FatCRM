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

#ifndef OPENEDWIDGETSREPOSITORY_H
#define OPENEDWIDGETSREPOSITORY_H

#include <QObject>

#include "itemeditwidgetbase.h"

class ItemEditWidgetBase;

class OpenedWidgetsRepository : public QObject
{
    Q_OBJECT
public:
    static OpenedWidgetsRepository *instance();
    ~OpenedWidgetsRepository() override;

    void registerWidget(ItemEditWidgetBase *widget);
    void unregisterWidget(ItemEditWidgetBase *widget);
    QSet<ItemEditWidgetBase*> openedWidgets() const;

private:
    QSet<ItemEditWidgetBase*> mItemEditWidgets;

    OpenedWidgetsRepository();
};

#endif // OPENEDWIDGETSREPOSITORY_H
