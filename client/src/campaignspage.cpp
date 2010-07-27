#include "campaignspage.h"

#include "campaignstreemodel.h"
#include "campaignsfilterproxymodel.h"
#include "sugarclient.h"
#include "enums.h"

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

#include <QMessageBox>

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
    CollectionFetchJob *job = new CollectionFetchJob( Collection::root(), CollectionFetchJob::Recursive );
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

void CampaignsPage::slotCampaignClicked( const QModelIndex &index )
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w ) {
        CampaignDetails *cd = dynamic_cast<CampaignDetails*>( w->detailsWidget(Campaign) );

        if ( cd->isEditing() ) {
            if ( !proceedIsOk() ) {
                mUi.campaignsTV->setCurrentIndex( mCurrentIndex );
                return;
            }
        }
        Item item = mUi.campaignsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
        campaignChanged(item );
    }
}

void CampaignsPage::campaignChanged( const Item &item )
{

    if ( item.isValid() && item.hasPayload<SugarCampaign>() ) {
        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        if ( w ) {
            CampaignDetails *cd = dynamic_cast<CampaignDetails*>( w->detailsWidget( Campaign ) );
            cd->setItem( item );
            mCurrentIndex = mUi.campaignsTV->selectionModel()->currentIndex();
            connect( cd, SIGNAL( modifyCampaign() ),
                 this, SLOT( slotModifyCampaign( ) ) );
        }
    }
}

void CampaignsPage::slotNewCampaignClicked()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w ) {
        CampaignDetails *cd = dynamic_cast<CampaignDetails*>( w->detailsWidget( Campaign ) );
        if ( cd->isEditing() ) {
            if ( !proceedIsOk() )
                return;
        }
        w->displayDockWidgets();
        cd->clearFields();
        connect( cd, SIGNAL( saveCampaign() ),
                 this, SLOT( slotAddCampaign( ) ) );
        // reset
        cd->initialize();
    }
}

void CampaignsPage::slotAddCampaign()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    CampaignDetails *cd = dynamic_cast<CampaignDetails*>( w->detailsWidget( Campaign ) );
    QMap<QString, QString> data;
    data = cd->campaignData();
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
    updateCampaignCombo( campaign.name(), campaign.id() );
    disconnect( cd, SIGNAL( saveCampaign() ),
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
        CampaignDetails *cd = dynamic_cast<CampaignDetails*>( w->detailsWidget( Campaign ) );
        QMap<QString, QString> data;
        data = cd->campaignData();

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
        updateCampaignCombo( campaign.name(), campaign.id() );
        cd->initialize();
    }
}

void CampaignsPage::slotRemoveCampaign()
{
    const QModelIndex index = mUi.campaignsTV->selectionModel()->currentIndex();
    if ( !index.isValid() )
        return;

    Item item = mUi.campaignsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    QMessageBox msgBox;
    msgBox.setWindowTitle( tr( "SugarClient - Delete Campaign" ) );
    msgBox.setText( QString( "The selected item will be removed permanentely!" ) );
    msgBox.setInformativeText( tr( "Are you sure you want to delete it?" ) );
    msgBox.setStandardButtons( QMessageBox::Yes |
                               QMessageBox::Cancel );
    msgBox.setDefaultButton( QMessageBox::Cancel );
    int ret = msgBox.exec();
    if ( ret == QMessageBox::Cancel )
        return;

    removeCampaignsData( item );

    if ( item.isValid() ) {
        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemDeleteJob *job = new ItemDeleteJob( item );
        Q_UNUSED( job );
    }
    const QModelIndex newIndex = mUi.campaignsTV->selectionModel()->currentIndex();
    if ( !newIndex.isValid() )
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
    AccountDetails *ad = dynamic_cast<AccountDetails*>( w->detailsWidget( Account ) );
    CampaignDetails *cad = dynamic_cast<CampaignDetails*>( w->detailsWidget( Campaign ) );
    ContactDetails *cod = dynamic_cast<ContactDetails*>( w->detailsWidget( Contact ) );
    OpportunityDetails *od = dynamic_cast<OpportunityDetails*>( w->detailsWidget(Opportunity ) );
    LeadDetails *ld = dynamic_cast<LeadDetails*>( w->detailsWidget( Lead ) );

    if ( item.hasPayload<SugarCampaign>() ) {
        SugarCampaign campaign;
        campaign = item.payload<SugarCampaign>();
        ad->removeCampaignData( campaign.name() );
        cod->removeCampaignData( campaign.name() );
        od->removeCampaignData( campaign.name() );
        ld->removeCampaignData( campaign.name() );
        cad->removeCampaignData( campaign.name() );
    }
}

void CampaignsPage::addCampaignsData()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    AccountDetails *ad = dynamic_cast<AccountDetails*>( w->detailsWidget( Account ) );
    CampaignDetails *cad = dynamic_cast<CampaignDetails*>( w->detailsWidget( Campaign ) );
    ContactDetails *cod = dynamic_cast<ContactDetails*>( w->detailsWidget( Contact ) );
    OpportunityDetails *od = dynamic_cast<OpportunityDetails*>( w->detailsWidget(Opportunity ) );
    LeadDetails *ld = dynamic_cast<LeadDetails*>( w->detailsWidget( Lead ) );

    QModelIndex index;
    Item item;
    SugarCampaign campaign;
    for ( int i = 0; i <  mUi.campaignsTV->model()->rowCount(); ++i ) {
       index  =  mUi.campaignsTV->model()->index( i, 0 );
       item = mUi.campaignsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
       if ( item.hasPayload<SugarCampaign>() ) {
           campaign = item.payload<SugarCampaign>();
           ad->addCampaignData( campaign.name(), campaign.id() );
           cod->addCampaignData( campaign.name(), campaign.id() );
           od->addCampaignData( campaign.name(), campaign.id() );
           ld->addCampaignData( campaign.name(), campaign.id() );
           cad->addCampaignData( campaign.name(), campaign.id() );
           // code below should be executed from
           //its own pages when implemented
           cad->addAssignedToData( campaign.assignedUserName(), campaign.assignedUserId() );
       }
    }
}

void CampaignsPage::updateCampaignCombo( const QString& name, const QString& id )
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );

    AccountDetails *ad =
        dynamic_cast<AccountDetails*>( w->detailsWidget( Account ) );
    CampaignDetails *cad =
        dynamic_cast<CampaignDetails*>( w->detailsWidget( Campaign ) );
    ContactDetails *cod =
        dynamic_cast<ContactDetails*>( w->detailsWidget( Contact ) );
    OpportunityDetails *od =
        dynamic_cast<OpportunityDetails*>( w->detailsWidget(Opportunity ) );
    LeadDetails *ld =
        dynamic_cast<LeadDetails*>( w->detailsWidget( Lead ) );

    ad->addCampaignData( name, id );
    cod->addCampaignData( name, id );
    od->addCampaignData( name, id );
    ld->addCampaignData( name, id );
    cad->addCampaignData( name, id );
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

    connect( mUi.campaignsTV, SIGNAL( clicked( const QModelIndex& ) ),
             this, SLOT( slotCampaignClicked( const QModelIndex& ) ) );

    connect( mUi.campaignsTV->model(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), SLOT( slotSetCurrent( const QModelIndex&,int,int ) ) );

    connect( mUi.campaignsTV->model(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotUpdateItemDetails( const QModelIndex&, const QModelIndex& ) ) );
}

void CampaignsPage::syncronize()
{
    if ( mUi.campaignsTV->model() != 0
         && mUi.campaignsTV->model()->rowCount() > 0 )
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

void CampaignsPage::slotUpdateItemDetails( const QModelIndex& topLeft, const QModelIndex& bottomRight )
{
    Q_UNUSED( bottomRight );
    Item item;
    SugarCampaign campaign;
    item = mUi.campaignsTV->model()->data( topLeft, EntityTreeModel::ItemRole ).value<Item>();
    campaignChanged( item );
}

bool CampaignsPage::proceedIsOk()
{
    bool proceed = true;
    QMessageBox msgBox;
    msgBox.setText( tr( "The current item has been modified." ) );
    msgBox.setInformativeText( tr( "Do you want to save your changes?" ) );
    msgBox.setStandardButtons( QMessageBox::Save |
                               QMessageBox::Discard );
    msgBox.setDefaultButton( QMessageBox::Save );
    int ret = msgBox.exec();
    if ( ret == QMessageBox::Save )
        proceed = false;
    return proceed;
}

void CampaignsPage::slotSetItem()
{
    mCurrentIndex  = mUi.campaignsTV->selectionModel()->currentIndex();
    if ( mCurrentIndex.isValid() )
        slotCampaignClicked( mCurrentIndex );
    else
        slotNewCampaignClicked();
}
