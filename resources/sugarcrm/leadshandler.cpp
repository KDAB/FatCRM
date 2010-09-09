#include "leadshandler.h"

#include "sugarsoap.h"

#include <akonadi/abstractdifferencesreporter.h>
#include <akonadi/collection.h>

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
    AccessorPair( valueGetter get, valueSetter set ) : getter( get ), setter( set ){}

public:
    valueGetter getter;
    valueSetter setter;
};



LeadsHandler::LeadsHandler()
    : ModuleHandler( QLatin1String( "Leads" ) ),
      mAccessors( new AccessorHash )
{
    mAccessors->insert( QLatin1String( "id" ), AccessorPair( 0, setId ) );
    mAccessors->insert( QLatin1String( "date_entered" ), AccessorPair( getDateEntered, setDateEntered ) );
    mAccessors->insert( QLatin1String( "date_modified" ), AccessorPair( getDateModified, setDateModified ) );
    mAccessors->insert( QLatin1String( "modified_user_id" ), AccessorPair( getModifiedUserId, setModifiedUserId ) );
    mAccessors->insert( QLatin1String( "modified_by_name" ), AccessorPair( getModifiedByName, setModifiedByName ) );
    mAccessors->insert( QLatin1String( "created_by" ), AccessorPair( getCreatedBy, setCreatedBy ) );
    mAccessors->insert( QLatin1String( "created_by_name" ), AccessorPair( getCreatedByName, setCreatedByName ) );
    mAccessors->insert( QLatin1String( "description" ), AccessorPair( getDescription, setDescription ) );
    mAccessors->insert( QLatin1String( "deleted" ), AccessorPair( getDeleted, setDeleted ) );
    mAccessors->insert( QLatin1String( "assigned_user_id" ), AccessorPair( getAssignedUserId, setAssignedUserId ) );
    mAccessors->insert( QLatin1String( "assigned_user_name" ), AccessorPair( getAssignedUserName, setAssignedUserName ) );
    mAccessors->insert( QLatin1String( "salutation" ), AccessorPair( getSalutation, setSalutation ) );
    mAccessors->insert( QLatin1String( "first_name" ), AccessorPair( getFirstName, setFirstName ) );
    mAccessors->insert( QLatin1String( "last_name" ), AccessorPair( getLastName, setLastName ) );
    mAccessors->insert( QLatin1String( "title" ), AccessorPair( getTitle, setTitle ) );
    mAccessors->insert( QLatin1String( "department" ), AccessorPair( getDepartment, setDepartment ) );
    mAccessors->insert( QLatin1String( "do_not_call" ), AccessorPair( getDoNotCall, setDoNotCall ) );
    mAccessors->insert( QLatin1String( "phone_home" ), AccessorPair( getPhoneHome, setPhoneHome ) );
    mAccessors->insert( QLatin1String( "phone_mobile" ), AccessorPair( getPhoneMobile, setPhoneMobile ) );
    mAccessors->insert( QLatin1String( "phone_work" ), AccessorPair( getPhoneWork, setPhoneWork ) );
    mAccessors->insert( QLatin1String( "phone_other" ), AccessorPair( getPhoneOther, setPhoneOther ) );
    mAccessors->insert( QLatin1String( "phone_fax" ), AccessorPair( getPhoneFax, setPhoneFax ) );
    mAccessors->insert( QLatin1String( "email1" ), AccessorPair( getEmail1, setEmail1 ) );
    mAccessors->insert( QLatin1String( "email2" ), AccessorPair( getEmail2, setEmail2 ) );
    mAccessors->insert( QLatin1String( "primary_address_street" ), AccessorPair( getPrimaryAddressStreet, setPrimaryAddressStreet ) );
    mAccessors->insert( QLatin1String( "primary_address_city" ), AccessorPair( getPrimaryAddressCity, setPrimaryAddressCity ) );
    mAccessors->insert( QLatin1String( "primary_address_state" ), AccessorPair( getPrimaryAddressState, setPrimaryAddressState ) );
    mAccessors->insert( QLatin1String( "primary_address_postalcode" ), AccessorPair( getPrimaryAddressPostalcode, setPrimaryAddressPostalcode ) );
    mAccessors->insert( QLatin1String( "primary_address_country" ), AccessorPair( getPrimaryAddressCountry, setPrimaryAddressCountry ) );
    mAccessors->insert( QLatin1String( "alt_address_street" ), AccessorPair( getAltAddressStreet, setAltAddressStreet ) );
    mAccessors->insert( QLatin1String( "alt_address_city" ), AccessorPair( getAltAddressCity, setAltAddressCity ) );
    mAccessors->insert( QLatin1String( "alt_address_state" ), AccessorPair( getAltAddressState, setAltAddressState ) );
    mAccessors->insert( QLatin1String( "alt_address_postalcode" ), AccessorPair( getAltAddressPostalcode, setAltAddressPostalcode ) );
    mAccessors->insert( QLatin1String( "alt_address_country" ), AccessorPair( getAltAddressCountry, setAltAddressCountry ) );
    mAccessors->insert( QLatin1String( "assistant" ), AccessorPair( getAssistant, setAssistant ) );
    mAccessors->insert( QLatin1String( "assistant_phone" ), AccessorPair( getAssistantPhone, setAssistantPhone ) );
    mAccessors->insert( QLatin1String( "converted" ), AccessorPair( getConverted, setConverted ) );
    mAccessors->insert( QLatin1String( "refered_by" ), AccessorPair( getReferedBy, setReferedBy ) );
    mAccessors->insert( QLatin1String( "lead_source" ), AccessorPair( getLeadSource, setLeadSource ) );
    mAccessors->insert( QLatin1String( "lead_source_description" ), AccessorPair( getLeadSourceDescription, setLeadSourceDescription ) );
    mAccessors->insert( QLatin1String( "status" ), AccessorPair( getStatus, setStatus ) );
    mAccessors->insert( QLatin1String( "status_description" ), AccessorPair( getStatusDescription, setStatusDescription ) );
    mAccessors->insert( QLatin1String( "reports_to_id" ), AccessorPair( getReportsToId, setReportsToId ) );
    mAccessors->insert( QLatin1String( "report_to_name" ), AccessorPair( getReportToName, setReportToName ) );
    mAccessors->insert( QLatin1String( "account_name" ), AccessorPair( getAccountName, setAccountName ) );
    mAccessors->insert( QLatin1String( "account_description" ), AccessorPair( getAccountDescription, setAccountDescription ) );
    mAccessors->insert( QLatin1String( "contact_id" ), AccessorPair( getContactId, setContactId ) );
    mAccessors->insert( QLatin1String( "account_id" ), AccessorPair( getAccountId, setAccountId ) );
    mAccessors->insert( QLatin1String( "opportunity_id" ), AccessorPair( getOpportunityId, setOpportunityId ) );
    mAccessors->insert( QLatin1String( "opportunity_name" ), AccessorPair( getOpportunityName, setOpportunityName ) );
    mAccessors->insert( QLatin1String( "opportunity_amount" ), AccessorPair( getOpportunityAmount, setOpportunityAmount ) );
    mAccessors->insert( QLatin1String( "campaign_id" ), AccessorPair( getCampaignId, setCampaignId ) );
    mAccessors->insert( QLatin1String( "campaign_name" ), AccessorPair( getCampaignName, setCampaignName ) );
    mAccessors->insert( QLatin1String( "c_accept_status_fields" ), AccessorPair( getCAcceptStatusFields, setCAcceptStatusFields ) );
    mAccessors->insert( QLatin1String( "m_accept_status_fields" ), AccessorPair( getMAcceptStatusFields, setMAcceptStatusFields ) );
    mAccessors->insert( QLatin1String( "birthdate" ), AccessorPair( getBirthdate, setBirthdate ) );
    mAccessors->insert( QLatin1String( "portal_name" ), AccessorPair( getPortalName, setPortalName ) );
    mAccessors->insert( QLatin1String( "portal_app" ), AccessorPair( getPortalApp, setPortalApp ) );
}

LeadsHandler::~LeadsHandler()
{
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

void LeadsHandler::listEntries( int offset, Sugarsoap* soap, const QString &sessionId )
{
    const QString query = QLatin1String( "" );
    const QString orderBy = QLatin1String( "leads.last_name" );
    const int maxResults = 100;
    const int fetchDeleted = 0; // do not fetch deleted items

    TNS__Select_fields selectedFields;
    selectedFields.setItems( mAccessors->keys() );

    soap->asyncGet_entry_list( sessionId, moduleName(), query, orderBy, offset, selectedFields, maxResults, fetchDeleted );
}

bool LeadsHandler::setEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId )
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
        if ( it->getter == 0 ) {
            continue;
        }
        TNS__Name_value field;
        field.setName( it.key() );
        field.setValue( it->getter( lead ) );
        if ( field.name() == "date_modified"  ||
             field.name() == "date_entered" )
            field.setValue( adjustedTime( field.value() ) );
        itemList << field;
    }

    TNS__Name_value_list valueList;
    valueList.setItems( itemList );
    soap->asyncSet_entry( sessionId, moduleName(), valueList );

    return true;
}

Akonadi::Item::List LeadsHandler::itemsFromListEntriesResponse( const TNS__Entry_list &entryList,
                                                                   const Akonadi::Collection &parentCollection )
{
    Akonadi::Item::List items;

    Q_FOREACH( const TNS__Entry_value &entry, entryList.items() ) {
        const QList<TNS__Name_value> valueList = entry.name_value_list().items();
        if ( valueList.isEmpty() ) {
            kWarning() << "Leads entry for id=" << entry.id() << "has no values";
            continue;
        }

        Akonadi::Item item;
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

            // adjust time to local system
            if ( namedValue.name() == "date_modified" ||
                 namedValue.name() == "date_entered" ) {
                accessIt->setter( adjustedTime(namedValue.value()), lead );
                continue;
            }
            accessIt->setter( namedValue.value(), lead );
        }
        item.setPayload<SugarLead>( lead );
        item.setRemoteRevision( getDateModified( lead ) );
        items << item;
    }

    return items;
}

void LeadsHandler::compare( Akonadi::AbstractDifferencesReporter *reporter,
                            const Akonadi::Item &leftItem, const Akonadi::Item &rightItem )
{
    Q_ASSERT( leftItem.hasPayload<SugarLead>() );
    Q_ASSERT( rightItem.hasPayload<SugarLead>() );

    reporter->setLeftPropertyValueTitle( i18nc( "@title:column", "Local Lead" ) );
    reporter->setRightPropertyValueTitle( i18nc( "@title:column", "Serverside Lead" ) );

    const SugarLead leftLead = leftItem.payload<SugarLead>();
    const SugarLead rightLead = rightItem.payload<SugarLead>();

    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( it->getter == 0 ) {
            continue;
        }

        const QString leftValue = it->getter( leftLead );
        const QString rightValue = it->getter( rightLead );

        if ( leftValue.isEmpty() && rightValue.isEmpty() ) {
            continue;
        }

        if ( leftValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalRightMode,
                                   it.key(), leftValue, rightValue );
        } else if ( rightValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalLeftMode,
                                   it.key(), leftValue, rightValue );
        } else if ( leftValue == rightValue ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::NormalMode,
                                   it.key(), leftValue, rightValue );
        } else {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::ConflictMode,
                                   it.key(), leftValue, rightValue );
        }
    }
}

QString LeadsHandler::adjustedTime( const QString dateTime ) const
{
    QVariant var = QVariant( dateTime );
    QDateTime dt = var.toDateTime();
    QDateTime system =  QDateTime::currentDateTime();
    QDateTime utctime( system );
    utctime.setTimeSpec( Qt::OffsetFromUTC );
    return dt.addSecs( system.secsTo( utctime ) ).toString("yyyy-MM-dd hh:mm");
}
