#include "campaignspage.h"

#include "campaignstreemodel.h"
#include "campaignsfilterproxymodel.h"
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

#include "kdcrmdata/sugarcampaign.h"

using namespace Akonadi;

CampaignsPage::CampaignsPage( QWidget *parent )
    : QWidget( parent ),
      mChangeRecorder( new ChangeRecorder( this ) )
{
    mUi.setupUi( this );
    initialize();
}

CampaignsPage::~CampaignsPage()
{
}

void CampaignsPage::slotResourceSelectionChanged( const QByteArray &identifier )
{
    if ( mCampaignsCollection.isValid() ) {
        mChangeRecorder->setCollectionMonitored( mCampaignsCollection, false );
    }

    /*
     * Look for the "Campaigns" collection explicitly by listing all collections
     * of the currently selected resource, filtering by MIME type.
     * include statistics to get the number of items in each collection
     */
    CollectionFetchJob *job = new CollectionFetchJob( mCampaignsCollection.root(), CollectionFetchJob::Recursive );
    job->fetchScope().setResource( identifier );
    job->fetchScope().setContentMimeTypes( QStringList() << SugarCampaign::mimeType() );
    job->fetchScope().setIncludeStatistics( true );
    connect( job, SIGNAL( result( KJob* ) ),
             this, SLOT( slotCollectionFetchResult( KJob* ) ) );
}

void CampaignsPage::slotCollectionFetchResult( KJob *job )
{

    CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob*>( job );

    // look for the "Campaigns" collection
    Q_FOREACH( const Collection &collection, fetchJob->collections() ) {
        if ( collection.remoteId() == QLatin1String( "Campaigns" ) ) {
            mCampaignsCollection = collection;
            break;
        }
    }

    if ( mCampaignsCollection.isValid() ) {
        mUi.newCampaignPB->setEnabled( true );
        mChangeRecorder->setCollectionMonitored( mCampaignsCollection, true );
        // if empty, the collection might not have been loaded yet, try synchronizing
        if ( mCampaignsCollection.statistics().count() == 0 ) {
            AgentManager::self()->synchronizeCollection( mCampaignsCollection );
        }

        setupCachePolicy();
    } else {
        mUi.newCampaignPB->setEnabled( false );
    }
}

void CampaignsPage::slotCampaignChanged( const Item &item )
{

    if ( item.isValid() && item.hasPayload<SugarCampaign>() ) {
        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        if ( w ) {
            w->campaignDetailsWidget()->setItem( item );
            connect( w->campaignDetailsWidget(), SIGNAL( modifyCampaign() ),
                 this, SLOT( slotModifyCampaign( ) ) );
        }
        emit campaignItemChanged();
    }
}

void CampaignsPage::slotNewCampaignClicked()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w ) {
        CampaignDetails *cd = w->campaignDetailsWidget();
        cd->clearFields();
        connect( cd, SIGNAL( saveCampaign() ),
                 this, SLOT( slotAddCampaign( ) ) );
    }

    emit showDetails();
}

void CampaignsPage::slotAddCampaign()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    QMap<QString, QString> data;
    data = w->campaignDetailsWidget()->campaignData();
    SugarCampaign campaign;
    campaign.setName( data.value( "name" ) );
    campaign.setDateEntered( data.value( "dateEntered" ) );
    campaign.setDateModified( data.value( "dateModified" ) );
    campaign.setModifiedUserId( data.value( "modifiedUserId" ) );
    campaign.setModifiedByName( data.value( "modifiedByName" ) );
    campaign.setCreatedBy( data.value( "createdBy" ) ); // id
    campaign.setCreatedByName( data.value( "createdByName" ) );
    campaign.setDeleted( data.value( "deleted" ) );
    campaign.setAssignedUserId( data.value( "assignedUserId" ) );
    campaign.setAssignedUserName( data.value( "assignedUserName" ) );
    campaign.setTrackerKey( data.value( "trackerKey" ) );
    campaign.setTrackerCount( data.value( "trackerCount" ) );
    campaign.setReferUrl( data.value( "referUrl" ) );
    campaign.setTrackerText( data.value( "trackerText" ) );
    campaign.setStartDate( data.value( "startDate" ) );
    campaign.setEndDate( data.value( "endDate" ) );
    campaign.setStatus( data.value( "status" ) );
    campaign.setImpressions( data.value( "impressions" ) );
    campaign.setCurrencyId( data.value( "currencyId" ) );
    campaign.setBudget( data.value( "budget" ) );
    campaign.setExpectedCost( data.value( "expectedCost" ) );
    campaign.setActualCost( data.value( "actualCost" ) );
    campaign.setExpectedRevenue( data.value( "expectedRevenue" ) );
    campaign.setCampaignType( data.value( "campaignType" ) );
    campaign.setObjective( data.value( "objective" ) );
    campaign.setContent( data.value( "content" ) );
    campaign.setFrequency( data.value( "frequency" ) );

    Item item;
    item.setMimeType( SugarCampaign::mimeType() );
    item.setPayload<SugarCampaign>( campaign );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob( item, mCampaignsCollection );
    Q_UNUSED( job );

    disconnect( w->campaignDetailsWidget(), SIGNAL( saveCampaign() ),
                 this, SLOT( slotAddCampaign( ) ) );
}

void CampaignsPage::slotModifyCampaign()
{
    const QModelIndex index = mUi.campaignsTV->selectionModel()->currentIndex();
    Item item = mUi.campaignsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    if ( item.isValid() ) {
        SugarCampaign campaign;
        if ( item.hasPayload<SugarCampaign>() ) {
            campaign = item.payload<SugarCampaign>();
        }

        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        QMap<QString, QString> data;
        data = w->campaignDetailsWidget()->campaignData();

        campaign.setName( data.value( "name" ) );
        campaign.setDateEntered( data.value( "dateEntered" ) );
        campaign.setDateModified( data.value( "dateModified" ) );
        campaign.setModifiedUserId( data.value( "modifiedUserId" ) );
        campaign.setModifiedByName( data.value( "modifiedByName" ) );
        campaign.setCreatedBy( data.value( "createdBy" ) ); // id
        campaign.setCreatedByName( data.value( "createdByName" ) );
        campaign.setDeleted( data.value( "deleted" ) );
        campaign.setAssignedUserId( data.value( "assignedUserId" ) );
        campaign.setAssignedUserName( data.value( "assignedUserName" ) );
        campaign.setTrackerKey( data.value( "trackerKey" ) );
        campaign.setTrackerCount( data.value( "trackerCount" ) );
        campaign.setReferUrl( data.value( "referUrl" ) );
        campaign.setTrackerText( data.value( "trackerText" ) );
        campaign.setStartDate( data.value( "startDate" ) );
        campaign.setEndDate( data.value( "endDate" ) );
        campaign.setStatus( data.value( "status" ) );
        campaign.setImpressions( data.value( "impressions" ) );
        campaign.setCurrencyId( data.value( "currencyId" ) );
        campaign.setBudget( data.value( "budget" ) );
        campaign.setExpectedCost( data.value( "expectedCost" ) );
        campaign.setActualCost( data.value( "actualCost" ) );
        campaign.setExpectedRevenue( data.value( "expectedRevenue" ) );
        campaign.setCampaignType( data.value( "campaignType" ) );
        campaign.setObjective( data.value( "objective" ) );
        campaign.setContent( data.value( "content" ) );
        item.setPayload<SugarCampaign>( campaign );

        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemModifyJob *job = new ItemModifyJob( item );
        Q_UNUSED( job );
    }
}

void CampaignsPage::slotRemoveCampaign()
{
    const QModelIndex index = mUi.campaignsTV->selectionModel()->currentIndex();
    Item item = mUi.campaignsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    removeCampaignsData( item );

    if ( item.isValid() ) {
        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemDeleteJob *job = new ItemDeleteJob( item );
        Q_UNUSED( job );
    }
    const QModelIndex newIndex = mUi.campaignsTV->selectionModel()->currentIndex();
    if ( !index.isValid() )
        mUi.removeCampaignPB->setEnabled( false );


}

void CampaignsPage::slotSetCurrent( const QModelIndex& index, int start, int end )
{
        if ( start == end ) {
            QModelIndex newIdx = mUi.campaignsTV->model()->index(start, 0, index);
            mUi.campaignsTV->setCurrentIndex( newIdx );
        }
        //model items are loaded
        if ( mUi.campaignsTV->model()->rowCount() == mCampaignsCollection.statistics().count() )
            addCampaignsData();
}

void CampaignsPage::removeCampaignsData( const Item &item )
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    AccountDetails *accountDetails = w->accountDetailsWidget();
    CampaignDetails *campaignDetails = w->campaignDetailsWidget();
    ContactDetails *contactDetails = w->contactDetailsWidget();
    OpportunityDetails *opportunityDetails = w->opportunityDetailsWidget();
    LeadDetails *leadDetails = w->leadDetailsWidget();

    if ( item.hasPayload<SugarCampaign>() ) {
        SugarCampaign campaign;
        campaign = item.payload<SugarCampaign>();
        accountDetails->removeCampaignData( campaign.name() );
        contactDetails->removeCampaignData( campaign.name() );
        opportunityDetails->removeCampaignData( campaign.name() );
        leadDetails->removeCampaignData( campaign.name() );
        campaignDetails->removeCampaignData( campaign.name() );
    }
}

void CampaignsPage::addCampaignsData()
{
    // Pending (Michel) - clean up
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    AccountDetails *accountDetails = w->accountDetailsWidget();
    CampaignDetails *campaignDetails = w->campaignDetailsWidget();
    ContactDetails *contactDetails = w->contactDetailsWidget();
    OpportunityDetails *opportunityDetails = w->opportunityDetailsWidget();
    LeadDetails *leadDetails = w->leadDetailsWidget();

    QModelIndex index;
    Item item;
    SugarCampaign campaign;
    for ( int i = 0; i <  mUi.campaignsTV->model()->rowCount(); ++i ) {
       index  =  mUi.campaignsTV->model()->index( i, 0 );
       item = mUi.campaignsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
       if ( item.hasPayload<SugarCampaign>() ) {
           campaign = item.payload<SugarCampaign>();
           accountDetails->addCampaignData( campaign.name(), campaign.id() );
           contactDetails->addCampaignData( campaign.name(), campaign.id() );
           opportunityDetails->addCampaignData( campaign.name(), campaign.id() );
           leadDetails->addCampaignData( campaign.name(), campaign.id() );
           campaignDetails->addCampaignData( campaign.name(), campaign.id() );
           // code below should be executed from
           //its own pages when implemented
           campaignDetails->addAssignedToData( campaign.assignedUserName(), campaign.assignedUserId() );
       }
    }
}

void CampaignsPage::initialize()
{
    mUi.campaignsTV->header()->setResizeMode( QHeaderView::ResizeToContents );

    connect( mUi.newCampaignPB, SIGNAL( clicked() ),
             this, SLOT( slotNewCampaignClicked() ) );
    connect( mUi.removeCampaignPB, SIGNAL( clicked() ),
             this, SLOT( slotRemoveCampaign() ) );

    // automatically get the full data when items change
    mChangeRecorder->itemFetchScope().fetchFullPayload( true );

    CampaignsTreeModel *campaignsModel = new CampaignsTreeModel( mChangeRecorder, this );

    CampaignsTreeModel::Columns columns;
    columns << CampaignsTreeModel::Campaign
            << CampaignsTreeModel::Status
            << CampaignsTreeModel::Type
            << CampaignsTreeModel::EndDate
            << CampaignsTreeModel::User;
    campaignsModel->setColumns( columns );

    // same as for the ContactsTreeModel, not strictly necessary
    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel( this );
    filterModel->setSourceModel( campaignsModel );
    filterModel->addMimeTypeInclusionFilter( SugarCampaign::mimeType() );
    filterModel->setHeaderGroup( EntityTreeModel::ItemListHeaders );

    CampaignsFilterProxyModel *filter = new CampaignsFilterProxyModel( this );
    filter->setSourceModel( filterModel );
    mUi.campaignsTV->setModel( filter );

    connect( mUi.searchLE, SIGNAL( textChanged( const QString& ) ),
             filter, SLOT( setFilterString( const QString& ) ) );

    connect( mUi.campaignsTV, SIGNAL( currentChanged( Akonadi::Item ) ), this, SLOT( slotCampaignChanged( Akonadi::Item ) ) );

    connect( mUi.campaignsTV->model(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), SLOT( slotSetCurrent( const QModelIndex&,int,int ) ) );

}

void CampaignsPage::syncronize()
{
    AgentManager::self()->synchronizeCollection( mCampaignsCollection );
}

void CampaignsPage::cachePolicyJobCompleted( KJob* job)
{
    if ( job->error() )
        emit statusMessage( tr("Error when setting cachepolicy: %1").arg( job->errorString() ) );
    else
        emit statusMessage( tr("Cache policy set") );

}

void CampaignsPage::setupCachePolicy()
{
    CachePolicy policy;
    policy.setIntervalCheckTime( 1 ); // Check for new data every minute
    policy.setInheritFromParent( false );
    mCampaignsCollection.setCachePolicy( policy );
    CollectionModifyJob *job = new CollectionModifyJob( mCampaignsCollection );
    connect( job, SIGNAL( result( KJob* ) ), this, SLOT( cachePolicyJobCompleted( KJob* ) ) );
}

