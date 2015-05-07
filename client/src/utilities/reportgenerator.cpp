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

void ReportGenerator::addHeader(KDReports::Report &report)
{
    KDReports::Header& header = report.header();
    header.setTabPositions(QList<QTextOption::Tab>() << report.rightAlignedTab());
    header.addInlineElement(KDReports::TextElement("\t"));
    header.addVariable(KDReports::DefaultLocaleLongDate);
}

void ReportGenerator::generateListReport(QAbstractItemModel *model, const QString &title, QWidget *parent)
{
    KDReports::Report report;
    setupReport(report);
    addTitle(report, title);

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
