#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>
#include <QMap>
#include "clientsettings.h"
class QModelIndex;

namespace Ui {
class ConfigurationDialog;
}

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDialog(QWidget *parent = 0);
    ~ConfigurationDialog();

    void setFullUserName(const QString &fullUserName);
    QString fullUserName() const;

    void setAssigneeFilters(const ClientSettings::AssigneeFilters &assigneeFilters);
    ClientSettings::AssigneeFilters assigneeFilters() const;

private Q_SLOTS:
    void slotGroupListClicked(const QModelIndex &idx);
    void slotGroupRemoved(const QString &group);
    void slotGroupAdded(const QString &group);
    void slotUsersChanged();

private:
    Ui::ConfigurationDialog *ui;
    ClientSettings::AssigneeFilters m_assigneeFilters;
    int m_currentFilterRow;
};

#endif // CONFIGURATIONDIALOG_H
