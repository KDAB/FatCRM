/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Tobias Koenig <tobias.koenig@kdab.com>

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

#ifndef FATCRMINPUTDIALOG_H
#define FATCRMINPUTDIALOG_H

#include <QDate>
#include <QPair>
#include <QString>

namespace FatCRMInputDialog {

    QPair<QString, QString> getAssignedUser(const QString &caption, const QString &label, bool *ok = 0);

    QDate getDate(const QString &caption, const QString &label, const QDate &value, bool *ok = 0);
}

#endif
