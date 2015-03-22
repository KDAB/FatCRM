#ifndef SALESFORCECONFIGDIALOG_H
#define SALESFORCECONFIGDIALOG_H

#include "ui_salesforceconfigdialog.h"

class Settings;

class SalesforceConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SalesforceConfigDialog(Settings *settings, const QString &accountName);

    ~SalesforceConfigDialog();

    QString accountName() const;
    QString host() const;
    QString user() const;
    QString password() const;

private:
    Ui_SalesforceConfigDialog mUi;
};

#endif
