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

    void generateListReport(QAbstractItemModel *model, const QString &title, QWidget *parent);

private:
    void setupReport(KDReports::Report &report);
    void addHeader(KDReports::Report &report);
    void addTitle(KDReports::Report &report, const QString &title);
    void finalizeReport(KDReports::Report &report, QWidget *parent);
};

#endif // REPORTGENERATOR_H
