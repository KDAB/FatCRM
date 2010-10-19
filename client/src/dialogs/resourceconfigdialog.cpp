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
    Private( ResourceConfigDialog *parent ) : q( parent ), mApplyButton( 0 ) {}

public:
    Ui::ResourceConfigDialog mUi;
    QPushButton *mApplyButton;

    AgentInstance mSelectedResource;
    AgentInstance mCurrentResource;

public: // slots
    void updateButtonStates();
    void addResource();
    void configureResource();
    void syncResources();
    void removeResource();
    void resourceCreateResult( KJob *job );
    void applyResourceSelection();
    void accountNameChanged();
    void accountModeChanged( int index );
    void agentInstanceChanged( const AgentInstance &agent );

private:
    void updateDetails();
};

void ResourceConfigDialog::Private::updateButtonStates()
{
    bool canConfigure = false;
    bool canDelete = false;

    mCurrentResource = AgentInstance();

    const QList<AgentInstance> selectedResources = mUi.resources->selectedAgentInstances();
    switch ( selectedResources.count() ) {
        case 0:
            break;

        case 1:
            mCurrentResource = selectedResources.first();
            canConfigure = !mCurrentResource.type().capabilities().contains( QLatin1String( "NoConfig" ) );
            canDelete = true;
            break;

        default:
            canDelete = true;
            break;
    }

    mUi.configureResource->setEnabled( canConfigure );
    mUi.removeResource->setEnabled( canDelete );
    mUi.syncResource->setEnabled( selectedResources.count() > 0 );
    mApplyButton->setEnabled( mCurrentResource.isValid() );

    updateDetails();
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
    if ( mCurrentResource.isValid() ) {
        mCurrentResource.configure( q );
    }
}

void ResourceConfigDialog::Private::syncResources()
{
    const QList<AgentInstance> selectedResources = mUi.resources->selectedAgentInstances();
    Q_FOREACH( AgentInstance resource, selectedResources ) {
        resource.synchronize();
    }
}

void ResourceConfigDialog::Private::removeResource()
{
    const QList<AgentInstance> selectedResources = mUi.resources->selectedAgentInstances();
    Q_FOREACH( const AgentInstance &resource, selectedResources ) {
        if ( resource == mSelectedResource ) {
            mSelectedResource = AgentInstance();
        }
        if ( resource == mCurrentResource ) {
            mCurrentResource = AgentInstance();
            applyResourceSelection();
        }
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

void ResourceConfigDialog::Private::applyResourceSelection()
{
    emit q->resourceSelected( mCurrentResource );
}

void ResourceConfigDialog::Private::accountNameChanged()
{
    if ( mCurrentResource.isValid() ) {
        const QString name = mUi.accountName->text().trimmed();
        if ( name.isEmpty() ) {
            mUi.accountName->setText( mCurrentResource.name() );
        } else {
            mCurrentResource.setName( name );
        }
    }
}

void ResourceConfigDialog::Private::accountModeChanged( int index )
{
    Q_ASSERT( index == 0 || index == 1 );

    if ( mCurrentResource.isValid() ) {
        mCurrentResource.setIsOnline( index == 0 );
    }
}

void ResourceConfigDialog::Private::agentInstanceChanged( const AgentInstance &agent )
{
    if ( agent == mCurrentResource ) {
        mCurrentResource = agent;
        updateDetails();
    }
}

void ResourceConfigDialog::Private::updateDetails()
{
    mUi.accountName->blockSignals( true );
    mUi.accountMode->blockSignals( true );

    if ( mCurrentResource.isValid() ) {
        mUi.accountDetails->setEnabled( true );
        mUi.accountName->setText( mCurrentResource.name() );
        mUi.accountMode->setCurrentIndex( mCurrentResource.isOnline() ? 0 : 1 );
    } else {
        mUi.accountDetails->setEnabled( false );
        mUi.accountName->clear();
        mUi.accountMode->setCurrentIndex( 0 );
    }

    mUi.accountName->blockSignals( false );
    mUi.accountMode->blockSignals( false );
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
    connect( d->mUi.syncResource, SIGNAL( clicked() ), SLOT( syncResources() ) );
    connect( d->mUi.removeResource, SIGNAL( clicked() ), SLOT( removeResource() ) );

    connect( d->mUi.accountName, SIGNAL( editingFinished() ), SLOT( accountNameChanged() ) );
    connect( d->mUi.accountMode, SIGNAL( activated( int ) ), SLOT( accountModeChanged( int ) ) );

    d->mApplyButton = d->mUi.buttonBox->button( QDialogButtonBox::Apply );
    d->mApplyButton->setText( i18nc( "@action:button", "Select for work" ) );
    connect( d->mApplyButton, SIGNAL( clicked() ), SLOT( applyResourceSelection() ) );

    connect( AgentManager::self(), SIGNAL( instanceNameChanged( Akonadi::AgentInstance ) ),
             SLOT( agentInstanceChanged( Akonadi::AgentInstance ) ) );
    connect( AgentManager::self(), SIGNAL( instanceOnline( Akonadi::AgentInstance, bool ) ),
             SLOT( agentInstanceChanged( Akonadi::AgentInstance ) ) );

    d->updateButtonStates();
}

ResourceConfigDialog::~ResourceConfigDialog()
{
    delete d;
}

void ResourceConfigDialog::resourceSelectionChanged( const AgentInstance &resource )
{
    d->mSelectedResource = resource;

    QItemSelectionModel *selectionModel = d->mUi.resources->view()->selectionModel();
    if ( !resource.isValid() ) {
        selectionModel->clear();
        return;
    }

    AgentFilterProxyModel *filterModel = d->mUi.resources->agentFilterProxyModel();
    for ( int i = 0; i < filterModel->rowCount( QModelIndex() ); ++i ) {
        const QModelIndex index = filterModel->index( i, 0, QModelIndex() );
        if ( index.isValid() ) {
            const QVariant data = filterModel->data( index );
            if ( data.isValid() ) {
                const AgentInstance instance = data.value<AgentInstance>();
                if ( instance.isValid() && instance.identifier() == resource.identifier() ) {
                    selectionModel->select( index, QItemSelectionModel::ClearAndSelect);
                    return;
                }
            }
        }
    }
}

#include "resourceconfigdialog.moc"
