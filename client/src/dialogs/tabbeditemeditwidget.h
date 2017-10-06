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

#ifndef TABBEDITEMEDITWIDGET_H
#define TABBEDITEMEDITWIDGET_H

#include "itemeditwidgetbase.h"

#include <QObject>

#include <AkonadiCore/Item>


namespace Ui {
class TabbedItemEditWidget;
}

class SimpleItemEditWidget;

class TabbedItemEditWidget : public ItemEditWidgetBase
{
    Q_OBJECT

public:
    explicit TabbedItemEditWidget(SimpleItemEditWidget *ItemEditWidget, DetailsType details, QWidget *parent = nullptr);
    ~TabbedItemEditWidget() override;

    // check if that can be protected instead
    Akonadi::Item item() const override;
    bool isModified() const override;
    QString title() const override;
    QString detailsName() const override;

private Q_SLOTS:
    void openWidget(const QString &itemKey);
    void dataChanged();
    void accept() override;

private:
    void initialize();
    int loadAssociatedData(const QString &accountId, DetailsType type);

    SimpleItemEditWidget * mItemEditWidget;
    DetailsType mType;
    QMap<QString, QPair<QPersistentModelIndex, DetailsType> > mItemsMap;
    Ui::TabbedItemEditWidget *mUi;
};

#endif // TABBEDITEMEDITWIDGET_H
