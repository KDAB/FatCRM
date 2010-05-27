#include "contactshandler.h"

#include "salesforcesoap.h"

#include <akonadi/collection.h>

#include <kabc/addressee.h>

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

class AccessorPair
{
public:
    AccessorPair( valueGetter get, valueSetter set ) : getter( get ), setter( set ) {}

public:
    valueGetter getter;
    valueSetter setter;
};

ContactsHandler::ContactsHandler()
    : ModuleHandler( QLatin1String( "Contact" ) ),
      mAccessors( new AccessorHash )
{
    mAccessors->insert( QLatin1String( "FirstName" ), AccessorPair( getFirstName, setFirstName ) );
    mAccessors->insert( QLatin1String( "LastName" ), AccessorPair( getLastName, setLastName ) );

    mAccessors->insert( QLatin1String( "Email" ), AccessorPair( getEmail1, setEmail1 ) );
    mAccessors->insert( QLatin1String( "Title" ), AccessorPair( getTitle, setTitle ) );
    mAccessors->insert( QLatin1String( "Department" ), AccessorPair( getDepartment, setDepartment ) );
    mAccessors->insert( QLatin1String( "HomePhone" ), AccessorPair( getHomePhone, setHomePhone ) );
    mAccessors->insert( QLatin1String( "Phone" ), AccessorPair( getWorkPhone, setWorkPhone ) );
    mAccessors->insert( QLatin1String( "MobilePhone" ), AccessorPair( getMobilePhone, setMobilePhone ) );
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

void ContactsHandler::listEntries( const TNS__QueryLocator &locator, SforceService* soap )
{
    static QString queryString = QLatin1String( "Select Id, " ) +
                                 QStringList( mAccessors->keys() ).join( QLatin1String( ", " ) ) +
                                 QLatin1String( " from Contact" ); // without trailing 's'

    if ( locator.value().isEmpty() ) {
        TNS__Query query;
        query.setQueryString( queryString );
        soap->asyncQuery( query );
    } else {
        TNS__QueryMore query;
        query.setQueryLocator( locator );
        soap->asyncQueryMore( query );
    }
}

bool ContactsHandler::setEntry( const Akonadi::Item &item, SforceService *soap )
{
    if ( !item.hasPayload<KABC::Addressee>() ) {
        kError() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Addressee payload";
        return false;
    }

    ENS__SObject object;
    object.setType( QLatin1String( "Contact" ) );

    // if there is an id add it, otherwise skip this field
    // no id will result in the contact being added
    if ( !item.remoteId().isEmpty() ) {
        object.setId( item.remoteId() );
    }

    QList<QString> valueList;
    const KABC::Addressee addressee = item.payload<KABC::Addressee>();
    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // TODO how do we specify which fields we have?
        // do we need to fill the full object?
        valueList << it->getter( addressee );
    }

    object.setAny( valueList );

    TNS__Upsert upsert;
    upsert.setSObjects( QList<ENS__SObject>() << object );
    soap->asyncUpsert( upsert );

    return true;
}

Akonadi::Item::List ContactsHandler::itemsFromListEntriesResponse( const TNS__QueryResult &queryResult,
                                                                   const Akonadi::Collection &parentCollection )
{
    Akonadi::Item::List items;

    Q_FOREACH( const ENS__SObject &entry, queryResult.records() ) {
        kDebug() << "Record of type=" << entry.type();
        const QList<QString> valueList = entry.any();
        if ( valueList.isEmpty() ) {
            kWarning() << "Contacts entry for id=" << entry.id().value() << "has no values";
            kDebug() << "fieldsToNull:" << entry.fieldsToNull();
            continue;
        }

        Akonadi::Item item;
        item.setRemoteId( entry.id().value() );
        item.setParentCollection( parentCollection );
        item.setMimeType( KABC::Addressee::mimeType() );

        KABC::Addressee addressee;
        addressee.setUid( entry.id().value() );

        // assume for now that the values are in the same order we have queried them
        Q_ASSERT( mAccessors->count() == valueList.count() );

        AccessorHash::const_iterator it    = mAccessors->constBegin();
        AccessorHash::const_iterator endIt = mAccessors->constEnd();
        for ( QList<QString>::const_iterator valueIt ; it != endIt; ++it, ++valueIt ) {
            it->setter( *valueIt, addressee );
        }

        item.setPayload<KABC::Addressee>( addressee );
        items << item;
    }

    return items;
}
