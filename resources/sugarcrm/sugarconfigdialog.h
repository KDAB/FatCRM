#ifndef SUGARCONFIGDIALOG_H
#define SUGARCONFIGDIALOG_H

#include "ui_sugarconfigdialog.h"

class Settings;

class SugarConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SugarConfigDialog( Settings *settings );

    ~SugarConfigDialog();

    QString host() const;
    QString user() const;
    QString password() const;

private:
    Ui_SugarConfigDialog mUi;
};

#endif
