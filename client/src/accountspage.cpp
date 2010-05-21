#include "accountspage.h"

#include "sugarclient.h"

#include <akonadi/agentmanager.h>
#include <akonadi/changerecorder.h>
#include <akonadi/collection.h>
#include <akonadi/collectionfetchjob.h>
#include <akonadi/collectionfetchscope.h>
#include <akonadi/collectionstatistics.h>
#include <akonadi/itemmodel.h>
#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemdeletejob.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/itemmodifyjob.h>
#include <akonadi/cachepolicy.h>
#include <akonadi/collectionmodifyjob.h>

#include "kdcrmdata/sugaraccount.h"

using namespace Akonadi;

AccountsPage::AccountsPage( QWidget *parent )
    : QWidget( parent ),
      mChangeRecorder( new ChangeRecorder( this ) )
{
    mUi.setupUi( this );
    initialize();
}

AccountsPage::~AccountsPage()
{
}

void AccountsPage::slotResourceSelectionChanged( const QByteArray &identifier )
{
    if ( mAccountsCollection.isValid() ) {
        mChangeRecorder->setCollectionMonitored( mAccountsCollection, false );
    }

    /*
     * Look for the "Contacts" collection explicitly by listing all collections
     * of the currently selected resource, filtering by MIME type.
     * include statistics to get the number of items in each collection
     */
    CollectionFetchJob *job = new CollectionFetchJob( Collection::root(), CollectionFetchJob::Recursive );
    job->fetchScope().setResource( identifier );
    job->fetchScope().setContentMimeTypes( QStringList() << SugarAccount::mimeType() );
    job->fetchScope().setIncludeStatistics( true );
    connect( job, SIGNAL( result( KJob* ) ),
             this, SLOT( slotCollectionFetchResult( KJob* ) ) );
}

void AccountsPage::slotCollectionFetchResult( KJob *job )
{

    CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob*>( job );

    // look for the "Contacts" collection
    int i = 0;
    Q_FOREACH( const Collection &collection, fetchJob->collections() ) {
        if ( collection.remoteId() == QLatin1String( "Accounts" ) ) {
            mAccountsCollection = collection;
            i++;
            break;
        }
    }

    if ( mAccountsCollection.isValid() ) {
        mUi.newAccountPB->setEnabled( true );
        mChangeRecorder->setCollectionMonitored( mAccountsCollection, true );

        // if empty, the collection might not have been loaded yet, try synchronizing
        if ( mAccountsCollection.statistics().count() == 0 ) {
            AgentManager::self()->synchronizeCollection( mAccountsCollection );
        }

        setupCachePolicy();
    } else {
        mUi.newAccountPB->setEnabled( false );
    }
}

void AccountsPage::slotAccountChanged( const Item &item )
{
    qDebug() << "Sorry, AccountsPage::slotAccountChanged NIY";
}

void AccountsPage::slotNewAccountClicked()
{
    qDebug() << "Sorry, AccountsPage::slotNewAccountClicked NIY";
}

void AccountsPage::slotAddAccount()
{
    qDebug() << "Sorry, AccountsPage::slotAddAccount NIY";
}

void AccountsPage::slotModifyAccount()
{
    qDebug() << "Sorry, AccountsPage::slotModifyAccount NIY";
}

void AccountsPage::slotRemoveAccount()
{
    qDebug() << "Sorry, AccountsPage::slotRemoveAccount NIY";
}

void AccountsPage::slotSetCurrent( const QModelIndex& index, int start, int end )
{
    qDebug() << "Sorry, AccountsPage::slotSetCurrent NIY";
}

void AccountsPage::addAccountsData()
{
    qDebug() << "Sorry, AccountsPage::addAccountData NIY";
}

void AccountsPage::initialize()
{
    mUi.accountsTV->header()->setResizeMode( QHeaderView::ResizeToContents );

    connect( mUi.newAccountPB, SIGNAL( clicked() ),
             this, SLOT( slotNewAccountClicked() ) );
    connect( mUi.removeAccountPB, SIGNAL( clicked() ),
             this, SLOT( slotRemoveAccount() ) );

    // automatically get the full data when items change
    mChangeRecorder->itemFetchScope().fetchFullPayload( true );

    /*
     * Use an Akonadi::ItemModel because we don't have a tree of
     * collections but only a single one
     */
    Akonadi::ItemModel *accountsModel = new Akonadi::ItemModel(this );

    accountsModel->setCollection( mAccountsCollection );
    /*
    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel( this );
    filterModel->setSourceModel( accountsModel );
    filterModel->addMimeTypeInclusionFilter( SugarAccount::mimeType() );
    filterModel->setHeaderGroup( EntityTreeModel::ItemListHeaders );
    */
    mUi.accountsTV->setModel( accountsModel );

    connect( mUi.accountsTV, SIGNAL( currentChanged( Akonadi::Item ) ), this, SLOT( slotAccountChanged( Akonadi::Item ) ) );

    connect( mUi.accountsTV->model(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), SLOT( slotSetCurrent( const QModelIndex&,int,int ) ) );

}

void AccountsPage::syncronize()
{
    AgentManager::self()->synchronizeCollection( mAccountsCollection );

}

void AccountsPage::cachePolicyJobCompleted( KJob* job)
{
    if ( job->error() )
        emit statusMessage( tr("Error when setting cachepolicy: %1").arg( job->errorString() ) );
    else
        emit statusMessage( tr("Cache policy set") );

}

void AccountsPage::setupCachePolicy()
{
    CachePolicy policy;
    policy.setIntervalCheckTime( 1 ); // Check for new data every minute
    policy.setInheritFromParent( false );
    mAccountsCollection.setCachePolicy( policy );
    CollectionModifyJob *job = new CollectionModifyJob( mAccountsCollection );
    connect( job, SIGNAL( result( KJob* ) ), this, SLOT( cachePolicyJobCompleted( KJob* ) ) );
}

void AccountsPage::slotFilterChanged( const QString& filterString )
{
    Q_UNUSED( filterString );
}
