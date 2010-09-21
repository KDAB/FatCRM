#include "leadshandler.h"

#include "sugarsoap.h"

#include <akonadi/abstractdifferencesreporter.h>
#include <akonadi/collection.h>

#include <kabc/address.h>

#include <kdcrmdata/sugarlead.h>

#include <KLocale>

#include <QHash>

typedef QString (*valueGetter)( const SugarLead& );
typedef void (*valueSetter)( const QString&, SugarLead&);

static void setId( const QString &value, SugarLead &lead )
{
    lead.setId( value );
}

static QString getDateEntered( const SugarLead &lead )
{
    return lead.dateEntered();
}

static void setDateEntered( const QString &value, SugarLead &lead )
{
    lead.setDateEntered( value );
}

static QString getDateModified( const SugarLead &lead )
{
    return lead.dateModified();
}

static void setDateModified( const QString &value, SugarLead &lead )
{
    lead.setDateModified( value );
}

static QString getModifiedUserId( const SugarLead &lead )
{
    return lead.modifiedUserId();
}

static void setModifiedUserId( const QString &value, SugarLead &lead )
{
    lead.setModifiedUserId( value );
}

static QString getModifiedByName( const SugarLead &lead )
{
    return lead.modifiedByName();
}

static void setModifiedByName( const QString &value, SugarLead &lead )
{
    lead.setModifiedByName( value );
}

static QString getCreatedBy( const SugarLead &lead )
{
    return lead.createdBy();
}

static void setCreatedBy( const QString &value, SugarLead &lead )
{
    lead.setCreatedBy( value );
}

static QString getCreatedByName( const SugarLead &lead )
{
    return lead.createdByName();
}

static void setCreatedByName( const QString &value, SugarLead &lead )
{
    lead.setCreatedByName( value );
}

static QString getDescription( const SugarLead &lead )
{
    return lead.description();
}

static void setDescription( const QString &value, SugarLead &lead )
{
    lead.setDescription( value );
}

static QString getDeleted( const SugarLead &lead )
{
    return lead.deleted();
}

static void setDeleted (const QString &value, SugarLead &lead )
{
    lead.setDeleted( value );
}

static QString getAssignedUserId( const SugarLead &lead )
{
    return lead.assignedUserId();
}

static void setAssignedUserId(const QString &value, SugarLead &lead)
{
    lead.setAssignedUserId( value );
}

static QString getAssignedUserName( const SugarLead &lead )
{
    return lead.assignedUserName();
}

static void setAssignedUserName(const QString &value, SugarLead &lead)
{
    lead.setAssignedUserName( value );
}

static QString getSalutation( const SugarLead &lead )
{
    return lead.salutation();
}

static void setSalutation(const QString &value, SugarLead &lead)
{
    lead.setSalutation( value );
}

static QString getFirstName( const SugarLead &lead )
{
    return lead.firstName();
}

static void setFirstName(const QString &value, SugarLead &lead)
{
    lead.setFirstName( value );
}

static QString getLastName( const SugarLead &lead )
{
    return lead.lastName();
}

static void setLastName(const QString &value, SugarLead &lead)
{
    lead.setLastName( value );
}

static QString getTitle( const SugarLead &lead )
{
    return lead.title();
}

static void setTitle(const QString &value, SugarLead &lead)
{
    lead.setTitle( value );
}

static QString getDepartment( const SugarLead &lead )
{
    return lead.department();
}

static void setDepartment(const QString &value, SugarLead &lead)
{
    lead.setDepartment( value );
}

static QString getDoNotCall( const SugarLead &lead )
{
    return lead.doNotCall();
}

static void setDoNotCall(const QString &value, SugarLead &lead)
{
    lead.setDoNotCall( value );
}

static QString getPhoneHome( const SugarLead &lead )
{
    return lead.phoneHome();
}

static void setPhoneHome(const QString &value, SugarLead &lead)
{
    lead.setPhoneHome( value );
}

static QString getPhoneMobile( const SugarLead &lead )
{
    return lead.phoneMobile();
}

static void setPhoneMobile(const QString &value, SugarLead &lead)
{
    lead.setPhoneMobile( value );
}

static QString getPhoneWork( const SugarLead &lead )
{
    return lead.phoneWork();
}

static void setPhoneWork(const QString &value, SugarLead &lead)
{
    lead.setPhoneWork( value );
}

static QString getPhoneOther( const SugarLead &lead )
{
    return lead.phoneOther();
}

static void setPhoneOther(const QString &value, SugarLead &lead)
{
    lead.setPhoneOther( value );
}

static QString getPhoneFax( const SugarLead &lead )
{
    return lead.phoneFax();
}

static void setPhoneFax(const QString &value, SugarLead &lead)
{
    lead.setPhoneFax( value );
}

static QString getEmail1( const SugarLead &lead )
{
    return lead.email1();
}

static void setEmail1(const QString &value, SugarLead &lead)
{
    lead.setEmail1( value );
}

static QString getEmail2( const SugarLead &lead )
{
    return lead.email2();
}

static void setEmail2(const QString &value, SugarLead &lead)
{
    lead.setEmail2( value );
}

static QString getPrimaryAddressStreet( const SugarLead &lead )
{
    return lead.primaryAddressStreet();
}

static void setPrimaryAddressStreet(const QString &value, SugarLead &lead)
{
    lead.setPrimaryAddressStreet( value );
}

static QString getPrimaryAddressCity( const SugarLead &lead )
{
    return lead.primaryAddressCity();
}

static void setPrimaryAddressCity(const QString &value, SugarLead &lead)
{
    lead.setPrimaryAddressCity( value );
}

static QString getPrimaryAddressState( const SugarLead &lead )
{
    return lead.primaryAddressState();
}

static void setPrimaryAddressState(const QString &value, SugarLead &lead)
{
    lead.setPrimaryAddressState( value );
}

static QString getPrimaryAddressPostalcode( const SugarLead &lead )
{
    return lead.primaryAddressPostalcode();
}

static void setPrimaryAddressPostalcode(const QString &value, SugarLead &lead)
{
    lead.setPrimaryAddressPostalcode( value );
}

static QString getPrimaryAddressCountry( const SugarLead &lead )
{
    return lead.primaryAddressCountry();
}

static void setPrimaryAddressCountry(const QString &value, SugarLead &lead)
{
    lead.setPrimaryAddressCountry( value );
}

static QString getAltAddressStreet( const SugarLead &lead )
{
    return lead.altAddressStreet();
}

static void setAltAddressStreet(const QString &value, SugarLead &lead)
{
    lead.setAltAddressStreet( value );
}

static QString getAltAddressCity( const SugarLead &lead )
{
    return lead.altAddressCity();
}

static void setAltAddressCity(const QString &value, SugarLead &lead)
{
    lead.setAltAddressCity( value );
}

static QString getAltAddressState( const SugarLead &lead )
{
    return lead.altAddressState();
}

static void setAltAddressState(const QString &value, SugarLead &lead)
{
    lead.setAltAddressState( value );
}

static QString getAltAddressPostalcode( const SugarLead &lead )
{
    return lead.altAddressPostalcode();
}

static void setAltAddressPostalcode(const QString &value, SugarLead &lead)
{
    lead.setAltAddressPostalcode( value );
}

static QString getAltAddressCountry( const SugarLead &lead )
{
    return lead.altAddressCountry();
}

static void setAltAddressCountry(const QString &value, SugarLead &lead)
{
    lead.setAltAddressCountry( value );
}

static QString getAssistant( const SugarLead &lead )
{
    return lead.assistant();
}

static void setAssistant(const QString &value, SugarLead &lead)
{
    lead.setAssistant( value );
}

static QString getAssistantPhone( const SugarLead &lead )
{
    return lead.assistantPhone();
}

static void setAssistantPhone(const QString &value, SugarLead &lead)
{
    lead.setAssistantPhone( value );
}

static QString getConverted( const SugarLead &lead )
{
    return lead.converted();
}

static void setConverted(const QString &value, SugarLead &lead)
{
    lead.setConverted( value );
}

static QString getReferedBy( const SugarLead &lead )
{
    return lead.referedBy();
}

static void setReferedBy(const QString &value, SugarLead &lead)
{
    lead.setReferedBy( value );
}

static QString getLeadSource( const SugarLead &lead )
{
    return lead.leadSource();
}

static void setLeadSource(const QString &value, SugarLead &lead)
{
    lead.setLeadSource( value );
}

static QString getLeadSourceDescription( const SugarLead &lead )
{
    return lead.leadSourceDescription();
}

static void setLeadSourceDescription(const QString &value, SugarLead &lead)
{
    lead.setLeadSourceDescription( value );
}

static QString getStatus( const SugarLead &lead )
{
    return lead.status();
}

static void setStatus(const QString &value, SugarLead &lead)
{
    lead.setStatus( value );
}

static QString getStatusDescription( const SugarLead &lead )
{
    return lead.statusDescription();
}

static void setStatusDescription(const QString &value, SugarLead &lead)
{
    lead.setStatusDescription( value );
}

static QString getReportsToId( const SugarLead &lead )
{
    return lead.reportsToId();
}

static void setReportsToId(const QString &value, SugarLead &lead)
{
    lead.setReportsToId( value );
}

static QString getReportToName( const SugarLead &lead )
{
    return lead.reportToName();
}

static void setReportToName(const QString &value, SugarLead &lead)
{
    lead.setReportToName( value );
}

static QString getAccountName( const SugarLead &lead )
{
    return lead.accountName();
}

static void setAccountName(const QString &value, SugarLead &lead)
{
    lead.setAccountName( value );
}

static QString getAccountDescription( const SugarLead &lead )
{
    return lead.accountDescription();
}

static void setAccountDescription(const QString &value, SugarLead &lead)
{
    lead.setAccountDescription( value );
}

static QString getContactId( const SugarLead &lead )
{
    return lead.contactId();
}

static void setContactId(const QString &value, SugarLead &lead)
{
    lead.setContactId( value );
}

static QString getAccountId( const SugarLead &lead )
{
    return lead.accountId();
}

static void setAccountId(const QString &value, SugarLead &lead)
{
    lead.setAccountId( value );
}

static QString getOpportunityId( const SugarLead &lead )
{
    return lead.opportunityId();
}

static void setOpportunityId(const QString &value, SugarLead &lead)
{
    lead.setOpportunityId( value );
}

static QString getOpportunityName( const SugarLead &lead )
{
    return lead.opportunityName();
}

static void setOpportunityName(const QString &value, SugarLead &lead)
{
    lead.setOpportunityName( value );
}

static QString getOpportunityAmount( const SugarLead &lead )
{
    return lead.opportunityAmount();
}

static void setOpportunityAmount(const QString &value, SugarLead &lead)
{
    lead.setOpportunityAmount( value );
}

static QString getCampaignId( const SugarLead &lead )
{
    return lead.campaignId();
}

static void setCampaignId( const QString &value, SugarLead &lead )
{
    lead.setCampaignId( value );
}

static QString getCampaignName( const SugarLead &lead )
{
    return lead.campaignName();
}

static void setCampaignName( const QString &value, SugarLead &lead )
{
    lead.setCampaignName( value );
}

static QString getCAcceptStatusFields( const SugarLead &lead )
{
    return lead.cAcceptStatusFields();
}

static void setCAcceptStatusFields(const QString &value, SugarLead &lead)
{
    lead.setCAcceptStatusFields( value );
}

static QString getMAcceptStatusFields( const SugarLead &lead )
{
    return lead.mAcceptStatusFields();
}

static void setMAcceptStatusFields(const QString &value, SugarLead &lead)
{
    lead.setMAcceptStatusFields( value );
}

static QString getBirthdate( const SugarLead &lead )
{
    return lead.birthdate();
}

static void setBirthdate( const QString &value, SugarLead &lead )
{
    lead.setBirthdate( value );
}

static QString getPortalName( const SugarLead &lead )
{
    return lead.portalName();
}

static void setPortalName( const QString &value, SugarLead &lead )
{
    lead.setPortalName( value );
}

static QString getPortalApp( const SugarLead &lead )
{
    return lead.portalApp();
}

static void setPortalApp( const QString &value, SugarLead &lead )
{
    lead.setPortalApp( value );
}

class AccessorPair
{
public:
    AccessorPair( valueGetter get, valueSetter set, const QString &name )
        : getter( get ), setter( set ), diffName( name )
    {}

public:
    valueGetter getter;
    valueSetter setter;
    const QString diffName;
};

LeadsHandler::LeadsHandler( SugarSession *session )
    : ModuleHandler( QLatin1String( "Leads" ), session ),
      mAccessors( new AccessorHash )
{
    mAccessors->insert( QLatin1String( "id" ),
                        new AccessorPair( 0, setId, QString() ) );
    mAccessors->insert( QLatin1String( "date_entered" ),
                        new AccessorPair( getDateEntered, setDateEntered, QString() ) );
    mAccessors->insert( QLatin1String( "date_modified" ),
                        new AccessorPair( getDateModified, setDateModified, QString() ) );
    mAccessors->insert( QLatin1String( "modified_user_id" ),
                        new AccessorPair( getModifiedUserId, setModifiedUserId, QString() ) );
    mAccessors->insert( QLatin1String( "modified_by_name" ),
                        new AccessorPair( getModifiedByName, setModifiedByName, QString() ) );
    mAccessors->insert( QLatin1String( "created_by" ),
                        new AccessorPair( getCreatedBy, setCreatedBy, QString() ) );
    mAccessors->insert( QLatin1String( "created_by_name" ),
                        new AccessorPair( getCreatedByName, setCreatedByName, QString() ) );
    mAccessors->insert( QLatin1String( "description" ),
                        new AccessorPair( getDescription, setDescription,
                                          i18nc( "@item:intable", "Description" ) ) );
    mAccessors->insert( QLatin1String( "deleted" ),
                        new AccessorPair( getDeleted, setDeleted, QString() ) );
    mAccessors->insert( QLatin1String( "assigned_user_id" ),
                        new AccessorPair( getAssignedUserId, setAssignedUserId, QString() ) );
    mAccessors->insert( QLatin1String( "assigned_user_name" ),
                        new AccessorPair( getAssignedUserName, setAssignedUserName,
                                          i18nc( "@item:intable", "Assigned To" ) ) );
    mAccessors->insert( QLatin1String( "salutation" ),
                        new AccessorPair( getSalutation, setSalutation,
                                          i18nc( "@item:intable", "Salutation" ) ) );
    mAccessors->insert( QLatin1String( "first_name" ),
                        new AccessorPair( getFirstName, setFirstName,
                                          i18nc( "@item:intable", "First Name" ) ) );
    mAccessors->insert( QLatin1String( "last_name" ),
                        new AccessorPair( getLastName, setLastName,
                                          i18nc( "@item:intable", "Last Name" ) ) );
    mAccessors->insert( QLatin1String( "title" ),
                        new AccessorPair( getTitle, setTitle,
                                          i18nc( "@item:intable job title", "Title" ) ) );
    mAccessors->insert( QLatin1String( "department" ),
                        new AccessorPair( getDepartment, setDepartment,
                                          i18nc( "@item:intable", "Department" ) ) );
    mAccessors->insert( QLatin1String( "do_not_call" ),
                        new AccessorPair( getDoNotCall, setDoNotCall, QString() ) );
    mAccessors->insert( QLatin1String( "phone_home" ),
                        new AccessorPair( getPhoneHome, setPhoneHome,
                                          i18nc( "@item:intable", "Phone (Home)" ) ) );
    mAccessors->insert( QLatin1String( "phone_mobile" ),
                        new AccessorPair( getPhoneMobile, setPhoneMobile,
                                          i18nc( "@item:intable", "Phone (Mobile)" ) ) );
    mAccessors->insert( QLatin1String( "phone_work" ),
                        new AccessorPair( getPhoneWork, setPhoneWork,
                                          i18nc( "@item:intable", "Phone (Office)" ) ) );
    mAccessors->insert( QLatin1String( "phone_other" ),
                        new AccessorPair( getPhoneOther, setPhoneOther,
                                          i18nc( "@item:intable", "Phone (Other)" ) ) );
    mAccessors->insert( QLatin1String( "phone_fax" ),
                        new AccessorPair( getPhoneFax, setPhoneFax,
                                          i18nc( "@item:intable", "Fax" ) ) );
    mAccessors->insert( QLatin1String( "email1" ),
                        new AccessorPair( getEmail1, setEmail1,
                                          i18nc( "@item:intable", "Primary Email" ) ) );
    mAccessors->insert( QLatin1String( "email2" ),
                        new AccessorPair( getEmail2, setEmail2,
                                          i18nc( "@item:intable", "Other Email" ) ) );
    mAccessors->insert( QLatin1String( "primary_address_street" ),
                        new AccessorPair( getPrimaryAddressStreet, setPrimaryAddressStreet, QString() ) );
    mAccessors->insert( QLatin1String( "primary_address_city" ),
                        new AccessorPair( getPrimaryAddressCity, setPrimaryAddressCity, QString() ) );
    mAccessors->insert( QLatin1String( "primary_address_state" ),
                        new AccessorPair( getPrimaryAddressState, setPrimaryAddressState, QString() ) );
    mAccessors->insert( QLatin1String( "primary_address_postalcode" ),
                        new AccessorPair( getPrimaryAddressPostalcode, setPrimaryAddressPostalcode, QString() ) );
    mAccessors->insert( QLatin1String( "primary_address_country" ),
                        new AccessorPair( getPrimaryAddressCountry, setPrimaryAddressCountry, QString() ) );
    mAccessors->insert( QLatin1String( "alt_address_street" ),
                        new AccessorPair( getAltAddressStreet, setAltAddressStreet, QString() ) );
    mAccessors->insert( QLatin1String( "alt_address_city" ),
                        new AccessorPair( getAltAddressCity, setAltAddressCity, QString() ) );
    mAccessors->insert( QLatin1String( "alt_address_state" ),
                        new AccessorPair( getAltAddressState, setAltAddressState, QString() ) );
    mAccessors->insert( QLatin1String( "alt_address_postalcode" ),
                        new AccessorPair( getAltAddressPostalcode, setAltAddressPostalcode, QString() ) );
    mAccessors->insert( QLatin1String( "alt_address_country" ),
                        new AccessorPair( getAltAddressCountry, setAltAddressCountry, QString() ) );
    mAccessors->insert( QLatin1String( "assistant" ),
                        new AccessorPair( getAssistant, setAssistant,
                                          i18nc( "@item:intable", "Assistant" ) ) );
    mAccessors->insert( QLatin1String( "assistant_phone" ),
                        new AccessorPair( getAssistantPhone, setAssistantPhone,
                                          i18nc( "@item:intable", "Assistant Phone" ) ) );
    mAccessors->insert( QLatin1String( "converted" ),
                        new AccessorPair( getConverted, setConverted, QString() ) );
    mAccessors->insert( QLatin1String( "refered_by" ),
                        new AccessorPair( getReferedBy, setReferedBy,
                                          i18nc( "@item:intable", "Referred By" ) ) );
    mAccessors->insert( QLatin1String( "lead_source" ),
                        new AccessorPair( getLeadSource, setLeadSource,
                                          i18nc( "@item:intable", "Lead Source" ) ) );
    mAccessors->insert( QLatin1String( "lead_source_description" ),
                        new AccessorPair( getLeadSourceDescription, setLeadSourceDescription,
                                          i18nc( "@item:intable", "Lead Source Description" ) ) );
    mAccessors->insert( QLatin1String( "status" ),
                        new AccessorPair( getStatus, setStatus,
                                          i18nc( "@item:intable", "Status" ) ) );
    mAccessors->insert( QLatin1String( "status_description" ),
                        new AccessorPair( getStatusDescription, setStatusDescription,
                                          i18nc( "@item:intable", "Status Description" ) ) );
    mAccessors->insert( QLatin1String( "reports_to_id" ),
                        new AccessorPair( getReportsToId, setReportsToId, QString() ) );
    mAccessors->insert( QLatin1String( "report_to_name" ),
                        new AccessorPair( getReportToName, setReportToName,
                                          i18nc( "@item:intable", "Reports To" ) ) );
    mAccessors->insert( QLatin1String( "account_name" ),
                        new AccessorPair( getAccountName, setAccountName,
                                          i18nc( "@item:intable", "Account" ) ) );
    mAccessors->insert( QLatin1String( "account_description" ),
                        new AccessorPair( getAccountDescription, setAccountDescription,
                                          i18nc( "@item:intable", "Account Description" ) ) );
    mAccessors->insert( QLatin1String( "contact_id" ),
                        new AccessorPair( getContactId, setContactId, QString() ) );
    mAccessors->insert( QLatin1String( "account_id" ),
                        new AccessorPair( getAccountId, setAccountId, QString() ) );
    mAccessors->insert( QLatin1String( "opportunity_id" ),
                        new AccessorPair( getOpportunityId, setOpportunityId, QString() ) );
    mAccessors->insert( QLatin1String( "opportunity_name" ),
                        new AccessorPair( getOpportunityName, setOpportunityName,
                                          i18nc( "@item:intable", "Opportunity" ) ) );
    mAccessors->insert( QLatin1String( "opportunity_amount" ),
                        new AccessorPair( getOpportunityAmount, setOpportunityAmount,
                                          i18nc( "@item:intable", "Opportunity Amount" ) ) );
    mAccessors->insert( QLatin1String( "campaign_id" ),
                        new AccessorPair( getCampaignId, setCampaignId, QString() ) );
    mAccessors->insert( QLatin1String( "campaign_name" ),
                        new AccessorPair( getCampaignName, setCampaignName,
                                          i18nc( "@item:intable", "Campaign" ) ) );
    mAccessors->insert( QLatin1String( "c_accept_status_fields" ),
                        new AccessorPair( getCAcceptStatusFields, setCAcceptStatusFields, QString() ) );
    mAccessors->insert( QLatin1String( "m_accept_status_fields" ),
                        new AccessorPair( getMAcceptStatusFields, setMAcceptStatusFields, QString() ) );
    mAccessors->insert( QLatin1String( "birthdate" ),
                        new AccessorPair( getBirthdate, setBirthdate,
                                          i18nc( "@item:intable", "Birthdate" ) ) );
    mAccessors->insert( QLatin1String( "portal_name" ),
                        new AccessorPair( getPortalName, setPortalName,
                                          i18nc( "@item:intable", "Portal" ) ) );
    mAccessors->insert( QLatin1String( "portal_app" ),
                        new AccessorPair( getPortalApp, setPortalApp,
                                          i18nc( "@item:intable", "Portal Application" ) ) );
}

LeadsHandler::~LeadsHandler()
{
    qDeleteAll( *mAccessors );
    delete mAccessors;
}

QStringList LeadsHandler::supportedFields() const
{
    return mAccessors->keys();
}

Akonadi::Collection LeadsHandler::collection() const
{
    Akonadi::Collection leadCollection;
    leadCollection.setRemoteId( moduleName() );
    leadCollection.setContentMimeTypes( QStringList() << SugarLead::mimeType() );
    leadCollection.setName( i18nc( "@item folder name", "Leads" ) );
    leadCollection.setRights( Akonadi::Collection::CanChangeItem |
                                 Akonadi::Collection::CanCreateItem |
                                 Akonadi::Collection::CanDeleteItem );

    return leadCollection;
}

void LeadsHandler::listEntries( const ListEntriesScope &scope )
{
    const QString query = scope.query( QLatin1String( "leads" ) );
    const QString orderBy = QLatin1String( "leads.last_name" );
    const int offset = scope.offset();
    const int maxResults = 100;
    const int fetchDeleted = scope.deleted();

    TNS__Select_fields selectedFields;
    selectedFields.setItems( mAccessors->keys() );

    soap()->asyncGet_entry_list( sessionId(), moduleName(), query, orderBy, offset, selectedFields, maxResults, fetchDeleted );
}

bool LeadsHandler::setEntry( const Akonadi::Item &item )
{
    if ( !item.hasPayload<SugarLead>() ) {
        kError() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Lead payload";
        return false;
    }

    QList<TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the lead being added
    if ( !item.remoteId().isEmpty() ) {
        TNS__Name_value field;
        field.setName( QLatin1String( "id" ) );
        field.setValue( item.remoteId() );
        itemList << field;
    }

    const SugarLead lead = item.payload<SugarLead>();
    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( (*it)->getter == 0 ) {
            continue;
        }
        TNS__Name_value field;
        field.setName( it.key() );
        field.setValue( (*it)->getter( lead ) );

        itemList << field;
    }

    TNS__Name_value_list valueList;
    valueList.setItems( itemList );
    soap()->asyncSet_entry( sessionId(), moduleName(), valueList );

    return true;
}

Akonadi::Item LeadsHandler::itemFromEntry( const TNS__Entry_value &entry, const Akonadi::Collection &parentCollection )
{
    Akonadi::Item item;

    const QList<TNS__Name_value> valueList = entry.name_value_list().items();
    if ( valueList.isEmpty() ) {
        kWarning() << "Leads entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId( entry.id() );
    item.setParentCollection( parentCollection );
    item.setMimeType( SugarLead::mimeType() );

    SugarLead lead;
    lead.setId( entry.id() );
    Q_FOREACH( const TNS__Name_value &namedValue, valueList ) {
        const AccessorHash::const_iterator accessIt = mAccessors->constFind( namedValue.name() );
        if ( accessIt == mAccessors->constEnd() ) {
            // no accessor for field
            continue;
        }

        (*accessIt)->setter( namedValue.value(), lead );
    }
    item.setPayload<SugarLead>( lead );
    item.setRemoteRevision( getDateModified( lead ) );

    return item;
}

void LeadsHandler::compare( Akonadi::AbstractDifferencesReporter *reporter,
                            const Akonadi::Item &leftItem, const Akonadi::Item &rightItem )
{
    Q_ASSERT( leftItem.hasPayload<SugarLead>() );
    Q_ASSERT( rightItem.hasPayload<SugarLead>() );

    const SugarLead leftLead = leftItem.payload<SugarLead>();
    const SugarLead rightLead = rightItem.payload<SugarLead>();

    const QString modifiedBy = getModifiedByName( rightLead );
    const QString modifiedOn = formatDate( getDateModified( rightLead ) );

    reporter->setLeftPropertyValueTitle( i18nc( "@title:column", "Local Lead" ) );
    reporter->setRightPropertyValueTitle(
        i18nc( "@title:column", "Serverside Lead: modified by %1 on %2",
               modifiedBy, modifiedOn ) );

    bool seenPrimaryAddress = false;
    bool seenOtherAddress = false;
    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( (*it)->getter == 0 ) {
            continue;
        }

        QString leftValue = (*it)->getter( leftLead );
        QString rightValue = (*it)->getter( rightLead );

        QString diffName = (*it)->diffName;
        if ( diffName.isEmpty() ) {
            // check for special fields
            if ( isAddressValue( it.key() ) ) {
                if ( isPrimaryAddressValue( it.key() ) ) {
                    if ( !seenPrimaryAddress ) {
                        seenPrimaryAddress = true;
                        diffName = i18nc( "@item:intable", "Primary Address" );

                        KABC::Address leftAddress( KABC::Address::Work | KABC::Address::Pref );
                        leftAddress.setStreet( getPrimaryAddressStreet( leftLead ) );
                        leftAddress.setLocality( getPrimaryAddressCity( leftLead ) );
                        leftAddress.setRegion( getPrimaryAddressState( leftLead ) );
                        leftAddress.setCountry( getPrimaryAddressCountry( leftLead ) );
                        leftAddress.setPostalCode( getPrimaryAddressPostalcode( leftLead ) );

                        KABC::Address rightAddress( KABC::Address::Work | KABC::Address::Pref );
                        rightAddress.setStreet( getPrimaryAddressStreet( rightLead ) );
                        rightAddress.setLocality( getPrimaryAddressCity( rightLead ) );
                        rightAddress.setRegion( getPrimaryAddressState( rightLead ) );
                        rightAddress.setCountry( getPrimaryAddressCountry( rightLead ) );
                        rightAddress.setPostalCode( getPrimaryAddressPostalcode( rightLead ) );

                        leftValue = leftAddress.formattedAddress();
                        rightValue = rightAddress.formattedAddress();
                    } else {
                        // already printed, skip
                        continue;
                    }
                } else {
                    if ( !seenOtherAddress ) {
                        seenOtherAddress = true;
                        diffName = i18nc( "@item:intable", "Other Address" );

                        KABC::Address leftAddress( KABC::Address::Home );
                        leftAddress.setStreet( getAltAddressStreet( leftLead ) );
                        leftAddress.setLocality( getAltAddressCity( leftLead ) );
                        leftAddress.setRegion( getAltAddressState( leftLead ) );
                        leftAddress.setCountry( getAltAddressCountry( leftLead ) );
                        leftAddress.setPostalCode( getAltAddressPostalcode( leftLead ) );

                        KABC::Address rightAddress( KABC::Address::Home );
                        rightAddress.setStreet( getAltAddressStreet( rightLead ) );
                        rightAddress.setLocality( getAltAddressCity( rightLead ) );
                        rightAddress.setRegion( getAltAddressState( rightLead ) );
                        rightAddress.setCountry( getAltAddressCountry( rightLead ) );
                        rightAddress.setPostalCode( getAltAddressPostalcode( rightLead ) );

                        leftValue = leftAddress.formattedAddress();
                        rightValue = rightAddress.formattedAddress();
                    } else {
                        // already printed, skip
                        continue;
                    }
                }
            } else if ( it.key() == "do_not_call" ) {
                diffName = i18nc( "@item:intable", "Do Not Call" );
                leftValue = getDoNotCall( leftLead ) == QLatin1String( "1" )
                                ? QLatin1String( "Yes" ) : QLatin1String( "No" );
                rightValue = getDoNotCall( rightLead ) == QLatin1String( "1" )
                                ? QLatin1String( "Yes" ) : QLatin1String( "No" );
            } else if ( it.key() == "converted" ) {
                diffName = i18nc( "@item:intable", "Converted" );
                leftValue = getConverted( leftLead ) == QLatin1String( "1" )
                                ? QLatin1String( "Yes" ) : QLatin1String( "No" );
                rightValue = getConverted( rightLead ) == QLatin1String( "1" )
                                ? QLatin1String( "Yes" ) : QLatin1String( "No" );
            } else {
                // internal field, skip
                continue;
            }
        }

        if ( leftValue.isEmpty() && rightValue.isEmpty() ) {
            continue;
        }

        if ( leftValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalRightMode,
                                   diffName, leftValue, rightValue );
        } else if ( rightValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalLeftMode,
                                   diffName, leftValue, rightValue );
        } else if ( leftValue != rightValue ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::ConflictMode,
                                   diffName, leftValue, rightValue );
        }
    }
}
