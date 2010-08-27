#include "shutdowndialog.h"

#include "ui_shutdowndialog.h"

#include <QButtonGroup>
#include <QDebug>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>

class ShutdownDialog::Private
{
    ShutdownDialog *const q;

public:
    Private( ShutdownDialog *parent ) : q( parent ) {}

public:
    Ui::ShutdownDialog mUi;
    QButtonGroup mButtons;
};

ShutdownDialog::ShutdownDialog( QWidget *parent )
    : QDialog( parent ), d( new Private( this ) )
{
    d->mUi.setupUi( this );

    d->mUi.buttonBox->button( QDialogButtonBox::Ok )->setText( tr( "Quit" ) );

    d->mButtons.setExclusive( true );
    d->mButtons.addButton( d->mUi.justQuit, JustQuit );
    d->mButtons.addButton( d->mUi.quitAndOffline, QuitAndAgentsOffline );
    d->mButtons.addButton( d->mUi.quitAndStop, QuitAndStopAkonadi );
}

ShutdownDialog::~ShutdownDialog()
{
    delete d;
}

ShutdownDialog::ShutdownOptions ShutdownDialog::getShutdownOption( QWidget *parent )
{
    QSettings settings( QLatin1String( "KDAB" ), QLatin1String( "FatCRM" ) );
    const bool dontAsk = settings.value( QLatin1String( "Shutdown/DontAskAgain" ), false ).toBool();

    const QVariant optionVar = settings.value( QLatin1String( "Shutdown/Option" ) );
    if ( dontAsk && optionVar.isValid() ) {
        switch ( optionVar.toInt() ) {
            case JustQuit: return JustQuit;
            case QuitAndAgentsOffline: return QuitAndAgentsOffline;
            case QuitAndStopAkonadi: return QuitAndStopAkonadi;
            default:
                // invalid, ask again
                break;
        }
    }

    ShutdownDialog dialog( parent );
    dialog.d->mButtons.button( optionVar.isValid() ? optionVar.toInt() : JustQuit )->setChecked( true );
    if ( dialog.exec() == QDialog::Rejected ) {
        return CancelShutdown;
    }

    ShutdownOptions result = static_cast<ShutdownOptions>( dialog.d->mButtons.checkedId() );

    settings.setValue( QLatin1String( "Shutdown/Option" ), result );
    settings.setValue( QLatin1String( "Shutdown/DontAskAgain" ),
                       dialog.d->mUi.dontAskAgain->isChecked() );

    return result;
}

#include "shutdowndialog.moc"
