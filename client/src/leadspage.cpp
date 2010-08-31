#include "leadspage.h"
#include "leadstreemodel.h"
#include "leadsfilterproxymodel.h"
#include "sugarclient.h"
#include "enums.h"

#include "kdcrmdata/sugarlead.h"

#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

#include <QDebug>

using namespace Akonadi;

LeadsPage::LeadsPage( QWidget *parent )
    : Page( parent, QString( SugarLead::mimeType() ), Lead )
{
    setupModel();
}

LeadsPage::~LeadsPage()
{
}


void LeadsPage::addItem( const QMap<QString, QString> data )
{

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
    ItemCreateJob *job = new ItemCreateJob( item, collection() );
    Q_UNUSED( job );

    clientWindow()->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
}

void LeadsPage::modifyItem(Item &item, const QMap<QString, QString> data)
{
    SugarLead lead;
    if ( item.hasPayload<SugarLead>() ) {
        lead = item.payload<SugarLead>();
    } else
        return;

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

void LeadsPage::setupModel()
{
    LeadsTreeModel *leadsModel = new LeadsTreeModel( recorder(), this );

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
    setFilter( filter );
    Page::setupModel();
}

