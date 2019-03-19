/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

class QAbstractItemModel;
namespace KDReports {
    class Report;
}
class QString;
class QWidget;

class ReportGenerator
{
public:
    ReportGenerator();

    void generateListReport(QAbstractItemModel *model, const QString &title, const QString &subTitle, QWidget *parent);

private:
    void setupReport(KDReports::Report &report);
    void addHeader(KDReports::Report &report);
    void addTitle(KDReports::Report &report, const QString &title);
    void addSubTitle(KDReports::Report &report, const QString &text);
    void finalizeReport(KDReports::Report &report, QWidget *parent);
};

#endif // REPORTGENERATOR_H
