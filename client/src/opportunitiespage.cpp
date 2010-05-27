#include "opportunitiespage.h"
#include "opportunitiestreemodel.h"
#include "opportunitiesfilterproxymodel.h"
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

#include "kdcrmdata/sugaropportunity.h"

using namespace Akonadi;

OpportunitiesPage::OpportunitiesPage( QWidget *parent )
    : QWidget( parent ),
      mChangeRecorder( new ChangeRecorder( this ) )
{
    mUi.setupUi( this );
    initialize();
}

OpportunitiesPage::~OpportunitiesPage()
{
}

void OpportunitiesPage::slotResourceSelectionChanged( const QByteArray &identifier )
{
    if ( mOpportunitiesCollection.isValid() ) {
        mChangeRecorder->setCollectionMonitored( mOpportunitiesCollection, false );
    }

    /*
     * Look for the "Opportunities" collection explicitly by listing all collections
     * of the currently selected resource, filtering by MIME type.
     * include statistics to get the number of items in each collection
     */
    CollectionFetchJob *job = new CollectionFetchJob( Collection::root(), CollectionFetchJob::Recursive );
    job->fetchScope().setResource( identifier );
    job->fetchScope().setContentMimeTypes( QStringList() << SugarOpportunity::mimeType() );
    job->fetchScope().setIncludeStatistics( true );
    connect( job, SIGNAL( result( KJob* ) ),
             this, SLOT( slotCollectionFetchResult( KJob* ) ) );
}

void OpportunitiesPage::slotCollectionFetchResult( KJob *job )
{

    CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob*>( job );

    // look for the "Opportunities" collection
    int i = 0;
    Q_FOREACH( const Collection &collection, fetchJob->collections() ) {
        if ( collection.remoteId() == QLatin1String( "Opportunities" ) ) {
            mOpportunitiesCollection = collection;
            i++;
            break;
        }
    }

    if ( mOpportunitiesCollection.isValid() ) {
        mUi.newOpportunityPB->setEnabled( true );
        mChangeRecorder->setCollectionMonitored( mOpportunitiesCollection, true );
        // if empty, the collection might not have been loaded yet, try synchronizing
        if ( mOpportunitiesCollection.statistics().count() == 0 ) {
            AgentManager::self()->synchronizeCollection( mOpportunitiesCollection );
        }

        setupCachePolicy();
    } else {
        mUi.newOpportunityPB->setEnabled( false );
    }
}

void OpportunitiesPage::slotOpportunityChanged( const Item &item )
{

    if ( item.isValid() && item.hasPayload<SugarOpportunity>() ) {
        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        if ( w ) {
            w->opportunityDetailsWidget()->setItem( item );
            connect( w->opportunityDetailsWidget(), SIGNAL( modifyOpportunity() ),
                 this, SLOT( slotModifyOpportunity( ) ) );
        }
        emit opportunityItemChanged();
    }
}

void OpportunitiesPage::slotNewOpportunityClicked()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w ) {
        OpportunityDetails *cd = w->opportunityDetailsWidget();
        cd->clearFields();
        connect( cd, SIGNAL( saveOpportunity() ),
                 this, SLOT( slotAddOpportunity( ) ) );
    }

    emit showDetails();
}

void OpportunitiesPage::slotAddOpportunity()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    QMap<QString, QString> data;
    data = w->opportunityDetailsWidget()->opportunityData();
    SugarOpportunity opportunity;
    opportunity.setName( data.value( "name" ) );
    opportunity.setDateEntered( data.value( "dateEntered" ) );
    opportunity.setDateModified( data.value( "dateModified" ) );
    opportunity.setModifiedUserId( data.value( "modifiedUserId" ) );
    opportunity.setModifiedByName( data.value( "modifiedByName" ) );
    opportunity.setCreatedBy( data.value( "createdBy" ) ); // id
    opportunity.setCreatedByName( data.value( "createdByName" ) );
    opportunity.setDescription( data.value( "description" ) );
    opportunity.setDeleted( data.value( "deleted" ) );
    opportunity.setAssignedUserId( data.value( "assignedUserId" ) );
    opportunity.setAssignedUserName( data.value( "assignedUserName" ) );
    opportunity.setOpportunityType( data.value( "opportunityType" ) );
    // Pending(michel)
    opportunity.setAccountName( data.value( "accountName" ) );
    opportunity.setAccountId( data.value( "accountId" ) );
    opportunity.setCampaignId( data.value( "campaignId" ) );
    opportunity.setCampaignName( data.value( "campaignName" ) );
    opportunity.setLeadSource( data.value( "leadSource" ) );
    opportunity.setAmount( data.value( "amount" ) );
    opportunity.setAmountUsDollar( data.value( "amountUsDollar" ) );
    opportunity.setCurrencyId( data.value( "currencyId" ) );
    opportunity.setCurrencyName( data.value( "currencyName" ) );
    opportunity.setCurrencySymbol( data.value( "currencySymbol" ) );
    opportunity.setDateClosed( data.value( "dateClosed" ) );
    opportunity.setNextStep( data.value( "nextStep" ) );
    opportunity.setSalesStage( data.value( "salesStage" ) );
    opportunity.setProbability( data.value( "probability" ) );

    Item item;
    item.setMimeType( SugarOpportunity::mimeType() );
    item.setPayload<SugarOpportunity>( opportunity );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob( item, mOpportunitiesCollection );
    Q_UNUSED( job );

    disconnect( w->opportunityDetailsWidget(), SIGNAL( saveOpportunity() ),
                 this, SLOT( slotAddOpportunity( ) ) );
}

void OpportunitiesPage::slotModifyOpportunity()
{
    const QModelIndex index = mUi.opportunitiesTV->selectionModel()->currentIndex();
    Item item = mUi.opportunitiesTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    if ( item.isValid() ) {
        SugarOpportunity opportunity;
        if ( item.hasPayload<SugarOpportunity>() ) {
            opportunity = item.payload<SugarOpportunity>();
        }

        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        QMap<QString, QString> data;
        data = w->opportunityDetailsWidget()->opportunityData();

        opportunity.setName( data.value( "name" ) );
        opportunity.setDateEntered( data.value( "dateEntered" ) );
        opportunity.setDateModified( data.value( "dateModified" ) );
        opportunity.setModifiedUserId( data.value( "modifiedUserId" ) );
        opportunity.setModifiedByName( data.value( "modifiedByName" ) );
        opportunity.setCreatedBy( data.value( "createdBy" ) ); // id
        opportunity.setCreatedByName( data.value( "createdByName" ) );
        opportunity.setDescription( data.value( "description" ) );
        opportunity.setDeleted( data.value( "deleted" ) );
        opportunity.setAssignedUserId( data.value( "assignedUserId" ) );
        opportunity.setAssignedUserName( data.value( "assignedUserName" ) );
        opportunity.setOpportunityType( data.value( "opportunityType" ) );
        opportunity.setAccountName( data.value( "accountName" ) );
        opportunity.setAccountId( data.value( "accountId" ) );
        qDebug() << "value pour id " << data.value( "accountId" );
        opportunity.setCampaignId( data.value( "campaignId" ) );
        opportunity.setCampaignName( data.value( "campaignName" ) );
        opportunity.setLeadSource( data.value( "leadSource" ) );
        opportunity.setAmount( data.value( "amount" ) );
        opportunity.setAmountUsDollar( data.value( "amountUsDollar" ) );
        opportunity.setCurrencyId( data.value( "currencyId" ) );
        opportunity.setCurrencyName( data.value( "currencyName" ) );
        opportunity.setCurrencySymbol( data.value( "currencySymbol" ) );
        opportunity.setDateClosed( data.value( "dateClosed" ) );
        opportunity.setNextStep( data.value( "nextStep" ) );
        opportunity.setSalesStage( data.value( "salesStage" ) );
        opportunity.setProbability( data.value( "probability" ) );

        item.setPayload<SugarOpportunity>( opportunity );

        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemModifyJob *job = new ItemModifyJob( item );
        Q_UNUSED( job );
    }
}

void OpportunitiesPage::slotRemoveOpportunity()
{
    const QModelIndex index = mUi.opportunitiesTV->selectionModel()->currentIndex();
    Item item = mUi.opportunitiesTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    if ( item.isValid() ) {
        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemDeleteJob *job = new ItemDeleteJob( item );
        Q_UNUSED( job );
    }
    const QModelIndex newIndex = mUi.opportunitiesTV->selectionModel()->currentIndex();
    if ( !index.isValid() )
        mUi.removeOpportunityPB->setEnabled( false );
}

void OpportunitiesPage::slotSetCurrent( const QModelIndex& index, int start, int end )
{
        if ( start == end ) {
            QModelIndex newIdx = mUi.opportunitiesTV->model()->index(start, 0, index);
            mUi.opportunitiesTV->setCurrentIndex( newIdx );
        }
        //model items are loaded
        if ( mUi.opportunitiesTV->model()->rowCount() == mOpportunitiesCollection.statistics().count() )
            addOpportunitiesData();
}

void OpportunitiesPage::addOpportunitiesData()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    OpportunityDetails *od = w->opportunityDetailsWidget();
    QModelIndex index;
    Item item;
    SugarOpportunity opportunity;
    for ( int i = 0; i <  mUi.opportunitiesTV->model()->rowCount(); ++i ) {
       index  =  mUi.opportunitiesTV->model()->index( i, 0 );
       item = mUi.opportunitiesTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
       if ( item.hasPayload<SugarOpportunity>() ) {
           opportunity = item.payload<SugarOpportunity>();
           // code below should be executed from
           // their own pages when implemented
           od->addCampaignData( opportunity.campaignName(), opportunity.campaignId() );
           od->addAssignedToData( opportunity.assignedUserName(), opportunity.assignedUserId() );
       }
    }
    // fill opportunities combo
    od->fillCombos();
}

void OpportunitiesPage::initialize()
{
    mUi.opportunitiesTV->header()->setResizeMode( QHeaderView::ResizeToContents );

    connect( mUi.newOpportunityPB, SIGNAL( clicked() ),
             this, SLOT( slotNewOpportunityClicked() ) );
    connect( mUi.removeOpportunityPB, SIGNAL( clicked() ),
             this, SLOT( slotRemoveOpportunity() ) );

    // automatically get the full data when items change
    mChangeRecorder->itemFetchScope().fetchFullPayload( true );

    OpportunitiesTreeModel *opportunitiesModel = new OpportunitiesTreeModel( mChangeRecorder, this );

    OpportunitiesTreeModel::Columns columns;
    columns << OpportunitiesTreeModel::Name
            << OpportunitiesTreeModel::AccountName
            << OpportunitiesTreeModel::SalesStage
            << OpportunitiesTreeModel::Amount
            << OpportunitiesTreeModel::Close
            << OpportunitiesTreeModel::AssignedTo;
    opportunitiesModel->setColumns( columns );

    // same as for the ContactsTreeModel, not strictly necessary
    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel( this );
    filterModel->setSourceModel( opportunitiesModel );
    filterModel->addMimeTypeInclusionFilter( SugarOpportunity::mimeType() );
    filterModel->setHeaderGroup( EntityTreeModel::ItemListHeaders );

    OpportunitiesFilterProxyModel *filter = new OpportunitiesFilterProxyModel( this );
    filter->setSourceModel( filterModel );
    mUi.opportunitiesTV->setModel( filter );

    connect( mUi.searchLE, SIGNAL( textChanged( const QString& ) ),
             filter, SLOT( setFilterString( const QString& ) ) );

    connect( mUi.opportunitiesTV, SIGNAL( currentChanged( Akonadi::Item ) ), this, SLOT( slotOpportunityChanged( Akonadi::Item ) ) );

    connect( mUi.opportunitiesTV->model(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), SLOT( slotSetCurrent( const QModelIndex&,int,int ) ) );

}

void OpportunitiesPage::syncronize()
{
    AgentManager::self()->synchronizeCollection( mOpportunitiesCollection );
}

void OpportunitiesPage::cachePolicyJobCompleted( KJob* job)
{
    if ( job->error() )
        emit statusMessage( tr("Error when setting cachepolicy: %1").arg( job->errorString() ) );
    else
        emit statusMessage( tr("Cache policy set") );

}

void OpportunitiesPage::setupCachePolicy()
{
    CachePolicy policy;
    policy.setIntervalCheckTime( 1 ); // Check for new data every minute
    policy.setInheritFromParent( false );
    mOpportunitiesCollection.setCachePolicy( policy );
    CollectionModifyJob *job = new CollectionModifyJob( mOpportunitiesCollection );
    connect( job, SIGNAL( result( KJob* ) ), this, SLOT( cachePolicyJobCompleted( KJob* ) ) );
}

void OpportunitiesPage::slotFilterChanged( const QString& filterString )
{
    Q_UNUSED( filterString );
}

