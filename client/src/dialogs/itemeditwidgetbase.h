/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef ITEMEDITWIDGETBASE_H
#define ITEMEDITWIDGETBASE_H

#include "details.h"

#include <QWidget>

namespace Akonadi
{
class Item;
class Collection;
}

class ItemEditWidgetBase : public QWidget
{
    Q_OBJECT
public:
    explicit ItemEditWidgetBase(QWidget *parent = 0);

    static Details *createDetailsForType(DetailsType type);

    virtual void showNewItem(const QMap<QString, QString> &data, const Akonadi::Collection &collection) { Q_UNUSED(data); Q_UNUSED(collection); }
    virtual bool isModified() const = 0;
    virtual Akonadi::Item item() const = 0;

protected:
    virtual QString title() const = 0;
    virtual QString detailsName() const = 0;

Q_SIGNALS:
    void itemSaved();
    void closing();
    void openWidgetForItem(Akonadi::Item item, DetailsType type);
    void dataModified();

protected Q_SLOTS:
    virtual void accept() = 0;

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *e) override;
};

#endif // ITEMEDITWIDGETBASE_H
