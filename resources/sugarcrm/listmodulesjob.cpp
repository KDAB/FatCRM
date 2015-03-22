#include "listmodulesjob.h"

#include "sugarsoap.h"

using namespace KDSoapGenerated;
#include <KDSoapClient/KDSoapMessage.h>

#include <KDebug>

#include <QStringList>

class ListModulesJob::Private
{
    ListModulesJob *const q;

public:
    explicit Private( ListModulesJob *parent ) : q( parent )
    {
    }

public:
    QStringList mModules;

public: // slots
    void listModulesDone( const KDSoapGenerated::TNS__Module_list &callResult );
    void listModulesError( const KDSoapMessage &fault );
};

void ListModulesJob::Private::listModulesDone( const KDSoapGenerated::TNS__Module_list &callResult )
{
    const KDSoapGenerated::TNS__Select_fields moduleNames = callResult.modules();
    mModules = moduleNames.items();

    kDebug() << "Got" << mModules.count() << "available modules";

    q->emitResult();
}

void ListModulesJob::Private::listModulesError( const KDSoapMessage &fault )
{
    if ( !q->handleLoginError( fault ) ) {
        kWarning() << "List Modules Error:" << fault.faultAsString();

        q->setError( SugarJob::SoapError );
        q->setErrorText( fault.faultAsString() );
        q->emitResult();
    }
}

ListModulesJob::ListModulesJob( SugarSession *session, QObject *parent )
    : SugarJob( session, parent ), d( new Private( this ) )
{
    connect( soap(), SIGNAL( get_available_modulesDone( KDSoapGenerated::TNS__Module_list ) ),
             this,  SLOT( listModulesDone( KDSoapGenerated::TNS__Module_list ) ) );
    connect( soap(), SIGNAL( get_available_modulesError( KDSoapMessage ) ),
             this,  SLOT( listModulesError( KDSoapMessage ) ) );
}

ListModulesJob::~ListModulesJob()
{
    delete d;
}

QStringList ListModulesJob::modules() const
{
    return d->mModules;
}

void ListModulesJob::startSugarTask()
{
    soap()->asyncGet_available_modules( sessionId() );
}

#include "listmodulesjob.moc"