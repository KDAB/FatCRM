/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "reportgenerator.h"

#include <KDReportsReport.h>
#include <KDReportsHeader.h>
#include <KDReportsTextElement.h>
#include <KDReportsAutoTableElement.h>

#include <KLocalizedString>

ReportGenerator::ReportGenerator()
{
}

ReportGenerator::~ReportGenerator()
{
}

void ReportGenerator::setupReport(KDReports::Report &report)
{
    report.setMargins(10, 10, 10, 10);
    report.setOrientation(QPrinter::Landscape);

    QFont font;
    font.setPointSize(8);
    report.setDefaultFont(font);

    addHeader(report);
}

void ReportGenerator::addTitle(KDReports::Report &report, const QString &title)
{
    KDReports::TextElement titleElement(title);
    titleElement.setBold(true);
    titleElement.setPointSize(14);
    report.addElement(titleElement, Qt::AlignCenter);
}

void ReportGenerator::addSubTitle(KDReports::Report &report, const QString &text)
{
    KDReports::TextElement titleElement(text);
    titleElement.setPointSize(12);
    report.addElement(titleElement, Qt::AlignCenter);
}

void ReportGenerator::addHeader(KDReports::Report &report)
{
    KDReports::Header& header = report.header();
    header.setTabPositions(QList<QTextOption::Tab>() << report.rightAlignedTab()); // ## does not work, KDReports bug
    header.addVariable(KDReports::DefaultLocaleLongDate);
    header.addInlineElement(KDReports::TextElement(QStringLiteral("\t")));
    header.addInlineElement(KDReports::TextElement(i18n("Page ")));
    header.addVariable(KDReports::PageNumber);
    header.addInlineElement(KDReports::TextElement(" / "));
    header.addVariable(KDReports::PageCount);
}

std::unique_ptr<KDReports::Report> ReportGenerator::generateListReport(QAbstractItemModel *model, const QString &title,
                                         const QString &subTitle)
{
    auto report = std::unique_ptr<KDReports::Report>(new KDReports::Report);

    setupReport(*report);
    addTitle(*report, title);
    addSubTitle(*report, subTitle);

    report->addVerticalSpacing(5);

    report->setParagraphMargins(1, 1, 1, 1);
    KDReports::AutoTableElement table(model);
    table.setVerticalHeaderVisible(false);
    // make sure country flags do not get huge
    table.setIconSize(QSize(16, 16));
    report->addElement(table);

    report->setPageSize(QPrinter::A4);

    return report;
}
