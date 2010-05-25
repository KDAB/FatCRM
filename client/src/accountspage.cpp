#include "accountspage.h"
#include "accountstreemodel.h"
#include "sugarclient.h"

#include <akonadi/agentmanager.h>
#include <akonadi/changerecorder.h>
#include <akonadi/collection.h>
#include <akonadi/collectionfetchjob.h>
#include <akonadi/collectionfetchscope.h>
#include <akonadi/collectionstatistics.h>
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
     * Look for the "Accounts" collection explicitly by listing all collections
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

    // look for the "Accounts" collection
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

    if ( item.isValid() && item.hasPayload<SugarAccount>() ) {
        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        if ( w ) {
            w->accountDetailsWidget()->setItem( item );
            connect( w->accountDetailsWidget(), SIGNAL( modifyAccount() ),
                 this, SLOT( slotModifyAccount( ) ) );
        }
        emit accountItemChanged();
    }
}

void AccountsPage::slotNewAccountClicked()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w ) {
        AccountDetails *cd = w->accountDetailsWidget();
        cd->clearFields();
        connect( cd, SIGNAL( saveAccount() ),
                 this, SLOT( slotAddAccount( ) ) );
    }

    emit showDetails();
}

void AccountsPage::slotAddAccount()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    QMap<QString, QString> data;
    data = w->accountDetailsWidget()->accountData();
    SugarAccount account;
    account.setName( data.value( "name" ) );
    account.setDateEntered( data.value( "dateEntered" ) );
    account.setDateModified( data.value( "dateModified" ) );
    account.setModifiedUserId( data.value( "modifiedUserId" ) );
    account.setModifiedByName( data.value( "modifiedByName" ) );
    account.setCreatedBy( data.value( "createdBy" ) ); // id
    account.setCreatedByName( data.value( "createdByName" ) );
    account.setDescription( data.value( "description" ) );
    account.setDeleted( data.value( "deleted" ) );
    account.setAssignedUserId( data.value( "assignedUserId" ) );
    account.setAssignedUserName( data.value( "assignedUserName" ) );
    account.setAccountType( data.value( "accountType" ) );
    account.setIndustry( data.value( "industry" ) );
    account.setAnnualRevenue( data.value( "annualRevenue" ) );
    account.setPhoneFax( data.value( "phoneFax" ) );
    account.setBillingAddressStreet( data.value( "billingAddressStreet" ) );
    account.setBillingAddressCity( data.value( "billingAddressCity" ) );
    account.setBillingAddressState( data.value( "billingAddressState" ) );
    account.setBillingAddressPostalcode( data.value( "billingAddressPostalcode" ) );
    account.setBillingAddressCountry( data.value( "billingAddressCountry" ) );
    account.setRating( data.value( "rating" ) );
    account.setPhoneOffice( data.value( "phoneOffice" ) );
    account.setPhoneAlternate( data.value( "phoneAlternate" ) );
    account.setWebsite( data.value( "website" ) );
    account.setOwnership( data.value( "ownership" ) );
    account.setEmployees( data.value( "employees" ) );
    account.setTyckerSymbol( data.value( "tyckerSymbol" ) );
    account.setShippingAddressStreet( data.value( "shippingAddressStreet" ) );
    account.setShippingAddressCity( data.value( "shippingAddressCity" ) );
    account.setShippingAddressState( data.value( "shippingAddressState" ) );
    account.setShippingAddressPostalcode( data.value( "shippingAddressPostalcode" ) );
    account.setShippingAddressCountry( data.value( "shippingAddressCountry" ) );
    account.setEmail1( data.value( "email1" ) );
    account.setParentId( data.value( "parentId" ) );
    account.setParentName( data.value( "parentName" ) );
    account.setSicCode( data.value( "sicCode" ) );
    account.setCampaignId( data.value( "campaignId" ) );
    account.setCampaignName( data.value( "campaignName" ) );

    Item item;
    item.setMimeType( SugarAccount::mimeType() );
    item.setPayload<SugarAccount>( account );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob( item, mAccountsCollection );
    Q_UNUSED( job );

    disconnect( w->accountDetailsWidget(), SIGNAL( saveContact() ),
                 this, SLOT( slotAddContact( ) ) );
}

void AccountsPage::slotModifyAccount()
{
    const QModelIndex index = mUi.accountsTV->selectionModel()->currentIndex();
    Item item = mUi.accountsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    if ( item.isValid() ) {
        SugarAccount account;
        if ( item.hasPayload<SugarAccount>() ) {
            account = item.payload<SugarAccount>();
        }

        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        QMap<QString, QString> data;
        data = w->accountDetailsWidget()->accountData();

        account.setName( data.value( "name" ) );
        account.setDateEntered( data.value( "dateEntered" ) );
        account.setDateModified( data.value( "dateModified" ) );
        account.setModifiedUserId( data.value( "modifiedUserId" ) );
        account.setModifiedByName( data.value( "modifiedByName" ) );
        account.setCreatedBy( data.value( "createdBy" ) ); // id
        account.setCreatedByName( data.value( "createdByName" ) );
        account.setDescription( data.value( "description" ) );
        account.setDeleted( data.value( "deleted" ) );
        account.setAssignedUserId( data.value( "assignedUserId" ) );
        account.setAssignedUserName( data.value( "assignedUserName" ) );
        account.setAccountType( data.value( "accountType" ) );
        account.setIndustry( data.value( "industry" ) );
        account.setAnnualRevenue( data.value( "annualRevenue" ) );
        account.setPhoneFax( data.value( "phoneFax" ) );
        account.setBillingAddressStreet( data.value( "billingAddressStreet" ) );
        account.setBillingAddressCity( data.value( "billingAddressCity" ) );
        account.setBillingAddressState( data.value( "billingAddressState" ) );
        account.setBillingAddressPostalcode( data.value( "billingAddressPostalcode" ) );
        account.setBillingAddressCountry( data.value( "billingAddressCountry" ) );
        account.setRating( data.value( "rating" ) );
        account.setPhoneOffice( data.value( "phoneOffice" ) );
        account.setPhoneAlternate( data.value( "phoneAlternate" ) );
        account.setWebsite( data.value( "website" ) );
        account.setOwnership( data.value( "ownership" ) );
        account.setEmployees( data.value( "employees" ) );
        account.setTyckerSymbol( data.value( "tyckerSymbol" ) );
        account.setShippingAddressStreet( data.value( "shippingAddressStreet" ) );
        account.setShippingAddressCity( data.value( "shippingAddressCity" ) );
        account.setShippingAddressState( data.value( "shippingAddressState" ) );
        account.setShippingAddressPostalcode( data.value( "shippingAddressPostalcode" ) );
        account.setShippingAddressCountry( data.value( "shippingAddressCountry" ) );
        account.setEmail1( data.value( "email1" ) );
        account.setParentId( data.value( "parentId" ) );
        account.setParentName( data.value( "parentName" ) );
        account.setSicCode( data.value( "sicCode" ) );
        account.setCampaignId( data.value( "campaignId" ) );
        account.setCampaignName( data.value( "campaignName" ) );

        item.setPayload<SugarAccount>( account );

        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemModifyJob *job = new ItemModifyJob( item );
        Q_UNUSED( job );
    }
}

void AccountsPage::slotRemoveAccount()
{
    const QModelIndex index = mUi.accountsTV->selectionModel()->currentIndex();
    Item item = mUi.accountsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    if ( item.isValid() ) {
        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemDeleteJob *job = new ItemDeleteJob( item );
        Q_UNUSED( job );
    }
    const QModelIndex newIndex = mUi.accountsTV->selectionModel()->currentIndex();
    if ( !index.isValid() )
        mUi.removeAccountPB->setEnabled( false );
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

    AccountsTreeModel *accountsModel = new AccountsTreeModel( mChangeRecorder, this );

    AccountsTreeModel::Columns columns;
    columns << AccountsTreeModel::Name
            << AccountsTreeModel::City
            << AccountsTreeModel::Country
            << AccountsTreeModel::Phone
            << AccountsTreeModel::Email
            << AccountsTreeModel::CreatedBy;
    accountsModel->setColumns( columns );

    // same as for the ContactsTreeModel, not strictly necessary
    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel( this );
    filterModel->setSourceModel( accountsModel );
    filterModel->addMimeTypeInclusionFilter( SugarAccount::mimeType() );
    filterModel->setHeaderGroup( EntityTreeModel::ItemListHeaders );

    mUi.accountsTV->setModel( filterModel );

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
