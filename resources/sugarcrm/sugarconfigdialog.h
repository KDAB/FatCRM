#ifndef SUGARCONFIGDIALOG_H
#define SUGARCONFIGDIALOG_H

#include "ui_sugarconfigdialog.h"

class Settings;

class SugarConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SugarConfigDialog(Settings *settings, const QString &accountName, QWidget *parent = 0);

    ~SugarConfigDialog();

    QString accountName() const;
    QString host() const;
    QString user() const;
    QString password() const;
    int intervalCheckTime() const;

private:
    Ui_SugarConfigDialog mUi;
};

#endif
