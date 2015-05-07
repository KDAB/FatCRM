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

    void setAssigneeFilters(const ClientSettings::GroupFilters &assigneeFilters);
    ClientSettings::GroupFilters assigneeFilters() const;

    void setCountryFilters(const ClientSettings::GroupFilters &countryFilters);
    ClientSettings::GroupFilters countryFilters() const;

private Q_SLOTS:
    void slotGroupListClicked(const QModelIndex &idx);
    void slotGroupRemoved(const QString &group);
    void slotGroupAdded(const QString &group);
    void slotCountryListClicked(const QModelIndex &idx);
    void slotCountryRemoved(const QString &country);
    void slotCountryAdded(const QString &country);
    void slotUsersChanged();
    void slotEditCountryGroup();

private:
    Ui::ConfigurationDialog *ui;
    ClientSettings::GroupFilters m_assigneeFilters;
    ClientSettings::GroupFilters m_countryFilters;
    int m_currentFilterRow;
    int m_currentCountryGroupRow;
};

#endif // CONFIGURATIONDIALOG_H
