#include "opportunitiespage.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "sugarclient.h"

#include "kdcrmdata/sugaropportunity.h"

#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

#include <QDebug>


using namespace Akonadi;

OpportunitiesPage::OpportunitiesPage( QWidget *parent )
    : Page( parent, QString( SugarOpportunity::mimeType() ), Opportunity )
{
    setupModel();
}

OpportunitiesPage::~OpportunitiesPage()
{
}

void OpportunitiesPage::addItem( const QMap<QString, QString> &data )
{
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
    item.setMimeType( mimeType() );
    item.setPayload<SugarOpportunity>( opportunity );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob( item, collection() );
    Q_UNUSED( job );
    clientWindow()->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
}

void OpportunitiesPage::modifyItem( Item &item, const QMap<QString, QString> &data )
{
        SugarOpportunity opportunity;
        if ( item.hasPayload<SugarOpportunity>() ) {
            opportunity = item.payload<SugarOpportunity>();
        } else
            return;

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

        clientWindow()->setEnabled( false );
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
        emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
}

void OpportunitiesPage::setupModel()
{
    ItemsTreeModel *opportunitiesModel = new ItemsTreeModel( recorder(), this );

    ItemsTreeModel::Columns columns;
    columns << ItemsTreeModel::OpportunityName
            << ItemsTreeModel::OpportunityAccountName
            << ItemsTreeModel::SalesStage
            << ItemsTreeModel::Amount
            << ItemsTreeModel::Close
            << ItemsTreeModel::AssignedTo;
    opportunitiesModel->setColumns( columns );

    // same as for the ContactsTreeModel, not strictly necessary
    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel( this );
    filterModel->setSourceModel( opportunitiesModel );
    filterModel->addMimeTypeInclusionFilter( SugarOpportunity::mimeType() );
    filterModel->setHeaderGroup( EntityTreeModel::ItemListHeaders );

    FilterProxyModel *filter = new FilterProxyModel( this );
    filter->setSourceModel( filterModel );
    setFilter( filter );
    Page::setupModel();
}
