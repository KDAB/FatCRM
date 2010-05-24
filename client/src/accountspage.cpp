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
        mAccountsModel->setCollection( mAccountsCollection );
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

    //qDebug() << "AccountsPage::slotAccountChanged( const Item &item )";
    //qDebug() << "item payload " << item.hasPayload<SugarAccount>();
    if ( item.isValid() && item.hasPayload<SugarAccount>() ) {
        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        if ( w ) {
            w->accountDetailsWidget()->setItem( item );
            connect( w->accountDetailsWidget(), SIGNAL( modifyContact() ),
                 this, SLOT( slotModifyContact( ) ) );
        }
        emit accountItemChanged();
    }
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
        if ( start == end ) {
            QModelIndex newIdx = mUi.accountsTV->model()->index(start, 0, index);
            mUi.accountsTV->setCurrentIndex( newIdx );
        }
        //model items are loaded
        if ( mUi.accountsTV->model()->rowCount() == mAccountsCollection.statistics().count() )
            addAccountsData();
}

void AccountsPage::addAccountsData()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    AccountDetails *cd = w->accountDetailsWidget();
    QModelIndex index;
    Item item;
    SugarAccount account;
    for ( int i = 0; i <  mUi.accountsTV->model()->rowCount(); ++i ) {
       index  =  mUi.accountsTV->model()->index( i, 0 );
       item = mUi.accountsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
       if ( item.hasPayload<SugarAccount>() ) {
           account = item.payload<SugarAccount>();
           cd->addAccountData( account.name(), account.id() );
           cd->addCampaignData( account.campaignName(), account.campaignId() );
           cd->addAssignedToData( account.assignedUserName(), account.assignedUserId() );
       }
    }
    // fill accounts combo
    cd->fillCombos();
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
    mAccountsModel = new Akonadi::ItemModel(this );

    //accountsModel->setCollection( mAccountsCollection );
    /*
    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel( this );
    filterModel->setSourceModel( accountsModel );
    filterModel->addMimeTypeInclusionFilter( SugarAccount::mimeType() );
    filterModel->setHeaderGroup( EntityTreeModel::ItemListHeaders );
    */
    mUi.accountsTV->setModel( mAccountsModel );

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
