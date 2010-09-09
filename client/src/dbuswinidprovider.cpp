#include "dbuswinidprovider.h"

#include <KDebug>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QWidget>

class DBusWinIdProvider::Private
{
    DBusWinIdProvider *const q;
public:
    Private( DBusWinIdProvider *parent, QWidget *window )
        : q( parent ), mWindow( window ), mObjectRegistered( false )
    {
    }

public:
    QWidget *mWindow;
    bool mObjectRegistered;

public: // slots
    void tryRegister();
};

void DBusWinIdProvider::Private::tryRegister()
{
    // we don't care if we can't register or if we are replaced by the real tray
    // this is for fallback in case the tray app is not running
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusConnectionInterface *busInterface = bus.interface();
    busInterface->registerService( QLatin1String( "org.freedesktop.akonaditray" ),
                                   QDBusConnectionInterface::DontQueueService,
                                   QDBusConnectionInterface::AllowReplacement );

    if ( !mObjectRegistered ) {
        mObjectRegistered = bus.registerObject( QLatin1String( "/Actions" ), q,
                                                QDBusConnection::ExportScriptableSlots );
        if ( !mObjectRegistered ) {
            kWarning() << "Failed to register provider object /Actions";
        }
    }

    kDebug() << "currentOwner="
             << busInterface->serviceOwner( QLatin1String( "org.freedesktop.akonaditray" ) );
}

DBusWinIdProvider::DBusWinIdProvider( QWidget *referenceWindow )
    : QObject( referenceWindow ), d( new Private( this, referenceWindow ) )
{
    Q_ASSERT( referenceWindow != 0 );

    QDBusServiceWatcher *watcher =
        new QDBusServiceWatcher( QLatin1String( "org.freedesktop.akonaditray" ),
                                 QDBusConnection::sessionBus(),
                                 QDBusServiceWatcher::WatchForUnregistration,
                                 this );
    connect( watcher, SIGNAL( serviceUnregistered( QString ) ),
             this, SLOT( tryRegister() ) );

    QMetaObject::invokeMethod( this, "tryRegister", Qt::QueuedConnection );
}

DBusWinIdProvider::~DBusWinIdProvider()
{
    delete d;
}

qlonglong DBusWinIdProvider::getWinId()
{
    return d->mWindow->winId();
}

#include "dbuswinidprovider.moc"
