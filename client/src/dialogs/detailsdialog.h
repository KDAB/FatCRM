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

#ifndef DETAILSDIALOG_H
#define DETAILSDIALOG_H

#include <QDialog>

namespace Akonadi
{
class Item;
class Collection;
}

class Details;
class KJob;

class DetailsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DetailsDialog(Details *details, QWidget *parent = 0);

    ~DetailsDialog();

    void showNewItem(const QMap<QString, QString> &data, const Akonadi::Collection &collection);

public Q_SLOTS:
    void setItem(const Akonadi::Item &item);
    void updateItem(const Akonadi::Item &item);
    void setOnline(bool online);

Q_SIGNALS:
    void itemSaved(const Akonadi::Item &item);

private:
    bool isModified() const;

    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void saveClicked())
    Q_PRIVATE_SLOT(d, void dataModified())
    Q_PRIVATE_SLOT(d, void saveResult(KJob *job))
};

#endif
