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

#include "reportgenerator.h"
#include <KDReportsReport.h>
#include <KDReportsHeader.h>
#include <KDReportsTextElement.h>
#include <KDReportsAutoTableElement.h>
#include <KDReportsPreviewDialog.h>
#include <KDReportsPreviewWidget.h>

ReportGenerator::ReportGenerator()
{
}

void ReportGenerator::setupReport(KDReports::Report &report)
{
    report.setMargins(5, 10, 5, 10);
    report.setOrientation(QPrinter::Landscape);

    QFont font;
    font.setPointSize(10);
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
    header.setTabPositions(QList<QTextOption::Tab>() << report.rightAlignedTab());
    header.addInlineElement(KDReports::TextElement("\t"));
    header.addVariable(KDReports::DefaultLocaleLongDate);
}

void ReportGenerator::generateListReport(QAbstractItemModel *model, const QString &title,
                                         const QString &subTitle, QWidget *parent)
{
    KDReports::Report report;
    setupReport(report);
    addTitle(report, title);
    addSubTitle(report, subTitle);

    report.addVerticalSpacing(5);

    report.setParagraphMargins(1, 1, 1, 1);
    KDReports::AutoTableElement table(model);
    table.setVerticalHeaderVisible(false);
    report.addElement(table);

    finalizeReport(report, parent);
}

void ReportGenerator::finalizeReport(KDReports::Report &report, QWidget *parent)
{
    report.setPageSize(QPrinter::A4);

    KDReports::PreviewDialog preview(&report, parent);
    preview.previewWidget()->setShowPageListWidget(false);
    preview.previewWidget()->setShowTableSettingsDialog(false);
    preview.resize(1167, 906);
    preview.exec();
}
