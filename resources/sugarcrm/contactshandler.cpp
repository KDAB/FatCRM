#include "contactshandler.h"

#include "sugarsoap.h"

#include <akonadi/collection.h>

#include <kabc/addressee.h>
#include <kabc/address.h>

#include <KLocale>

#include <QHash>

typedef QString (*valueGetter)( const KABC::Addressee& );
typedef void (*valueSetter)( const QString&, KABC::Addressee& );

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

static QString getAddress( const QString &type, const KABC::Addressee &addressee )
{
    // Pending(michel)
    // we need to decide which types
    // we will support here
    // see address.cpp for a list of types
    if ( type == "Work" )
        return addressee.address( KABC::Address::Work ).toString();
    else
        return addressee.address( KABC::Address::Pref ).toString();
}

static void setAddress( const KABC::Address &address, KABC::Addressee &addressee )
{
    addressee.insertAddress( address );
}

class AccessorPair
{
public:
    AccessorPair( valueGetter get, valueSetter set ) : getter( get ), setter( set ) {}

public:
    valueGetter getter;
    valueSetter setter;
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
    mAccessors->insert( QLatin1String( "phone_home" ), AccessorPair( getHomePhone, setHomePhone ) );
    mAccessors->insert( QLatin1String( "phone_work" ), AccessorPair( getWorkPhone, setWorkPhone ) );
    mAccessors->insert( QLatin1String( "phone_mobile" ), AccessorPair( getMobilePhone, setMobilePhone ) );
    // Pending (michel)
    // we need to find a way
    // for the address
    //mAccessors->insert( QLatin1String( "address" ), AccessorPair( getAddress, setAddress ) );
}

ContactsHandler::~ContactsHandler()
{
    delete mAccessors;
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
        TNS__Name_value field;
        field.setName( it.key() );
        field.setValue( it->getter( addressee ) );

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

        Q_FOREACH( const TNS__Name_value &namedValue, valueList ) {
            const AccessorHash::const_iterator accessIt = mAccessors->constFind( namedValue.name() );
            if ( accessIt == mAccessors->constEnd() ) {
                // no accessor for field
                continue;
            }

            accessIt->setter( namedValue.value(), addressee );
        }

        item.setPayload<KABC::Addressee>( addressee );
        items << item;
    }

    return items;
}
