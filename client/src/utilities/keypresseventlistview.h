/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef KEYPRESSEVENTLISTVIEW_H
#define KEYPRESSEVENTLISTVIEW_H

#include <QListView>

/*
 * This ListView is emitting the editSelectedItem signal
 * after an item was selected and the user is pressing
 * the Enter or Return key.
 */

class KeyPressEventListView : public QListView
{
    Q_OBJECT

public:
    explicit KeyPressEventListView(QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent *event) override;

signals:
    void returnPressed(const QModelIndex &index);
};

#endif // KEYPRESSEVENTLISTVIEW_H
