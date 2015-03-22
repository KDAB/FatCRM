#include "sugarconfigdialog.h"

#include "settings.h"

SugarConfigDialog::SugarConfigDialog(Settings *settings, const QString &accountName, QWidget *parent)
    : QDialog(parent)
{
    mUi.setupUi(this);

    mUi.accountName->setText(accountName);
    mUi.host->setText(settings->host());
    mUi.user->setText(settings->user());
    mUi.password->setText(settings->password());
}

SugarConfigDialog::~SugarConfigDialog()
{
}

QString SugarConfigDialog::accountName() const
{
    return mUi.accountName->text();
}

QString SugarConfigDialog::host() const
{
    return mUi.host->text();
}

QString SugarConfigDialog::user() const
{
    return mUi.user->text();
}

QString SugarConfigDialog::password() const
{
    return mUi.password->text();
}

#include "sugarconfigdialog.moc"
