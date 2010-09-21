#include "accountshandler.h"

#include "sugarsoap.h"

#include <akonadi/abstractdifferencesreporter.h>
#include <akonadi/collection.h>

#include <kabc/address.h>

#include <kdcrmdata/sugaraccount.h>

#include <KLocale>

#include <QHash>

typedef QString (*valueGetter)( const SugarAccount& );
typedef void (*valueSetter)( const QString&, SugarAccount&);

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

static QString getTickerSymbol( const SugarAccount &account )
{
    return account.tyckerSymbol();
}

static void setTickerSymbol(const QString &value, SugarAccount &account)
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
    AccessorPair( valueGetter get, valueSetter set, const QString &name )
        : getter( get ), setter( set ), diffName( name )
    {}

public:
    valueGetter getter;
    valueSetter setter;
    const QString diffName;
};

AccountsHandler::AccountsHandler( SugarSession *session )
    : ModuleHandler( QLatin1String( "Accounts" ), session ),
      mAccessors( new AccessorHash )
{
    mAccessors->insert( QLatin1String( "id" ),
                        new AccessorPair( 0, setId, QString() ) );
    mAccessors->insert( QLatin1String( "name" ),
                        new AccessorPair( getName, setName,
                                          i18nc( "@item:intable account name", "Name" ) ) );
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
    mAccessors->insert( QLatin1String( "account_type" ),
                        new AccessorPair( getAccountType, setAccountType,
                                          i18nc( "@item:intable", "Type" ) ) );
    mAccessors->insert( QLatin1String( "industry" ),
                        new AccessorPair( getIndustry, setIndustry,
                                          i18nc( "@item:intable", "Industry" ) ) );
    mAccessors->insert( QLatin1String( "annual_revenue" ),
                        new AccessorPair( getAnnualRevenue, setAnnualRevenue,
                                          i18nc( "@item:intable", "Annual Revenue" ) ) );
    mAccessors->insert( QLatin1String( "phone_fax" ),
                        new AccessorPair( getPhoneFax, setPhoneFax,
                                          i18nc( "@item:intable", "Fax" ) ) );
    mAccessors->insert( QLatin1String( "billing_address_street" ),
                        new AccessorPair( getBillingAddressStreet, setBillingAddressStreet, QString() ) );
    mAccessors->insert( QLatin1String( "billing_address_city" ),
                        new AccessorPair( getBillingAddressCity, setBillingAddressCity, QString() ) );
    mAccessors->insert( QLatin1String( "billing_address_state" ),
                        new AccessorPair( getBillingAddressState, setBillingAddressState, QString() ) );
    mAccessors->insert( QLatin1String( "billing_address_postalcode" ),
                        new AccessorPair( getBillingAddressPostalcode, setBillingAddressPostalcode, QString() ) );
    mAccessors->insert( QLatin1String( "billing_address_country" ),
                        new AccessorPair( getBillingAddressCountry, setBillingAddressCountry, QString() ) );
    mAccessors->insert( QLatin1String( "rating" ),
                        new AccessorPair( getRating, setRating,
                                          i18nc( "@item:intable", "Rating" ) ) );
    mAccessors->insert( QLatin1String( "phone_office" ),
                        new AccessorPair( getPhoneOffice, setPhoneOffice,
                                          i18nc( "@item:intable", "Phone (Office)" ) ) );
    mAccessors->insert( QLatin1String( "phone_alternate" ),
                        new AccessorPair( getPhoneAlternate, setPhoneAlternate,
                                          i18nc( "@item:intable", "Phone (Other)" ) ) );
    mAccessors->insert( QLatin1String( "website" ),
                        new AccessorPair( getWebsite, setWebsite,
                                          i18nc( "@item:intable", "Website" ) ) );
    mAccessors->insert( QLatin1String( "ownership" ),
                        new AccessorPair( getOwnership, setOwnership,
                                          i18nc( "@item:intable", "Ownership" ) ) );
    mAccessors->insert( QLatin1String( "employees" ),
                        new AccessorPair( getEmployees, setEmployees,
                                          i18nc( "@item:intable", "Employees" ) ) );
    mAccessors->insert( QLatin1String( "ticker_symbol" ),
                        new AccessorPair( getTickerSymbol, setTickerSymbol,
                                          i18nc( "@item:intable", "Ticker Symbol" ) ) );
    mAccessors->insert( QLatin1String( "shipping_address_street" ),
                        new AccessorPair( getShippingAddressStreet, setShippingAddressStreet, QString() ) );
    mAccessors->insert( QLatin1String( "shipping_address_city" ),
                        new AccessorPair( getShippingAddressCity, setShippingAddressCity, QString() ) );
    mAccessors->insert( QLatin1String( "shipping_address_state" ),
                        new AccessorPair( getShippingAddressState, setShippingAddressState, QString() ) );
    mAccessors->insert( QLatin1String( "shipping_address_postalcode" ),
                        new AccessorPair( getShippingAddressPostalcode, setShippingAddressPostalcode, QString() ) );
    mAccessors->insert( QLatin1String( "shipping_address_country" ),
                        new AccessorPair( getShippingAddressCountry, setShippingAddressCountry, QString() ) );
    mAccessors->insert( QLatin1String( "email1" ),
                        new AccessorPair( getEmail1, setEmail1,
                                          i18nc( "@item:intable", "Primary Email" ) ) );
    mAccessors->insert( QLatin1String( "parent_id" ),
                        new AccessorPair( getParentId, setParentId, QString() ) );
    mAccessors->insert( QLatin1String( "parent_name" ),
                        new AccessorPair( getParentName, setParentName,
                                          i18nc( "@item:intable", "Member Of" ) ) );
    mAccessors->insert( QLatin1String( "sic_code" ),
                        new AccessorPair( getSicCode, setSicCode,
                                          i18nc( "@item:intable", "SIC Code" ) ) );
    mAccessors->insert( QLatin1String( "campaign_id" ),
                        new AccessorPair( getCampaignId, setCampaignId, QString() ) );
    mAccessors->insert( QLatin1String( "campaign_name" ),
                        new AccessorPair( getCampaignName, setCampaignName,
                                          i18nc( "@item:intable", "Campaign" ) ) );
}

AccountsHandler::~AccountsHandler()
{
    qDeleteAll( *mAccessors );
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

void AccountsHandler::listEntries( const ListEntriesScope &scope )
{
    const QString query = scope.query( QLatin1String( "accounts" ) );
    const QString orderBy = QLatin1String( "accounts.name" );
    const int offset = scope.offset();
    const int maxResults = 100;
    const int fetchDeleted = scope.deleted();

    TNS__Select_fields selectedFields;
    selectedFields.setItems( mAccessors->keys() );

    soap()->asyncGet_entry_list( sessionId(), moduleName(), query, orderBy, offset, selectedFields, maxResults, fetchDeleted );
}

bool AccountsHandler::setEntry( const Akonadi::Item &item )
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
        // check if this is a read-only field
        if ( (*it)->getter == 0 ) {
            continue;
        }
        TNS__Name_value field;
        field.setName( it.key() );
        field.setValue( (*it)->getter( account ) );

        itemList << field;
    }

    TNS__Name_value_list valueList;
    valueList.setItems( itemList );
    soap()->asyncSet_entry( sessionId(), moduleName(), valueList );

    return true;
}

Akonadi::Item AccountsHandler::itemFromEntry( const TNS__Entry_value &entry, const Akonadi::Collection &parentCollection )
{
    Akonadi::Item item;

    const QList<TNS__Name_value> valueList = entry.name_value_list().items();
    if ( valueList.isEmpty() ) {
        kWarning() << "Accounts entry for id=" << entry.id() << "has no values";
        return item;
    }

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

        (*accessIt)->setter( namedValue.value(), account );
    }
    item.setPayload<SugarAccount>( account );
    item.setRemoteRevision( getDateModified( account ) );

    return item;
}

void AccountsHandler::compare( Akonadi::AbstractDifferencesReporter *reporter,
                               const Akonadi::Item &leftItem, const Akonadi::Item &rightItem )
{
    Q_ASSERT( leftItem.hasPayload<SugarAccount>() );
    Q_ASSERT( rightItem.hasPayload<SugarAccount>() );

    const SugarAccount leftAccount = leftItem.payload<SugarAccount>();
    const SugarAccount rightAccount = rightItem.payload<SugarAccount>();

    const QString modifiedBy = getModifiedByName( rightAccount );
    const QString modifiedOn = formatDate( getDateModified( rightAccount ) );

    reporter->setLeftPropertyValueTitle( i18nc( "@title:column", "Local Account" ) );
    reporter->setRightPropertyValueTitle(
        i18nc( "@title:column", "Serverside Account: modified by %1 on %2",
               modifiedBy, modifiedOn ) );

    bool seenBillingAddress = false;
    bool seenShippingAddress = false;

    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( (*it)->getter == 0 ) {
            continue;
        }

        QString leftValue = (*it)->getter( leftAccount );
        QString rightValue = (*it)->getter( rightAccount );

        QString diffName = (*it)->diffName;
        if ( diffName.isEmpty() ) {
            // check for special fields
            if ( it.key().startsWith( QLatin1String( "billing" ) ) ) {
                if ( !seenBillingAddress ) {
                    seenBillingAddress = true;
                    diffName = i18nc( "@item:intable", "Billing Address" );

                    KABC::Address leftAddress;
                    leftAddress.setStreet( getBillingAddressStreet( leftAccount ) );
                    leftAddress.setLocality( getBillingAddressCity( leftAccount ) );
                    leftAddress.setRegion( getBillingAddressState( leftAccount ) );
                    leftAddress.setCountry( getBillingAddressCountry( leftAccount ) );
                    leftAddress.setPostalCode( getBillingAddressPostalcode( leftAccount ) );

                    KABC::Address rightAddress;
                    rightAddress.setStreet( getBillingAddressStreet( rightAccount ) );
                    rightAddress.setLocality( getBillingAddressCity( rightAccount ) );
                    rightAddress.setRegion( getBillingAddressState( rightAccount ) );
                    rightAddress.setCountry( getBillingAddressCountry( rightAccount ) );
                    rightAddress.setPostalCode( getBillingAddressPostalcode( rightAccount ) );

                    leftValue = leftAddress.formattedAddress();
                    rightValue = rightAddress.formattedAddress();
                } else {
                    // already printed, skip
                    continue;
                }
            } else if ( it.key().startsWith( QLatin1String( "shipping" ) ) ) {
                if ( !seenShippingAddress ) {
                    seenShippingAddress = true;
                    diffName = i18nc( "@item:intable", "Shipping Address" );

                    KABC::Address leftAddress( KABC::Address::Parcel );
                    leftAddress.setStreet( getShippingAddressStreet( leftAccount ) );
                    leftAddress.setLocality( getShippingAddressCity( leftAccount ) );
                    leftAddress.setRegion( getShippingAddressState( leftAccount ) );
                    leftAddress.setCountry( getShippingAddressCountry( leftAccount ) );
                    leftAddress.setPostalCode( getShippingAddressPostalcode( leftAccount ) );

                    KABC::Address rightAddress( KABC::Address::Parcel );
                    rightAddress.setStreet( getShippingAddressStreet( rightAccount ) );
                    rightAddress.setLocality( getShippingAddressCity( rightAccount ) );
                    rightAddress.setRegion( getShippingAddressState( rightAccount ) );
                    rightAddress.setCountry( getShippingAddressCountry( rightAccount ) );
                    rightAddress.setPostalCode( getShippingAddressPostalcode( rightAccount ) );

                    leftValue = leftAddress.formattedAddress();
                    rightValue = rightAddress.formattedAddress();
                } else {
                    // already printed, skip
                    continue;
                }
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
