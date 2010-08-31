#include "campaignspage.h"

#include "campaignstreemodel.h"
#include "campaignsfilterproxymodel.h"
#include "sugarclient.h"
#include "enums.h"

#include "kdcrmdata/sugarcampaign.h"

#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

using namespace Akonadi;

CampaignsPage::CampaignsPage( QWidget *parent )
    : Page( parent, QString( SugarCampaign::mimeType() ), Campaign )
{
    setupModel();
}

CampaignsPage::~CampaignsPage()
{
}

void CampaignsPage::addItem(const QMap<QString, QString> data )
{
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
    ItemCreateJob *job = new ItemCreateJob( item, collection() );
    Q_UNUSED( job );

    clientWindow()->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
}

void CampaignsPage::modifyItem(Item &item, const QMap<QString, QString> data)
{
    SugarCampaign campaign;
    if ( item.hasPayload<SugarCampaign>() ) {
        campaign = item.payload<SugarCampaign>();
    } else
        return;
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
    item.setRemoteRevision( data.value( "remoteRevision" ) );
    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemModifyJob *job = new ItemModifyJob( item );
    if ( !job->exec() )
        return; //qDebug() << "Error:" << job->errorString();

    clientWindow()->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
}

void CampaignsPage::setupModel()
{
    CampaignsTreeModel *campaignsModel = new CampaignsTreeModel( recorder(), this );

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
    setFilter( filter );
    Page::setupModel();
}
