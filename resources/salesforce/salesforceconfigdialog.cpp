#include "salesforceconfigdialog.h"

#include "settings.h"

SalesforceConfigDialog::SalesforceConfigDialog(Settings *settings, const QString &accountName)
    : QDialog()
{
    mUi.setupUi( this );

    mUi.accountName->setText( accountName );
#if 0
    mUi.host->setText( settings->host() );
#else
    // hide the host/endpoint input until we find a way to determine the
    // end point URL from some user visible one
    mUi.label->setVisible( false );
    mUi.host->setVisible( false );
#endif
    mUi.user->setText( settings->user() );
    mUi.password->setText( settings->password() );
}

SalesforceConfigDialog::~SalesforceConfigDialog()
{
}

QString SalesforceConfigDialog::accountName() const
{
    return mUi.accountName->text();
}

QString SalesforceConfigDialog::host() const
{
    return mUi.host->text();
}

QString SalesforceConfigDialog::user() const
{
    return mUi.user->text();
}

QString SalesforceConfigDialog::password() const
{
    return mUi.password->text();
}

#include "salesforceconfigdialog.moc"
