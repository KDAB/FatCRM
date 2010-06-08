#include "leadspage.h"
#include "leadstreemodel.h"
#include "leadsfilterproxymodel.h"
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

#include "kdcrmdata/sugarlead.h"

using namespace Akonadi;

LeadsPage::LeadsPage( QWidget *parent )
    : QWidget( parent ),
      mChangeRecorder( new ChangeRecorder( this ) )
{
    mUi.setupUi( this );
    initialize();
}

LeadsPage::~LeadsPage()
{
}

void LeadsPage::slotResourceSelectionChanged( const QByteArray &identifier )
{
    if ( mLeadsCollection.isValid() ) {
        mChangeRecorder->setCollectionMonitored( mLeadsCollection, false );
    }

    /*
     * Look for the "Leads" collection explicitly by listing all collections
     * of the currently selected resource, filtering by MIME type.
     * include statistics to get the number of items in each collection
     */
    CollectionFetchJob *job = new CollectionFetchJob( Collection::root(), CollectionFetchJob::Recursive );
    job->fetchScope().setResource( identifier );
    job->fetchScope().setContentMimeTypes( QStringList() << SugarLead::mimeType() );
    job->fetchScope().setIncludeStatistics( true );
    connect( job, SIGNAL( result( KJob* ) ),
             this, SLOT( slotCollectionFetchResult( KJob* ) ) );
}

void LeadsPage::slotCollectionFetchResult( KJob *job )
{

    CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob*>( job );

    // look for the "Leads" collection
    int i = 0;
    Q_FOREACH( const Collection &collection, fetchJob->collections() ) {
        if ( collection.remoteId() == QLatin1String( "Leads" ) ) {
            mLeadsCollection = collection;
            i++;
            break;
        }
    }

    if ( mLeadsCollection.isValid() ) {
        mUi.newLeadPB->setEnabled( true );
        mChangeRecorder->setCollectionMonitored( mLeadsCollection, true );
        // if empty, the collection might not have been loaded yet, try synchronizing
        if ( mLeadsCollection.statistics().count() == 0 ) {
            AgentManager::self()->synchronizeCollection( mLeadsCollection );
        }

        setupCachePolicy();
    } else {
        mUi.newLeadPB->setEnabled( false );
    }
}

void LeadsPage::slotLeadChanged( const Item &item )
{

    if ( item.isValid() && item.hasPayload<SugarLead>() ) {
        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        if ( w ) {
            w->leadDetailsWidget()->setItem( item );
            connect( w->leadDetailsWidget(), SIGNAL( modifyLead() ),
                     this, SLOT( slotModifyLead( ) ) );
        }
        emit leadItemChanged();
    }
}

void LeadsPage::slotNewLeadClicked()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w ) {
        LeadDetails *cd = w->leadDetailsWidget();
        cd->clearFields();
        connect( cd, SIGNAL( saveLead() ),
                 this, SLOT( slotAddLead( ) ) );
    }

    emit showDetails();
}

void LeadsPage::slotAddLead()
{

    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    QMap<QString, QString> data;
    data = w->leadDetailsWidget()->leadData();
    SugarLead lead;
    lead.setDateEntered( data.value( "dateEntered" ) );
    lead.setDateModified( data.value( "dateModified" ) );
    lead.setModifiedUserId( data.value( "modifiedUserId" ) );
    lead.setModifiedByName( data.value( "modifiedByName" ) );
    lead.setCreatedBy( data.value( "createdBy" ) ); // id
    lead.setCreatedByName( data.value( "createdByName" ) );
    lead.setDescription( data.value( "description" ) );
    lead.setDeleted( data.value( "deleted" ) );
    lead.setAssignedUserId( data.value( "assignedUserId" ) );
    lead.setAssignedUserName( data.value( "assignedUserName" ) );
    lead.setSalutation( data.value( "salutation" ) );
    lead.setFirstName( data.value( "firstName" ) );
    lead.setLastName( data.value( "lastName" ) );
    lead.setTitle( data.value( "title" ) );
    lead.setDepartment( data.value( "department" ) );
    lead.setDoNotCall( data.value( "doNotCall" ) );
    lead.setPhoneHome( data.value( "phoneHome" ) );
    lead.setPhoneMobile( data.value( "phoneMobile" ) );
    lead.setPhoneWork( data.value( "phoneWork" ) );
    lead.setPhoneOther( data.value( "phoneOther" ) );
    lead.setPhoneFax( data.value( "phoneFax" ) );
    lead.setEmail1( data.value( "email1" ) );
    lead.setEmail2( data.value( "email2" ) );
    lead.setPrimaryAddressStreet( data.value( "primaryAddressStreet" ) );
    lead.setPrimaryAddressCity( data.value( "primaryAddressCity" ) );
    lead.setPrimaryAddressState( data.value( "primaryAddressState" ) );
    lead.setPrimaryAddressPostalcode( data.value( "primaryAddressPostalcode" ) );
    lead.setPrimaryAddressCountry( data.value( "primaryAddressCountry" ) );
    lead.setAltAddressStreet( data.value( "altAddressStreet" ) );
    lead.setAltAddressCity( data.value( "altAddressCity" ) );
    lead.setAltAddressState( data.value( "altAddressState" ) );
    lead.setAltAddressPostalcode( data.value( "altAddressPostalcode" ) );
    lead.setAltAddressCountry( data.value( "altAddressCountry" ) );
    lead.setAssistant( data.value( "assistant" ) );
    lead.setAssistantPhone( data.value( "assistantPhone" ) );
    lead.setConverted( data.value( "converted" ) );
    lead.setReferedBy( data.value( "referedBy" ) );
    lead.setLeadSource( data.value( "leadSource" ) );
    lead.setLeadSourceDescription( data.value( "leadSourceDescription" ) );
    lead.setStatus( data.value( "status" ) );
    lead.setStatusDescription( data.value( "statusDescription" ) );
    lead.setReportsToId( data.value( "reportsToId" ) );
    lead.setReportToName( data.value( "reportToName" ) );
    lead.setAccountName( data.value( "accountName" ) );
    lead.setAccountDescription( data.value( "accountDescription" ) );
    lead.setContactId( data.value( "contactId" ) );
    lead.setAccountId( data.value( "accountId" ) );
    lead.setOpportunityId( data.value( "opportunityId" ) );
    lead.setOpportunityName( data.value( "opportunityName" ) );
    lead.setOpportunityAmount( data.value( "opportunityAmount" ) );
    lead.setCampaignId( data.value( "campaignId" ) );
    lead.setCampaignName( data.value( "campaignName" ) );
    lead.setCAcceptStatusFields( data.value( "cAcceptStatusFields" ) );
    lead.setMAcceptStatusFields( data.value( "mAcceptStatusFields" ) );
    lead.setBirthdate( data.value( "birthdate" ) );
    lead.setPortalName( data.value( "portalName" ) );
    lead.setPortalApp( data.value( "portalApp" ) );

    Item item;
    item.setMimeType( SugarLead::mimeType() );
    item.setPayload<SugarLead>( lead );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob( item, mLeadsCollection );
    Q_UNUSED( job );

    disconnect( w->leadDetailsWidget(), SIGNAL( saveLead() ),
                this, SLOT( slotAddLead( ) ) );
}

void LeadsPage::slotModifyLead()
{
    const QModelIndex index = mUi.leadsTV->selectionModel()->currentIndex();
    Item item = mUi.leadsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    if ( item.isValid() ) {
        SugarLead lead;
        if ( item.hasPayload<SugarLead>() ) {
            lead = item.payload<SugarLead>();
        }

        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        QMap<QString, QString> data;
        data = w->leadDetailsWidget()->leadData();
        lead.setDateEntered( data.value( "dateEntered" ) );
        lead.setDateModified( data.value( "dateModified" ) );
        lead.setModifiedUserId( data.value( "modifiedUserId" ) );
        lead.setModifiedByName( data.value( "modifiedByName" ) );
        lead.setCreatedBy( data.value( "createdBy" ) ); // id
        lead.setCreatedByName( data.value( "createdByName" ) );
        lead.setDescription( data.value( "description" ) );
        lead.setDeleted( data.value( "deleted" ) );
        lead.setAssignedUserId( data.value( "assignedUserId" ) );
        lead.setAssignedUserName( data.value( "assignedUserName" ) );
        lead.setSalutation( data.value( "salutation" ) );
        lead.setFirstName( data.value( "firstName" ) );
        lead.setLastName( data.value( "lastName" ) );
        lead.setTitle( data.value( "title" ) );
        lead.setDepartment( data.value( "department" ) );
        lead.setDoNotCall( data.value( "doNotCall" ) );
        lead.setPhoneHome( data.value( "phoneHome" ) );
        lead.setPhoneMobile( data.value( "phoneMobile" ) );
        lead.setPhoneWork( data.value( "phoneWork" ) );
        lead.setPhoneOther( data.value( "phoneOther" ) );
        lead.setPhoneFax( data.value( "phoneFax" ) );
        lead.setEmail1( data.value( "email1" ) );
        lead.setEmail2( data.value( "email2" ) );
        lead.setPrimaryAddressStreet( data.value( "primaryAddressStreet" ) );
        lead.setPrimaryAddressCity( data.value( "primaryAddressCity" ) );
        lead.setPrimaryAddressState( data.value( "primaryAddressState" ) );
        lead.setPrimaryAddressPostalcode( data.value( "primaryAddressPostalcode" ) );
        lead.setPrimaryAddressCountry( data.value( "primaryAddressCountry" ) );
        lead.setAltAddressStreet( data.value( "altAddressStreet" ) );
        lead.setAltAddressCity( data.value( "altAddressCity" ) );
        lead.setAltAddressState( data.value( "altAddressState" ) );
        lead.setAltAddressPostalcode( data.value( "altAddressPostalcode" ) );
        lead.setAltAddressCountry( data.value( "altAddressCountry" ) );
        lead.setAssistant( data.value( "assistant" ) );
        lead.setAssistantPhone( data.value( "assistantPhone" ) );
        lead.setConverted( data.value( "converted" ) );
        lead.setReferedBy( data.value( "referedBy" ) );
        lead.setLeadSource( data.value( "leadSource" ) );
        lead.setLeadSourceDescription( data.value( "leadSourceDescription" ) );
        lead.setStatus( data.value( "status" ) );
        lead.setStatusDescription( data.value( "statusDescription" ) );
        lead.setReportsToId( data.value( "reportsToId" ) );
        lead.setReportToName( data.value( "reportToName" ) );
        lead.setAccountName( data.value( "accountName" ) );
        lead.setAccountDescription( data.value( "accountDescription" ) );
        lead.setContactId( data.value( "contactId" ) );
        lead.setAccountId( data.value( "accountId" ) );
        lead.setOpportunityId( data.value( "opportunityId" ) );
        lead.setOpportunityName( data.value( "opportunityName" ) );
        lead.setOpportunityAmount( data.value( "opportunityAmount" ) );
        lead.setCampaignId( data.value( "campaignId" ) );
        lead.setCampaignName( data.value( "campaignName" ) );
        lead.setCAcceptStatusFields( data.value( "cAcceptStatusFields" ) );
        lead.setMAcceptStatusFields( data.value( "mAcceptStatusFields" ) );
        lead.setBirthdate( data.value( "birthdate" ) );
        lead.setPortalName( data.value( "portalName" ) );
        lead.setPortalApp( data.value( "portalApp" ) );

        item.setPayload<SugarLead>( lead );

        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemModifyJob *job = new ItemModifyJob( item );
        Q_UNUSED( job );
    }
}

void LeadsPage::slotRemoveLead()
{
    const QModelIndex index = mUi.leadsTV->selectionModel()->currentIndex();
    Item item = mUi.leadsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    if ( item.isValid() ) {
        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemDeleteJob *job = new ItemDeleteJob( item );
        Q_UNUSED( job );
    }
    const QModelIndex newIndex = mUi.leadsTV->selectionModel()->currentIndex();
    if ( !index.isValid() )
        mUi.removeLeadPB->setEnabled( false );
}

void LeadsPage::slotSetCurrent( const QModelIndex& index, int start, int end )
{
    if ( start == end ) {
        QModelIndex newIdx = mUi.leadsTV->model()->index(start, 0, index);
        mUi.leadsTV->setCurrentIndex( newIdx );
    }
    //model items are loaded
    if ( mUi.leadsTV->model()->rowCount() == mLeadsCollection.statistics().count() )
        addLeadsData();
}

void LeadsPage::addLeadsData()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    LeadDetails *ad = w->leadDetailsWidget();

    QModelIndex index;
    Item item;
    SugarLead lead;
    for ( int i = 0; i <  mUi.leadsTV->model()->rowCount(); ++i ) {
        index  =  mUi.leadsTV->model()->index( i, 0 );
        item = mUi.leadsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
        if ( item.hasPayload<SugarLead>() ) {
            lead = item.payload<SugarLead>();
            // code below should be executed from
            // their own pages when implemented
            ad->addAssignedToData( lead.assignedUserName(), lead.assignedUserId() );
        }
    }
}

void LeadsPage::initialize()
{
    mUi.leadsTV->header()->setResizeMode( QHeaderView::ResizeToContents );

    connect( mUi.newLeadPB, SIGNAL( clicked() ),
             this, SLOT( slotNewLeadClicked() ) );
    connect( mUi.removeLeadPB, SIGNAL( clicked() ),
             this, SLOT( slotRemoveLead() ) );


    // automatically get the full data when items change
    mChangeRecorder->itemFetchScope().fetchFullPayload( true );

    LeadsTreeModel *leadsModel = new LeadsTreeModel( mChangeRecorder, this );

    LeadsTreeModel::Columns columns;
    columns << LeadsTreeModel::Name
            << LeadsTreeModel::Status
            << LeadsTreeModel::AccountName
            << LeadsTreeModel::Email
            << LeadsTreeModel::User;
    leadsModel->setColumns( columns );

    // same as for the ContactsTreeModel, not strictly necessary
    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel( this );
    filterModel->setSourceModel( leadsModel );
    filterModel->addMimeTypeInclusionFilter( SugarLead::mimeType() );
    filterModel->setHeaderGroup( EntityTreeModel::ItemListHeaders );

    LeadsFilterProxyModel *filter = new LeadsFilterProxyModel( this );
    filter->setSourceModel( filterModel );
    mUi.leadsTV->setModel( filter );

    connect( mUi.searchLE, SIGNAL( textChanged( const QString& ) ),
             filter, SLOT( setFilterString( const QString& ) ) );

    connect( mUi.leadsTV, SIGNAL( currentChanged( Akonadi::Item ) ), this, SLOT( slotLeadChanged( Akonadi::Item ) ) );

    connect( mUi.leadsTV->model(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), SLOT( slotSetCurrent( const QModelIndex&,int,int ) ) );

}

void LeadsPage::syncronize()
{
    AgentManager::self()->synchronizeCollection( mLeadsCollection );
}

void LeadsPage::cachePolicyJobCompleted( KJob* job)
{
    if ( job->error() )
        emit statusMessage( tr("Error when setting cachepolicy: %1").arg( job->errorString() ) );
    else
        emit statusMessage( tr("Cache policy set") );

}

void LeadsPage::setupCachePolicy()
{
    CachePolicy policy;
    policy.setIntervalCheckTime( 1 ); // Check for new data every minute
    policy.setInheritFromParent( false );
    mLeadsCollection.setCachePolicy( policy );
    CollectionModifyJob *job = new CollectionModifyJob( mLeadsCollection );
    connect( job, SIGNAL( result( KJob* ) ), this, SLOT( cachePolicyJobCompleted( KJob* ) ) );
}

void LeadsPage::slotFilterChanged( const QString& filterString )
{
    Q_UNUSED( filterString );
}
