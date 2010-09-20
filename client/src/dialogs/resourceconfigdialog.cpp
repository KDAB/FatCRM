#include "resourceconfigdialog.h"

#include "ui_resourceconfigdialog.h"

#include <akonadi/agentfilterproxymodel.h>
#include <akonadi/agentinstance.h>
#include <akonadi/agentinstancecreatejob.h>
#include <akonadi/agentinstancewidget.h>
#include <akonadi/agentmanager.h>
#include <akonadi/agenttype.h>
#include <akonadi/agenttypedialog.h>

#include <KDebug>

#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QMessageBox>

using namespace Akonadi;

class ResourceConfigDialog::Private
{
    ResourceConfigDialog *const q;

public:
    Private( ResourceConfigDialog *parent ) : q( parent ) {}

public:
    Ui::ResourceConfigDialog mUi;

public: // slots
    void updateButtonStates();
    void addResource();
    void configureResource();
    void removeResource();
    void resourceCreateResult( KJob *job );
};

void ResourceConfigDialog::Private::updateButtonStates()
{
    bool canConfigure = false;
    bool canDelete = false;

    const QList<AgentInstance> selectedResources = mUi.resources->selectedAgentInstances();
    switch ( selectedResources.count() ) {
        case 0:
            break;

        case 1:
            canConfigure = !selectedResources.first().type().capabilities().contains( QLatin1String( "NoConfig" ) );
            canDelete = true;
            break;

        default:
            canDelete = true;
            break;
    }

    mUi.configureResource->setEnabled( canConfigure );
    mUi.removeResource->setEnabled( canDelete );
}

void ResourceConfigDialog::Private::addResource()
{
    AgentTypeDialog dialog( q );

    AgentFilterProxyModel* filterModel = dialog.agentFilterProxyModel();
    filterModel->addCapabilityFilter( QLatin1String( "KDCRM" ) );

    if ( dialog.exec() == QDialog::Accepted ) {
        const AgentType resourceType = dialog.agentType();
        if ( resourceType.isValid() ) {
            AgentInstanceCreateJob *job = new AgentInstanceCreateJob( resourceType );
            job->configure( q );
            connect( job, SIGNAL( result( KJob* ) ), q, SLOT( resourceCreateResult( KJob* ) ) );
            job->start();
        }
    }
}

void ResourceConfigDialog::Private::configureResource()
{
    const QList<AgentInstance> selectedResources = mUi.resources->selectedAgentInstances();
    Q_ASSERT( selectedResources.count() == 1 );

    AgentInstance resource = selectedResources.first();
    resource.configure( q );
}

void ResourceConfigDialog::Private::removeResource()
{
    const QList<AgentInstance> selectedResources = mUi.resources->selectedAgentInstances();
    Q_FOREACH( const AgentInstance &resource, selectedResources ) {
        AgentManager::self()->removeInstance( resource );
    }
}

void ResourceConfigDialog::Private::resourceCreateResult( KJob *job )
{
    kDebug() << "error=" << job->error() << "string=" << job->errorString();
    if ( job->error() != 0 ) {
        QMessageBox::critical( q, i18nc( "@title:window", "Failed to create CRM connector" ),
                               job->errorString() );
    }
}

ResourceConfigDialog::ResourceConfigDialog( QWidget *parent )
    : QDialog( parent ), d( new Private( this ) )
{
    d->mUi.setupUi( this );

    AgentFilterProxyModel *filterModel = d->mUi.resources->agentFilterProxyModel();
    filterModel->addCapabilityFilter( QLatin1String( "KDCRM" ) );

    connect( d->mUi.resources->view()->selectionModel(),
             SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ),
             SLOT( updateButtonStates() ) );

    connect( d->mUi.addResource, SIGNAL( clicked() ), SLOT( addResource() ) );
    connect( d->mUi.configureResource, SIGNAL( clicked() ), SLOT( configureResource() ) );
    connect( d->mUi.removeResource, SIGNAL( clicked() ), SLOT( removeResource() ) );

    d->updateButtonStates();
}

ResourceConfigDialog::~ResourceConfigDialog()
{
    delete d;
}

#include "resourceconfigdialog.moc"
