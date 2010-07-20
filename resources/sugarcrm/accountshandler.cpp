#include "accountshandler.h"

#include "sugarsoap.h"

#include <akonadi/collection.h>

#include <kdcrmdata/sugaraccount.h>

#include <KLocale>

#include <QHash>

typedef QString (*valueGetter)( const SugarAccount& );
typedef void (*valueSetter)( const QString&, SugarAccount&);

static QString getId( const SugarAccount &account )
{

    return account.id();
}

static void setId( const QString &value, SugarAccount &account )
{

    account.setId( value );
}

static QString getName( const SugarAccount &account )
{
    return account.name();
}

static void setName( const QString &value, SugarAccount &account )
{
    account.setName( value );
}

static QString getDateEntered( const SugarAccount &account )
{
    return account.dateEntered();
}

static void setDateEntered( const QString &value, SugarAccount &account )
{
    account.setDateEntered( value );
}

static QString getDateModified( const SugarAccount &account )
{
    return account.dateModified();
}

static void setDateModified( const QString &value, SugarAccount &account )
{
    account.setDateModified( value );
}

static QString getModifiedUserId( const SugarAccount &account )
{
    return account.modifiedUserId();
}

static void setModifiedUserId( const QString &value, SugarAccount &account )
{
    account.setModifiedUserId( value );
}

static QString getModifiedByName( const SugarAccount &account )
{
    return account.modifiedByName();
}

static void setModifiedByName( const QString &value, SugarAccount &account )
{
    account.setModifiedByName( value );
}

static QString getCreatedBy( const SugarAccount &account )
{
    return account.createdBy();
}

static void setCreatedBy( const QString &value, SugarAccount &account )
{
    account.setCreatedBy( value );
}

static QString getCreatedByName( const SugarAccount &account )
{
    return account.createdByName();
}

static void setCreatedByName( const QString &value, SugarAccount &account )
{
    account.setCreatedByName( value );
}

static QString getDescription( const SugarAccount &account )
{
    return account.description();
}

static void setDescription( const QString &value, SugarAccount &account )
{
    account.setDescription( value );
}

static QString getDeleted( const SugarAccount &account )
{
    return account.deleted();
}

static void setDeleted (const QString &value, SugarAccount &account )
{
    account.setDeleted( value );
}

static QString getAssignedUserId( const SugarAccount &account )
{
    return account.assignedUserId();
}

static void setAssignedUserId(const QString &value, SugarAccount &account)
{
    account.setAssignedUserId( value );
}

static QString getAssignedUserName( const SugarAccount &account )
{
    return account.assignedUserName();
}

static void setAssignedUserName(const QString &value, SugarAccount &account)
{
    account.setAssignedUserName( value );
}

static QString getAccountType( const SugarAccount &account )
{
    return account.accountType();
}

static void setAccountType(const QString &value, SugarAccount &account)
{
    account.setAccountType( value );
}

static QString getIndustry( const SugarAccount &account )
{
    return account.industry();
}

static void setIndustry(const QString &value, SugarAccount &account)
{
    account.setIndustry( value );
}

static QString getAnnualRevenue( const SugarAccount &account )
{
    return account.annualRevenue();
}

static void setAnnualRevenue(const QString &value, SugarAccount &account)
{
    account.setAnnualRevenue( value );
}

static QString getPhoneFax( const SugarAccount &account )
{
    return account.phoneFax();
}

static void setPhoneFax(const QString &value, SugarAccount &account)
{
    account.setPhoneFax( value );
}

static QString getBillingAddressStreet( const SugarAccount &account )
{
    return account.billingAddressStreet();
}

static void setBillingAddressStreet(const QString &value, SugarAccount &account)
{
    account.setBillingAddressStreet( value );
}

static QString getBillingAddressCity( const SugarAccount &account )
{
    return account.billingAddressCity();
}

static void setBillingAddressCity(const QString &value, SugarAccount &account)
{
    account.setBillingAddressCity( value );
}

static QString getBillingAddressState( const SugarAccount &account )
{
    return account.billingAddressState();
}

static void setBillingAddressState(const QString &value, SugarAccount &account)
{
    account.setBillingAddressState( value );
}

static QString getBillingAddressPostalcode( const SugarAccount &account )
{
    return account.billingAddressPostalcode();
}

static void setBillingAddressPostalcode(const QString &value, SugarAccount &account)
{
    account.setBillingAddressPostalcode( value );
}

static QString getBillingAddressCountry( const SugarAccount &account )
{
    return account.billingAddressCountry();
}

static void setBillingAddressCountry(const QString &value, SugarAccount &account)
{
    account.setBillingAddressCountry( value );
}

static QString getRating( const SugarAccount &account )
{
    return account.rating();
}

static void setRating(const QString &value, SugarAccount &account)
{
    account.setRating( value );
}

static QString getPhoneOffice( const SugarAccount &account )
{
    return account.phoneOffice();
}

static void setPhoneOffice(const QString &value, SugarAccount &account)
{
    account.setPhoneOffice( value );
}

static QString getPhoneAlternate( const SugarAccount &account )
{
    return account.phoneAlternate();
}

static void setPhoneAlternate(const QString &value, SugarAccount &account)
{
    account.setPhoneAlternate( value );
}

static QString getWebsite( const SugarAccount &account )
{
    return account.website();
}

static void setWebsite(const QString &value, SugarAccount &account)
{
    account.setWebsite( value );
}

static QString getOwnership( const SugarAccount &account )
{
    return account.ownership();
}

static void setOwnership(const QString &value, SugarAccount &account)
{
    account.setOwnership( value );
}

static QString getEmployees( const SugarAccount &account )
{
    return account.employees();
}

static void setEmployees(const QString &value, SugarAccount &account)
{
    account.setEmployees( value );
}

static QString getTyckerSymbol( const SugarAccount &account )
{
    return account.tyckerSymbol();
}

static void setTyckerSymbol(const QString &value, SugarAccount &account)
{
    account.setTyckerSymbol( value );
}

static QString getShippingAddressStreet( const SugarAccount &account )
{
    return account.shippingAddressStreet();
}

static void setShippingAddressStreet(const QString &value, SugarAccount &account)
{
    account.setShippingAddressStreet( value );
}

static QString getShippingAddressCity( const SugarAccount &account )
{
    return account.shippingAddressCity();
}

static void setShippingAddressCity(const QString &value, SugarAccount &account)
{
    account.setShippingAddressCity( value );
}

static QString getShippingAddressState( const SugarAccount &account )
{
    return account.shippingAddressState();
}

static void setShippingAddressState(const QString &value, SugarAccount &account)
{
    account.setShippingAddressState( value );
}

static QString getShippingAddressPostalcode( const SugarAccount &account )
{
    return account.shippingAddressPostalcode();
}

static void setShippingAddressPostalcode(const QString &value, SugarAccount &account)
{
    account.setShippingAddressPostalcode( value );
}

static QString getShippingAddressCountry( const SugarAccount &account )
{
    return account.shippingAddressCountry();
}

static void setShippingAddressCountry(const QString &value, SugarAccount &account)
{
    account.setShippingAddressCountry( value );
}

static QString getEmail1( const SugarAccount &account )
{
    return account.email1();
}

static void setEmail1( const QString &value, SugarAccount &account )
{
    account.setEmail1( value );
}

static QString getParentId( const SugarAccount &account )
{
    return account.parentId();
}

static void setParentId( const QString &value, SugarAccount &account )
{
    account.setParentId( value );
}

static QString getParentName( const SugarAccount &account )
{
    return account.parentName();
}

static void setParentName( const QString &value, SugarAccount &account )
{
    account.setParentName( value );
}

static QString getSicCode( const SugarAccount &account )
{
    return account.sicCode();
}

static void setSicCode( const QString &value, SugarAccount &account )
{
    account.setSicCode( value );
}

static QString getCampaignId( const SugarAccount &account )
{
    return account.campaignId();
}

static void setCampaignId( const QString &value, SugarAccount &account )
{
    account.setCampaignId( value );
}

static QString getCampaignName( const SugarAccount &account )
{
    return account.campaignName();
}

static void setCampaignName( const QString &value, SugarAccount &account )
{
    account.setCampaignName( value );
}


class AccessorPair
{
public:
    AccessorPair( valueGetter get, valueSetter set ) : getter( get ), setter( set ){}

public:
    valueGetter getter;
    valueSetter setter;
};



AccountsHandler::AccountsHandler()
    : ModuleHandler( QLatin1String( "Accounts" ) ),
      mAccessors( new AccessorHash )
{
    mAccessors->insert( QLatin1String( "id" ), AccessorPair( getId, setId ) );
    mAccessors->insert( QLatin1String( "name" ), AccessorPair( getName, setName ) );
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
    mAccessors->insert( QLatin1String( "account_type" ), AccessorPair( getAccountType, setAccountType ) );
    mAccessors->insert( QLatin1String( "industry" ), AccessorPair( getIndustry, setIndustry ) );
    mAccessors->insert( QLatin1String( "annual_revenue" ), AccessorPair( getAnnualRevenue, setAnnualRevenue ) );
    mAccessors->insert( QLatin1String( "phone_fax" ), AccessorPair( getPhoneFax, setPhoneFax ) );
    mAccessors->insert( QLatin1String( "billing_address_street" ), AccessorPair( getBillingAddressStreet, setBillingAddressStreet ) );
    mAccessors->insert( QLatin1String( "billing_address_city" ), AccessorPair( getBillingAddressCity, setBillingAddressCity ) );
    mAccessors->insert( QLatin1String( "billing_address_state" ), AccessorPair( getBillingAddressState, setBillingAddressState ) );
    mAccessors->insert( QLatin1String( "billing_address_postalcode" ), AccessorPair( getBillingAddressPostalcode, setBillingAddressPostalcode ) );
    mAccessors->insert( QLatin1String( "billing_address_country" ), AccessorPair( getBillingAddressCountry, setBillingAddressCountry ) );
    mAccessors->insert( QLatin1String( "rating" ), AccessorPair( getRating, setRating ) );
    mAccessors->insert( QLatin1String( "phone_office" ), AccessorPair( getPhoneOffice, setPhoneOffice ) );
    mAccessors->insert( QLatin1String( "phone_alternate" ), AccessorPair( getPhoneAlternate, setPhoneAlternate ) );
    mAccessors->insert( QLatin1String( "website" ), AccessorPair( getWebsite, setWebsite ) );
    mAccessors->insert( QLatin1String( "ownership" ), AccessorPair( getOwnership, setOwnership ) );
    mAccessors->insert( QLatin1String( "employees" ), AccessorPair( getEmployees, setEmployees ) );
    mAccessors->insert( QLatin1String( "ticker_symbol" ), AccessorPair( getTyckerSymbol, setTyckerSymbol ) );
    mAccessors->insert( QLatin1String( "shipping_address_street" ), AccessorPair( getShippingAddressStreet, setShippingAddressStreet ) );
    mAccessors->insert( QLatin1String( "shipping_address_city" ), AccessorPair( getShippingAddressCity, setShippingAddressCity ) );
    mAccessors->insert( QLatin1String( "shipping_address_state" ), AccessorPair( getShippingAddressState, setShippingAddressState ) );
    mAccessors->insert( QLatin1String( "shipping_address_postalcode" ), AccessorPair( getShippingAddressPostalcode, setShippingAddressPostalcode ) );
    mAccessors->insert( QLatin1String( "shipping_address_country" ), AccessorPair( getShippingAddressCountry, setShippingAddressCountry ) );
    mAccessors->insert( QLatin1String( "email1" ), AccessorPair( getEmail1, setEmail1 ) );
    mAccessors->insert( QLatin1String( "parent_id" ), AccessorPair( getParentId, setParentId ) );
    mAccessors->insert( QLatin1String( "parent_name" ), AccessorPair( getParentName, setParentName ) );
    mAccessors->insert( QLatin1String( "sic_code" ), AccessorPair( getSicCode, setSicCode ) );
    mAccessors->insert( QLatin1String( "campaign_id" ), AccessorPair( getCampaignId, setCampaignId ) );
    mAccessors->insert( QLatin1String( "campaign_name" ), AccessorPair( getCampaignName, setCampaignName ) );
}

AccountsHandler::~AccountsHandler()
{
    delete mAccessors;
}

QStringList AccountsHandler::supportedFields() const
{
    return mAccessors->keys();
}

Akonadi::Collection AccountsHandler::collection() const
{
    Akonadi::Collection accountCollection;
    accountCollection.setRemoteId( moduleName() );
    accountCollection.setContentMimeTypes( QStringList() << SugarAccount::mimeType() );
    accountCollection.setName( i18nc( "@item folder name", "Accounts" ) );
    accountCollection.setRights( Akonadi::Collection::CanChangeItem |
                                 Akonadi::Collection::CanCreateItem |
                                 Akonadi::Collection::CanDeleteItem );

    return accountCollection;
}

void AccountsHandler::listEntries( int offset, Sugarsoap* soap, const QString &sessionId )
{
    const QString query = QLatin1String( "" );
    const QString orderBy = QLatin1String( "accounts.name" );
    const int maxResults = 100;
    const int fetchDeleted = 0; // do not fetch deleted items

    TNS__Select_fields selectedFields;
    selectedFields.setItems( mAccessors->keys() );

    soap->asyncGet_entry_list( sessionId, moduleName(), query, orderBy, offset, selectedFields, maxResults, fetchDeleted );
}

bool AccountsHandler::setEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId )
{
    if ( !item.hasPayload<SugarAccount>() ) {
        kError() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Account payload";
        return false;
    }

    QList<TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the account being added
    if ( !item.remoteId().isEmpty() ) {
        TNS__Name_value field;
        field.setName( QLatin1String( "id" ) );
        field.setValue( item.remoteId() );

        itemList << field;
    }

    const SugarAccount account = item.payload<SugarAccount>();
    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        TNS__Name_value field;
        field.setName( it.key() );
        field.setValue( it->getter( account ) );
        itemList << field;
    }

    TNS__Name_value_list valueList;
    valueList.setItems( itemList );
    soap->asyncSet_entry( sessionId, moduleName(), valueList );

    return true;
}

Akonadi::Item::List AccountsHandler::itemsFromListEntriesResponse( const TNS__Entry_list &entryList,
                                                                   const Akonadi::Collection &parentCollection )
{
    Akonadi::Item::List items;

    Q_FOREACH( const TNS__Entry_value &entry, entryList.items() ) {
        const QList<TNS__Name_value> valueList = entry.name_value_list().items();
        if ( valueList.isEmpty() ) {
            kWarning() << "Accounts entry for id=" << entry.id() << "has no values";
            continue;
        }

        Akonadi::Item item;
        item.setRemoteId( entry.id() );
        item.setParentCollection( parentCollection );
        item.setMimeType( SugarAccount::mimeType() );

        SugarAccount account;
        account.setId( entry.id() );
        Q_FOREACH( const TNS__Name_value &namedValue, valueList ) {
            const AccessorHash::const_iterator accessIt = mAccessors->constFind( namedValue.name() );
            if ( accessIt == mAccessors->constEnd() ) {
                // no accessor for field
                continue;
            }
            accessIt->setter( namedValue.value(), account );
        }
        item.setPayload<SugarAccount>( account );
        item.setRemoteRevision( getDateModified( account ) );
        items << item;
    }

    return items;
}

