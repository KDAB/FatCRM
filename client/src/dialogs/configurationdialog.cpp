#include "configurationdialog.h"
#include "ui_configurationdialog.h"

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}

void ConfigurationDialog::setFullUserName(const QString &fullUserName)
{
    ui->fullName->setText(fullUserName);
}

QString ConfigurationDialog::fullUserName() const
{
    return ui->fullName->text();
}
