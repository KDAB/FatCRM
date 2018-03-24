/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef REPORTPAGE_H
#define REPORTPAGE_H

#include <QWidget>

namespace Ui {
class ReportPage;
}
class ItemsTreeModel;

class ReportPage : public QWidget
{
    Q_OBJECT

public:
    explicit ReportPage(QWidget *parent = nullptr);
    ~ReportPage() override;

    void setOppModel(ItemsTreeModel *model);

private slots:
    void on_calculateCreatedWonLostReport_clicked();
    void on_calculateOpenPerCountryReport_clicked();
    void on_pbMonthlySpreadsheet_clicked();

private:
    Ui::ReportPage *ui;
    ItemsTreeModel *mOppModel = nullptr;
};

#endif // REPORTPAGE_H
