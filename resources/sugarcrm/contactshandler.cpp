#include "contactshandler.h"

#include "sugarsoap.h"

#include <akonadi/abstractdifferencesreporter.h>
#include <akonadi/collection.h>

#include <kabc/addressee.h>
#include <kabc/address.h>

#include <KLocale>

#include <QHash>

typedef QString (*valueGetter)( const KABC::Addressee& );
typedef void (*valueSetter)( const QString&, KABC::Addressee&);
typedef void (*addressSetter)( const QString&, KABC::Address& );

static QString getFirstName( const KABC::Addressee &addressee )
{
    return addressee.givenName();
}

static void setFirstName( const QString &value, KABC::Addressee &addressee )
{
    addressee.setGivenName( value );
}

static QString getLastName( const KABC::Addressee &addressee )
{
    return addressee.familyName();
}

static void setLastName( const QString &value, KABC::Addressee &addressee )
{
    addressee.setFamilyName( value );
}

static QString getTitle( const KABC::Addressee &addressee )
{
    return addressee.title();
}

static void setTitle( const QString &value, KABC::Addressee &addressee )
{
    addressee.setTitle( value );
}

static QString getDepartment( const KABC::Addressee &addressee )
{
    return addressee.department();
}

static void setDepartment( const QString &value, KABC::Addressee &addressee )
{
    addressee.setDepartment( value );
}

static QString getAccountName( const KABC::Addressee &addressee )
{
    return addressee.organization();
}

static void setAccountName( const QString &value, KABC::Addressee &addressee )
{
    addressee.setOrganization( value );
}

static QString getAccountId( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-AccountId" );
}

static void setAccountId( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "FATCRM", "X-AccountId", value );
}

static QString getAssistantName( const KABC::Addressee &addressee )
{
    return addressee.custom( "KADDRESSBOOK", "X-AssistantsName" );
}

static void setAssitantName( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "KADDRESSBOOK", "X-AssistantsName", value );
}

static QString getAssistantPhone( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-AssistantsPhone" );
}

static void setAssistantPhone( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "FATCRM", "X-AssistantsPhone", value );
}

static QString getLeadSourceName( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-LeadSourceName" );
}

static void setLeadSourceName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-LeadSourceName", value );
}

static QString getCampaignName( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-CampaignName" );
}

static void setCampaignName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-CampaignName",value );
}

static QString getCampaignId( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-CampaignId" );
}

static void setCampaignId(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-CampaignId",value );
}

static QString getAssignedUserName( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-AssignedUserName" );
}

static void setAssignedUserName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-AssignedUserName", value );
}

static QString getAssignedUserId( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-AssignedUserId" );
}

static void setAssignedUserId(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-AssignedUserId", value );
}

static QString getReportsToUserName( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-ReportsToUserName" );
}

static void setReportsToUserName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-ReportsToUserName", value );
}

static QString getReportsToUserId( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-ReportsToUserId" );
}

static void setReportsToUserId(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-ReportsToUserId", value );
}

static QString getModifiedByName( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-ModifiedByName" );
}

static void setModifiedByName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-ModifiedByName", value );
}

static QString getDateModified( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-DateModified" );
}

static void setDateModified(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-DateModified", value );
}

static QString getModifiedUserName( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-ModifiedUserName" );
}

static void setModifiedUserName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-ModifiedUserName", value );
}

static QString getModifiedUserId( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-ModifiedUserId" );
}

static void setModifiedUserId(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-ModifiedUserId", value );
}

static QString getDateCreated( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-DateCreated" );
}

static void setDateCreated(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-DateCreated", value );
}

static void setContactId(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-ContactId", value );
}

static QString getCreatedByName( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-CreatedByName" );
}

static void setCreatedByName(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-CreatedByName", value );
}

static QString getCreatedById( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-CreatedById" );
}

static void setCreatedById(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-CreatedById", value );
}

static QString getSalutation( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-Salutation" );
}

static void setSalutation(const QString &value, KABC::Addressee &addressee)
{
    addressee.insertCustom( "FATCRM", "X-Salutation", value );
}

static QString getOpportunityRoleFields( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-OpportunityRoleFields" );
}

static void setOpportunityRoleFields( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "FATCRM","X-OpportunityRoleFields", value );
}

static QString getCAcceptStatusFields( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-CacceptStatusFields" );
}

static void setCAcceptStatusFields( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "FATCRM","X-CacceptStatusFields", value );
}

static QString getMAcceptStatusFields( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-MacceptStatusFields" );
}

static void setMAcceptStatusFields( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "FATCRM","X-MacceptStatusFields", value );
}

static QString getDeleted( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-Deleted" );
}

static void setDeleted (const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "FATCRM","X-Deleted", value );
}

static QString getDoNotCall( const KABC::Addressee &addressee )
{
    return addressee.custom( "FATCRM", "X-DoNotCall" );
}

static void setDoNotCall ( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertCustom( "FATCRM","X-DoNotCall", value );
}

static QString getNote( const KABC::Addressee &addressee )
{
    return addressee.note();
}

static void setNote( const QString &value, KABC::Addressee &addressee )
{
    addressee.setNote( value );
}

static QString getBirthday( const KABC::Addressee &addressee )
{
    return addressee.birthday().toString( QString("yyyy-MM-dd" ) );
}

static void setBirthday( const QString &value, KABC::Addressee &addressee )
{
    addressee.setBirthday( QDateTime::fromString( value, QString("yyyy-MM-dd" ) ) );
}

static QString getEmail1( const KABC::Addressee &addressee )
{
    return addressee.preferredEmail();
}

static void setEmail1( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertEmail( value, true );
}

static QString getEmail2( const KABC::Addressee &addressee )
{
    // preferred might not be the first one, so remove it and take the first of
    // the remaining instead of always taking the second one
    QStringList emails = addressee.emails();
    emails.removeAll( addressee.preferredEmail() );
    if ( emails.count() >= 1 ) {
        return emails[ 0 ];
    }

    return QString();
}

static void setEmail2( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertEmail( value, false );
}

static QString getHomePhone( const KABC::Addressee &addressee )
{
    return addressee.phoneNumber( KABC::PhoneNumber::Home ).number();
}

static void setHomePhone( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertPhoneNumber( KABC::PhoneNumber( value, KABC::PhoneNumber::Home ) );
}

static QString getWorkPhone( const KABC::Addressee &addressee )
{
    return addressee.phoneNumber( KABC::PhoneNumber::Work ).number();
}

static void setWorkPhone( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertPhoneNumber( KABC::PhoneNumber( value, KABC::PhoneNumber::Work ) );
}

static QString getMobilePhone( const KABC::Addressee &addressee )
{
    return addressee.phoneNumber( KABC::PhoneNumber::Cell ).number();
}

static void setMobilePhone( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertPhoneNumber( KABC::PhoneNumber( value, KABC::PhoneNumber::Cell ) );
}

// Pending (michel)
// we need to decide what other phone will be
static QString getOtherPhone( const KABC::Addressee &addressee )
{
    return addressee.phoneNumber( KABC::PhoneNumber::Car ).number();
}

static void setOtherPhone( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertPhoneNumber( KABC::PhoneNumber( value, KABC::PhoneNumber::Car ) );
}

static QString getFaxPhone( const KABC::Addressee &addressee )
{
    return addressee.phoneNumber( KABC::PhoneNumber::Work|KABC::PhoneNumber::Fax ).number();
}

static void setFaxPhone( const QString &value, KABC::Addressee &addressee )
{
    addressee.insertPhoneNumber( KABC::PhoneNumber( value, KABC::PhoneNumber::Work|KABC::PhoneNumber::Fax ) );
}

static QString getPrimaryStreet( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Work).street();
}

static void setPrimaryStreet(const QString &value, KABC::Address &address )
{
    address.setStreet( value );
}

static QString getPrimaryCity( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Work|KABC::Address::Pref).locality();
}

static void setPrimaryCity(const QString &value, KABC::Address &address )
{
    address.setLocality( value );
}

static QString getPrimaryState( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Work|KABC::Address::Pref).region();
}

static void setPrimaryState( const QString &value, KABC::Address &address )
{
    address.setRegion( value );
}

static QString getPrimaryPostalcode( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Work|KABC::Address::Pref).postalCode();
}

static void setPrimaryPostalcode( const QString &value, KABC::Address &address )
{
    address.setPostalCode( value );
}

static QString getPrimaryCountry( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Work|KABC::Address::Pref).country();
}

static void setPrimaryCountry( const QString &value, KABC::Address &address )
{
    address.setCountry( value );
}

static QString getOtherStreet( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Home).street();
}

static void setOtherStreet(const QString &value, KABC::Address &address )
{
    address.setStreet( value );
}

static QString getOtherCity( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Home).locality();
}

static void setOtherCity(const QString &value, KABC::Address &address )
{
    address.setLocality( value );
}

static QString getOtherState( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Home).region();
}

static void setOtherState( const QString &value, KABC::Address &address )
{
    address.setRegion( value );
}

static QString getOtherPostalcode( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Home).postalCode();
}

static void setOtherPostalcode( const QString &value, KABC::Address &address )
{
    address.setPostalCode( value );
}

static QString getOtherCountry( const KABC::Addressee &addressee )
{
    return addressee.address(KABC::Address::Home).country();
}

static void setOtherCountry( const QString &value, KABC::Address &address )
{
    address.setCountry( value );
}

class AccessorPair
{
public:
    AccessorPair( valueGetter get, valueSetter set ) : getter( get ) { setter.vSetter = set; }

    AccessorPair( valueGetter get, addressSetter set ): getter( get ){ setter.aSetter = set; }

public:
    valueGetter getter;
    union tsetter {
        valueSetter vSetter;
        addressSetter aSetter;
    } setter;
};

ContactsHandler::ContactsHandler()
    : ModuleHandler( QLatin1String( "Contacts" ) ),
      mAccessors( new AccessorHash )
{
    mAccessors->insert( QLatin1String( "first_name" ), AccessorPair( getFirstName, setFirstName ) );
    mAccessors->insert( QLatin1String( "last_name" ), AccessorPair( getLastName, setLastName ) );
    mAccessors->insert( QLatin1String( "email1" ), AccessorPair( getEmail1, setEmail1 ) );
    mAccessors->insert( QLatin1String( "email2" ), AccessorPair( getEmail2, setEmail2 ) );
    mAccessors->insert( QLatin1String( "title" ), AccessorPair( getTitle, setTitle ) );
    mAccessors->insert( QLatin1String( "department" ), AccessorPair( getDepartment, setDepartment ) );
    mAccessors->insert( QLatin1String( "account_name" ), AccessorPair( getAccountName, setAccountName ) );
    mAccessors->insert( QLatin1String( "account_id" ), AccessorPair( getAccountId, setAccountId ) );
    mAccessors->insert( QLatin1String( "phone_home" ), AccessorPair( getHomePhone, setHomePhone ) );
    mAccessors->insert( QLatin1String( "phone_work" ), AccessorPair( getWorkPhone, setWorkPhone ) );
    mAccessors->insert( QLatin1String( "phone_mobile" ), AccessorPair( getMobilePhone, setMobilePhone ) );
    mAccessors->insert( QLatin1String( "phone_other" ), AccessorPair( getOtherPhone, setOtherPhone ) );
    mAccessors->insert( QLatin1String( "phone_fax" ), AccessorPair( getFaxPhone, setFaxPhone ) );
    mAccessors->insert( QLatin1String( "primary_address_street" ), AccessorPair( getPrimaryStreet, setPrimaryStreet ) );
    mAccessors->insert( QLatin1String( "primary_address_city" ), AccessorPair( getPrimaryCity, setPrimaryCity ) );
    mAccessors->insert( QLatin1String( "primary_address_state" ), AccessorPair( getPrimaryState, setPrimaryState ) );
    mAccessors->insert( QLatin1String( "primary_address_postalcode" ), AccessorPair( getPrimaryPostalcode, setPrimaryPostalcode ) );
    mAccessors->insert( QLatin1String( "primary_address_country" ), AccessorPair( getPrimaryCountry, setPrimaryCountry ) );
    mAccessors->insert( QLatin1String( "alt_address_street" ), AccessorPair( getOtherStreet, setOtherStreet ) );
    mAccessors->insert( QLatin1String( "alt_address_city" ), AccessorPair( getOtherCity, setOtherCity ) );
    mAccessors->insert( QLatin1String( "alt_address_state" ), AccessorPair( getOtherState, setOtherState ) );
    mAccessors->insert( QLatin1String( "alt_address_postalcode" ), AccessorPair( getOtherPostalcode, setOtherPostalcode ) );
    mAccessors->insert( QLatin1String( "alt_address_country" ), AccessorPair( getOtherCountry, setOtherCountry ) );
    mAccessors->insert( QLatin1String( "birthdate" ), AccessorPair( getBirthday, setBirthday ) );
    mAccessors->insert( QLatin1String( "description" ), AccessorPair( getNote, setNote ) );
    mAccessors->insert( QLatin1String( "assistant" ), AccessorPair( getAssistantName, setAssitantName ) );
    mAccessors->insert( QLatin1String( "assistant_phone" ), AccessorPair( getAssistantPhone, setAssistantPhone ) );
    mAccessors->insert( QLatin1String( "lead_source" ), AccessorPair( getLeadSourceName, setLeadSourceName ) );
    mAccessors->insert( QLatin1String( "campaign_name" ), AccessorPair( getCampaignName, setCampaignName ) );
    mAccessors->insert( QLatin1String( "campaign_id" ), AccessorPair( getCampaignId, setCampaignId ) );
    mAccessors->insert( QLatin1String( "assigned_user_name" ), AccessorPair( getAssignedUserName, setAssignedUserName ) );
    mAccessors->insert( QLatin1String( "assigned_user_id" ), AccessorPair( getAssignedUserId, setAssignedUserId ) );
    mAccessors->insert( QLatin1String( "report_to_name" ), AccessorPair( getReportsToUserName, setReportsToUserName ) );
    mAccessors->insert( QLatin1String( "reports_to_id" ), AccessorPair( getReportsToUserId, setReportsToUserId ) );
    mAccessors->insert( QLatin1String( "modified_by_name" ), AccessorPair( getModifiedByName, setModifiedByName ) );
    mAccessors->insert( QLatin1String( "date_modified" ), AccessorPair( getDateModified, setDateModified ) );
    mAccessors->insert( QLatin1String( "modified_user_id" ), AccessorPair( getModifiedUserId, setModifiedUserId ) );
    mAccessors->insert( QLatin1String( "modified_user_name" ), AccessorPair( getModifiedUserName, setModifiedUserName ) );
    mAccessors->insert( QLatin1String( "date_entered" ), AccessorPair( getDateCreated, setDateCreated ) );
    mAccessors->insert( QLatin1String( "id" ), AccessorPair( 0, setContactId ) );
    mAccessors->insert( QLatin1String( "created_by_name" ), AccessorPair( getCreatedByName, setCreatedByName ) );
    mAccessors->insert( QLatin1String( "created_by" ), AccessorPair( getCreatedById, setCreatedById ) );
    mAccessors->insert( QLatin1String( "salutation" ), AccessorPair( getSalutation, setSalutation ) );
    mAccessors->insert( QLatin1String( "opportunity_role_fields" ), AccessorPair( getOpportunityRoleFields, setOpportunityRoleFields ) );
    mAccessors->insert( QLatin1String( "c_accept_status_fields" ),AccessorPair( getCAcceptStatusFields, setCAcceptStatusFields ) );
    mAccessors->insert( QLatin1String( "m_accept_status_fields" ),AccessorPair( getMAcceptStatusFields, setMAcceptStatusFields ) );
    mAccessors->insert( QLatin1String( "deleted" ),AccessorPair( getDeleted, setDeleted ) );
    mAccessors->insert( QLatin1String( "do_not_call" ),AccessorPair( getDoNotCall, setDoNotCall ) );
}

ContactsHandler::~ContactsHandler()
{
    delete mAccessors;
}

QStringList ContactsHandler::supportedFields() const
{
    return mAccessors->keys();
}

Akonadi::Collection ContactsHandler::collection() const
{
    Akonadi::Collection contactCollection;
    contactCollection.setRemoteId( moduleName() );
    contactCollection.setContentMimeTypes( QStringList() << KABC::Addressee::mimeType() );
    contactCollection.setName( i18nc( "@item folder name", "Contacts" ) );
    contactCollection.setRights( Akonadi::Collection::CanChangeItem |
                                 Akonadi::Collection::CanCreateItem |
                                 Akonadi::Collection::CanDeleteItem );

    return contactCollection;
}

void ContactsHandler::listEntries( int offset, Sugarsoap* soap, const QString &sessionId )
{
    const QString query = QLatin1String( "" );
    const QString orderBy = QLatin1String( "contacts.last_name" );
    const int maxResults = 100;
    const int fetchDeleted = 0; // do not fetch deleted items

    TNS__Select_fields selectedFields;
    selectedFields.setItems( mAccessors->keys() );

    soap->asyncGet_entry_list( sessionId, moduleName(), query, orderBy, offset, selectedFields, maxResults, fetchDeleted );
}

bool ContactsHandler::setEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId )
{
    if ( !item.hasPayload<KABC::Addressee>() ) {
        kError() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Addressee payload";
        return false;
    }

    QList<TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the contact being added
    if ( !item.remoteId().isEmpty() ) {
        TNS__Name_value field;
        field.setName( QLatin1String( "id" ) );
        field.setValue( item.remoteId() );

        itemList << field;
    }

    const KABC::Addressee addressee = item.payload<KABC::Addressee>();
    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( it->getter == 0 ) {
            continue;
        }
        TNS__Name_value field;
        field.setName( it.key() );
        field.setValue( it->getter( addressee ) );
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

Akonadi::Item::List ContactsHandler::itemsFromListEntriesResponse( const TNS__Entry_list &entryList,
                                                                   const Akonadi::Collection &parentCollection )
{
    Akonadi::Item::List items;

    Q_FOREACH( const TNS__Entry_value &entry, entryList.items() ) {
        const QList<TNS__Name_value> valueList = entry.name_value_list().items();
        if ( valueList.isEmpty() ) {
            kWarning() << "Contacts entry for id=" << entry.id() << "has no values";
            continue;
        }

        Akonadi::Item item;
        item.setRemoteId( entry.id() );
        item.setParentCollection( parentCollection );
        item.setMimeType( KABC::Addressee::mimeType() );

        KABC::Addressee addressee;
        addressee.setUid( entry.id() );
        KABC::Address workAddress,  homeAddress;
        workAddress.setType( KABC::Address::Work | KABC::Address::Pref );
        homeAddress.setType( KABC::Address::Home );

        Q_FOREACH( const TNS__Name_value &namedValue, valueList ) {
            const AccessorHash::const_iterator accessIt = mAccessors->constFind( namedValue.name() );
            if ( accessIt == mAccessors->constEnd() ) {
                // no accessor for field
                continue;
            }

            // adjust time to local system
            if ( namedValue.name() == "date_modified" ||
                 namedValue.name() == "date_entered" ) {
                accessIt->setter.vSetter( adjustedTime(namedValue.value()), addressee );
                continue;
            }

            if ( isAddressValue( namedValue.name() ) ) {
                KABC::Address& address =
                    isPrimaryAddressValue( namedValue.name() ) ? workAddress : homeAddress;
                accessIt->setter.aSetter( namedValue.value(), address );
            } else {
                accessIt->setter.vSetter( namedValue.value(), addressee );
            }
        }
        addressee.insertAddress( workAddress );
        addressee.insertAddress( homeAddress );
        item.setPayload<KABC::Addressee>( addressee );
        item.setRemoteRevision( getDateModified( addressee ) );
        items << item;
    }

    return items;
}

void ContactsHandler::compare( Akonadi::AbstractDifferencesReporter *reporter,
                               const Akonadi::Item &leftItem, const Akonadi::Item &rightItem )
{
    Q_ASSERT( leftItem.hasPayload<KABC::Addressee>() );
    Q_ASSERT( rightItem.hasPayload<KABC::Addressee>() );

    reporter->setLeftPropertyValueTitle( i18nc( "@title:column", "Local Contact" ) );
    reporter->setRightPropertyValueTitle( i18nc( "@title:column", "Serverside Contact" ) );

    const KABC::Addressee leftContact = leftItem.payload<KABC::Addressee>();
    const KABC::Addressee rightContact = rightItem.payload<KABC::Addressee>();

    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( it->getter == 0 ) {
            continue;
        }

        const QString leftValue = it->getter( leftContact );
        const QString rightValue = it->getter( rightContact );

        if ( leftValue.isEmpty() && rightValue.isEmpty() ) {
            continue;
        }

        if ( leftValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalRightMode,
                                   it.key(), leftValue, rightValue );
        } else if ( rightValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalLeftMode,
                                   it.key(), leftValue, rightValue );
        } else if ( leftValue != rightValue ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::ConflictMode,
                                   it.key(), leftValue, rightValue );
        }
    }
}

QString ContactsHandler::adjustedTime( const QString dateTime ) const
{
    QVariant var = QVariant( dateTime );
    QDateTime dt = var.toDateTime();
    QDateTime system =  QDateTime::currentDateTime();
    QDateTime utctime( system );
    utctime.setTimeSpec( Qt::OffsetFromUTC );
    return dt.addSecs( system.secsTo( utctime ) ).toString("yyyy-MM-dd hh:mm");
}
