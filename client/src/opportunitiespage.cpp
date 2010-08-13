#include "opportunitiespage.h"
#include "opportunitiestreemodel.h"
#include "opportunitiesfilterproxymodel.h"
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

#include "kdcrmdata/sugaropportunity.h"

#include <QMessageBox>

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
    Q_FOREACH( const Collection &collection, fetchJob->collections() ) {
        if ( collection.remoteId() == QLatin1String( "Opportunities" ) ) {
            mOpportunitiesCollection = collection;
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

void OpportunitiesPage::slotOpportunityClicked( const QModelIndex &index )
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w ) {
        OpportunityDetails *od = dynamic_cast<OpportunityDetails*>( w->detailsWidget(Opportunity) );

        if ( od->isEditing() ) {
            if ( !proceedIsOk() ) {
                mUi.opportunitiesTV->setCurrentIndex( mCurrentIndex );
                return;
            }
        }
        Item item = mUi.opportunitiesTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
        opportunityChanged(item );
    }
}

void OpportunitiesPage::opportunityChanged( const Item &item )
{
    if ( item.isValid() && item.hasPayload<SugarOpportunity>() ) {
        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        OpportunityDetails *od = dynamic_cast<OpportunityDetails*>( w->detailsWidget( Opportunity ) );
        od->setItem( item );
        mCurrentIndex = mUi.opportunitiesTV->selectionModel()->currentIndex();
        connect( od, SIGNAL( modifyOpportunity() ),
                 this, SLOT( slotModifyOpportunity( ) ) );
    }
}

void OpportunitiesPage::slotNewOpportunityClicked()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w ) {
        OpportunityDetails *od = dynamic_cast<OpportunityDetails*>( w->detailsWidget( Opportunity ) );

        if ( od->isEditing() ) {
            if ( !proceedIsOk() )
                return;
        }

        w->displayDockWidgets();
        od->clearFields();
        connect( od, SIGNAL( saveOpportunity() ),
                 this, SLOT( slotAddOpportunity( ) ) );
        od->initialize();
    }
}

void OpportunitiesPage::slotAddOpportunity()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    OpportunityDetails *od = dynamic_cast<OpportunityDetails*>( w->detailsWidget(Opportunity ) );
    QMap<QString, QString> data;
    data = od->opportunityData();
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
    w->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );

    disconnect( od, SIGNAL( saveOpportunity() ),
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
        OpportunityDetails *od = dynamic_cast<OpportunityDetails*>( w->detailsWidget( Opportunity ) );

        disconnect( od, SIGNAL( modifyOpportunity() ),
               this, SLOT( slotModifyOpportunity() ) );

        QMap<QString, QString> data;
        data = od->opportunityData();

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
        item.setRemoteRevision( data.value( "remoteRevision" ) );
        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemModifyJob *job = new ItemModifyJob( item );
        if ( !job->exec() )
            return; //qDebug() << "Error:" << job->errorString();

        w->setEnabled( false );
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
        emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
        od->reset();
    }
}

void OpportunitiesPage::slotRemoveOpportunity()
{
    const QModelIndex index = mUi.opportunitiesTV->selectionModel()->currentIndex();
    if ( !index.isValid() )
        return;

    Item item = mUi.opportunitiesTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    QMessageBox msgBox;
    msgBox.setWindowTitle( tr( "SugarClient - Delete Opportunity" ) );
    msgBox.setText( QString( "The selected item will be removed permanentely!" ) );
    msgBox.setInformativeText( tr( "Are you sure you want to delete it?" ) );
    msgBox.setStandardButtons( QMessageBox::Yes |
                               QMessageBox::Cancel );
    msgBox.setDefaultButton( QMessageBox::Cancel );
    int ret = msgBox.exec();
    if ( ret == QMessageBox::Cancel )
        return;

    if ( item.isValid() ) {
        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemDeleteJob *job = new ItemDeleteJob( item );
        Q_UNUSED( job );
    }
    const QModelIndex newIndex = mUi.opportunitiesTV->selectionModel()->currentIndex();
    if ( !newIndex.isValid() )
        mUi.removeOpportunityPB->setEnabled( false );
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w )
        w->displayDockWidgets( false );
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
    OpportunityDetails *od = dynamic_cast<OpportunityDetails*>( w->detailsWidget(Opportunity ) );
    QModelIndex index;
    Item item;
    SugarOpportunity opportunity;
    for ( int i = 0; i <  mUi.opportunitiesTV->model()->rowCount(); ++i ) {
       index  =  mUi.opportunitiesTV->model()->index( i, 0 );
       item = mUi.opportunitiesTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
       if ( item.hasPayload<SugarOpportunity>() ) {
           opportunity = item.payload<SugarOpportunity>();
           // Pending(michel) - call this from the right module.
           od->addAssignedToData( opportunity.assignedUserName(), opportunity.assignedUserId() );
       }
    }
}

void OpportunitiesPage::initialize()
{
    mUi.opportunitiesTV->header()->setResizeMode( QHeaderView::ResizeToContents );
    connect( mUi.clearOpportunitySearch, SIGNAL( clicked() ),
             this, SLOT( slotResetSearch() ) );
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

    connect( mUi.opportunitiesTV, SIGNAL( clicked( const QModelIndex& ) ),
             this, SLOT( slotOpportunityClicked( const QModelIndex& ) ) );

    connect( mUi.opportunitiesTV->model(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), SLOT( slotSetCurrent( const QModelIndex&,int,int ) ) );

    connect( mUi.opportunitiesTV->model(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotUpdateItemDetails( const QModelIndex&, const QModelIndex& ) ) );
}

void OpportunitiesPage::syncronize()
{
   if ( mUi.opportunitiesTV->model() != 0
         && mUi.opportunitiesTV->model()->rowCount() > 0 )
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

void OpportunitiesPage::slotUpdateItemDetails( const QModelIndex& topLeft, const QModelIndex& bottomRight )
{
    Q_UNUSED( bottomRight );
    Item item;
    SugarOpportunity opportunity;
    item = mUi.opportunitiesTV->model()->data( topLeft, EntityTreeModel::ItemRole ).value<Item>();
    opportunityChanged( item );
}

bool OpportunitiesPage::proceedIsOk()
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

void OpportunitiesPage::slotSetItem()
{
    mCurrentIndex  = mUi.opportunitiesTV->selectionModel()->currentIndex();
    if ( mCurrentIndex.isValid() )
        slotOpportunityClicked( mCurrentIndex );
    else
        slotNewOpportunityClicked();
}

void OpportunitiesPage::slotResetSearch()
{
    mUi.searchLE->clear();
}
