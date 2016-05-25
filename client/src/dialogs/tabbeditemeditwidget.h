/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
class QPushButton;

class TabbedItemEditWidget : public ItemEditWidgetBase
{
    Q_OBJECT

public:
    explicit TabbedItemEditWidget(SimpleItemEditWidget *ItemEditWidget, DetailsType details, QWidget *parent = 0);
    ~TabbedItemEditWidget();

    // check if that can be protected instead
    Akonadi::Item item() const Q_DECL_OVERRIDE;
    bool isModified() const Q_DECL_OVERRIDE;
    QString title() const Q_DECL_OVERRIDE;
    QString detailsName() const Q_DECL_OVERRIDE;

private Q_SLOTS:
    void openWidget(const QString &itemKey);
    void dataChanged();
    void accept() Q_DECL_OVERRIDE;

private:
    void initialize();
    void loadAssociatedData(const QString &accountId, DetailsType type);

    SimpleItemEditWidget * mItemEditWidget;
    DetailsType mType;
    QPushButton *mSaveButton;
    QMap<QString, QPair<Akonadi::Item, DetailsType> > mItemsMap;
    Ui::TabbedItemEditWidget *mUi;
};

#endif // TABBEDITEMEDITWIDGET_H
