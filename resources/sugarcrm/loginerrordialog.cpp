#include "loginerrordialog.h"

#include "settings.h"
#include "sugarconfigdialog.h"
#include "sugarsession.h"

#include <Akonadi/ResourceBase>
#include <KJob>
#include <KLocale>
#include <KPushButton>

#include <QLabel>

class LoginErrorDialog::Private
{
    LoginErrorDialog *const q;
public:
    Private( LoginErrorDialog *parent, KJob *job, SugarSession *session, Akonadi::ResourceBase *resource )
        : q( parent ), mJob( job ), mSession( session ), mResource(resource)
    {
    }

public:
    KJob *mJob;
    SugarSession *mSession;
    Akonadi::ResourceBase *mResource;

public: // slots
    void changeConfig();
    void cancel();
};

void LoginErrorDialog::Private::changeConfig()
{
    SugarConfigDialog dialog( Settings::self(), mResource->name(), q );
    if ( dialog.exec() == QDialog::Rejected ) {
        return;
    }

    const QString accountName = dialog.accountName();
    const QString host = dialog.host();
    const QString user = dialog.user();
    const QString password = dialog.password();

    SugarSession::RequiredAction action = mSession->setSessionParameters( user, password, host );
    switch ( action ) {
        case SugarSession::None:
            break;

        case SugarSession::NewLogin:
            mSession->createSoapInterface();
            // fall through
        case SugarSession::ReLogin:
            break;
    }

    mResource->setName(accountName);

    Settings::self()->setHost( host );
    Settings::self()->setUser( user );
    Settings::self()->setPassword( password );
    Settings::self()->writeConfig();

    q->accept();
}

void LoginErrorDialog::Private::cancel()
{
    mJob->deleteLater();
    q->reject();
}

LoginErrorDialog::LoginErrorDialog(KJob *job, SugarSession *session, Akonadi::ResourceBase *resource, QWidget *parent )
    : KDialog( parent ), d( new Private( this, job, session, resource ) )
{
    job->setAutoDelete( false );

    setButtons( User1 | User2 | User3 );
    setDefaultButton( User3 );

    button( User3 )->setText( i18nc( "@action:button", "Retry" ) );
    button( User2 )->setText( i18nc( "@action:button", "Change Configuration..." ) );
    button( User1 )->setText( i18nc( "@action:button", "Cancel" ) );

    connect( this, SIGNAL( user3Clicked() ), SLOT( accept() ) );
    connect( this, SIGNAL( user2Clicked() ), SLOT( changeConfig() ) );
    connect( this, SIGNAL( user1Clicked() ), SLOT( cancel() ) );

    QWidget *widget = new QWidget( this );
    QHBoxLayout *box = new QHBoxLayout( widget );

    QLabel *iconLabel = new QLabel( widget );
    box->addWidget( iconLabel );

    QLabel *textLabel = new QLabel( widget );
    box->addWidget( textLabel );

    setMainWidget( widget );

    if ( style() != 0 ) {
        QIcon icon = style()->standardIcon( QStyle::SP_MessageBoxCritical );
        if ( !icon.isNull() ) {
            iconLabel->setPixmap( icon.pixmap( QSize( 64, 64 ) ) );
        } else {
            iconLabel->hide();
        }
    } else {
        iconLabel->hide();
    }
    textLabel->setText( d->mJob->errorText() );
}

LoginErrorDialog::~LoginErrorDialog()
{
    delete d;
}

#include "loginerrordialog.moc"
