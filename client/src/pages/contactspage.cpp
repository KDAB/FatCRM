#include "contactspage.h"
#include "contactstreemodel.h"
#include "filterproxymodel.h"
#include "sugarclient.h"

#include <kabc/addressee.h>
#include <kabc/address.h>

#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

using namespace Akonadi;

ContactsPage::ContactsPage( QWidget *parent )
    : Page( parent, QString( KABC::Addressee::mimeType() ), Contact )
{
    setupModel();
}

ContactsPage::~ContactsPage()
{
}

void ContactsPage::addItem( const QMap<QString, QString> &data)
{
    KABC::Addressee addressee;
    addressee.setGivenName( data.value( "firstName" ) );
    addressee.setFamilyName( data.value( "lastName" ) );
    addressee.setTitle( data.value( "title" ) );
    addressee.setDepartment( data.value( "department" ) );
    addressee.setOrganization( data.value( "accountName" ) );
    addressee.insertCustom( "FATCRM", "X-AccountId", data.value( "accountId" ) );
    addressee.insertEmail( data.value( "primaryEmail" ), true );
    addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "homePhone" ) , KABC::PhoneNumber::Home ) );
    addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "mobilePhone" ) , KABC::PhoneNumber::Cell ) );
    addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "officePhone" ) , KABC::PhoneNumber::Work ) );
    addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "otherPhone" ) , KABC::PhoneNumber::Car ) );
    addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "fax" ) , KABC::PhoneNumber::Work|KABC::PhoneNumber::Fax ) );

    KABC::Address primaryAddress;
    primaryAddress.setType( KABC::Address::Work|KABC::Address::Pref );
    primaryAddress.setStreet( data.value( "primaryAddress" ) );
    primaryAddress.setLocality( data.value( "city" ) );
    primaryAddress.setRegion( data.value( "state" ) );
    primaryAddress.setPostalCode( data.value( "postalCode" ) );
    primaryAddress.setCountry( data.value( "country" ) );
    addressee.insertAddress( primaryAddress );

    KABC::Address otherAddress;
    otherAddress.setType( KABC::Address::Home );
    otherAddress.setStreet( data.value( "otherAddress" ) );
    otherAddress.setLocality( data.value( "otherCity" ) );
    otherAddress.setRegion( data.value( "otherState" ) );
    otherAddress.setPostalCode( data.value( "otherPostalCode" ) );
    otherAddress.setCountry( data.value( "otherCountry" ) );
    addressee.insertAddress( otherAddress );

    addressee.setBirthday( QDateTime::fromString( data.value( "birthDate" ), QString( "yyyy-MM-dd" ) ) );

    addressee.setNote( data.value( "description" ) );
    addressee.insertCustom( "KADDRESSBOOK", "X-AssistantsName", data.value( "assistant" ) );
    addressee.insertCustom( "FATCRM", "X-AssistantsPhone", data.value( "assistantPhone" ) );
    addressee.insertCustom( "FATCRM", "X-LeadSourceName",data.value( "leadSource" ) );
    addressee.insertCustom( "FATCRM", "X-CampaignName",data.value( "campaign" ) );
    addressee.insertCustom( "FATCRM", "X-CampaignId", data.value( "campaignId" ) );
    addressee.insertCustom( "FATCRM", "X-CacceptStatusFields", data.value( "cAcceptStatusFields" ) );
    addressee.insertCustom( "FATCRM", "X-MacceptStatusFields", data.value( "mAcceptStatusFields" ) );
    addressee.insertCustom( "FATCRM", "X-AssignedUserName",data.value( "assignedTo" ) );
    addressee.insertCustom( "FATCRM", "X-AssignedUserId", data.value( "assignedToId" ) );
    addressee.insertCustom( "FATCRM", "X-ReportsToUserName",data.value( "reportsTo" ) );
    addressee.insertCustom( "FATCRM", "X-ReportsToUserId",data.value( "reportsToId" ) );
    addressee.insertCustom( "FATCRM", "X-OpportunityRoleFields", data.value( "opportunityRoleFields" ) );
    addressee.insertCustom( "FATCRM", "X-ModifiedByName", data.value( "modifiedBy" ) );
    addressee.insertCustom( "FATCRM", "X-DateModified", data.value( "modifiedDate" ) );
    addressee.insertCustom( "FATCRM", "X-ModifiedUserId", data.value( "modifiedUserId" ) );
    addressee.insertCustom( "FATCRM", "X-ModifiedUserName", data.value( "modifiedUserName" ) );
    addressee.insertCustom( "FATCRM", "X-DateCreated", data.value( "createdDate" ) );
    addressee.insertCustom( "FATCRM", "X-CreatedByName", data.value( "createdBy" ) );
    addressee.insertCustom( "FATCRM", "X-CreatedById", data.value( "createdById" ) );
    addressee.insertCustom( "FATCRM", "X-ContactId", data.value( "contactId" ) );
    addressee.insertCustom( "FATCRM", "X-Salutation", data.value( "salutation" ) );
    addressee.insertCustom( "FATCRM", "X-Deleted", data.value( "deleted" ) );
    addressee.insertCustom( "FATCRM", "X-DoNotCall", data.value( "doNotCall" ) );

    Item item;
    item.setMimeType( KABC::Addressee::mimeType() );
    item.setPayload<KABC::Addressee>( addressee );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob( item, collection() );
    Q_UNUSED( job );

    clientWindow()->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
    updateReportToCombo( addressee.custom( "FATCRM", "X-ReportsToUserName" ),
                         addressee.custom( "FATCRM", "X-ReportsToUserId" ) );
}

void ContactsPage::modifyItem(Item &item, const QMap<QString, QString> &data)
{
    if ( item.isValid() ) {
        KABC::Addressee addressee;
        if ( item.hasPayload<KABC::Addressee>() ) {
            addressee = item.payload<KABC::Addressee>();
        } else
            return;
        addressee.setGivenName( data.value( "firstName" ) );
        addressee.setFamilyName( data.value( "lastName" ) );
        addressee.setTitle( data.value( "title" ) );
        addressee.setDepartment( data.value( "department" ) );
        addressee.setOrganization( data.value( "accountName" ) );
        // remove before replacing
        // primary email
        addressee.removeEmail( addressee.preferredEmail() );
        addressee.insertEmail(data.value( "primaryEmail" ), true );
        // home phone
        addressee.removePhoneNumber( addressee.phoneNumber(KABC::PhoneNumber::Home ) );
        addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "homePhone" ), KABC::PhoneNumber::Home ) );
        // cell phone
        addressee.removePhoneNumber( addressee.phoneNumber(KABC::PhoneNumber::Cell ) );
        addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "mobilePhone" ), KABC::PhoneNumber::Cell ) );

        // work phone
        addressee.removePhoneNumber( addressee.phoneNumber( KABC::PhoneNumber::Work ) );
        addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "officePhone" ), KABC::PhoneNumber::Work ) );

        // other phone - Pending( michel ) - investigate.
        addressee.removePhoneNumber( addressee.phoneNumber( KABC::PhoneNumber::Car ) );
        addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "otherPhone" ), KABC::PhoneNumber::Car ) );

        // fax
        addressee.removePhoneNumber( addressee.phoneNumber( KABC::PhoneNumber::Work|KABC::PhoneNumber::Fax ) );
        addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "fax" ), KABC::PhoneNumber::Work|KABC::PhoneNumber::Fax ) );


        KABC::Address primaryAddress;
        primaryAddress.setType( KABC::Address::Work|KABC::Address::Pref );
        primaryAddress.setStreet( data.value( "primaryAddress" ) );
        primaryAddress.setLocality( data.value( "city" ) );
        primaryAddress.setRegion( data.value( "state" ) );
        primaryAddress.setPostalCode( data.value( "postalCode" ) );
        primaryAddress.setCountry( data.value( "country" ) );

        if ( !addressee.addresses( KABC::Address::Work|KABC::Address::Pref ).isEmpty())
            addressee.removeAddress( addressee.addresses( KABC::Address::Work|KABC::Address::Pref ).first() );
        addressee.insertAddress( primaryAddress );

        KABC::Address otherAddress;
        otherAddress.setType( KABC::Address::Home);
        otherAddress.setStreet( data.value( "otherAddress" ) );
        otherAddress.setLocality( data.value( "otherCity" ) );
        otherAddress.setRegion( data.value( "otherState" ) );
        otherAddress.setPostalCode( data.value( "otherPostalCode" ) );
        otherAddress.setCountry( data.value( "otherCountry" ) );

        if ( !addressee.addresses( KABC::Address::Home ).isEmpty())
            addressee.removeAddress( addressee.addresses( KABC::Address::Home ).first() );
        addressee.insertAddress( otherAddress );

        addressee.setBirthday( QDateTime::fromString( data.value( "birthDate" ), QString( "yyyy-MM-dd" ) ) );

        addressee.setNote( data.value( "description" ) );
        addressee.removeCustom( "KADDRESSBOOK", "X-AssistantsName" );
        addressee.insertCustom( "KADDRESSBOOK", "X-AssistantsName", data.value( "assistant" ) );
        addressee.removeCustom( "FATCRM", "X-AssistantsPhone" );
        addressee.insertCustom( "FATCRM", "X-AssistantsPhone", data.value( "assistantPhone" ) );
        addressee.removeCustom( "FATCRM", "X-LeadSourceName" );
        addressee.insertCustom( "FATCRM", "X-LeadSourceName", data.value( "leadSource" ) );
        addressee.removeCustom( "FATCRM", "X-CampaignName" );
        addressee.insertCustom( "FATCRM", "X-CampaignName", data.value( "campaign" ) );
        addressee.removeCustom( "FATCRM", "X-CampaignId" );
        addressee.insertCustom( "FATCRM", "X-CampaignId", data.value( "campaignId" ) );
        addressee.removeCustom( "FATCRM", "X-AccountId" );
        addressee.insertCustom( "FATCRM", "X-AccountId", data.value( "accountId" ) );
        addressee.removeCustom( "FATCRM", "X-OpportunityRoleFields" );
        addressee.insertCustom( "FATCRM", "X-OpportunityRoleFields", data.value( "opportunityRoleFields" ) );
        addressee.removeCustom( "FATCRM", "X-CacceptStatusFields" );
        addressee.insertCustom( "FATCRM", "X-CacceptStatusFields", data.value( "cAcceptStatusFields" ) );
        addressee.removeCustom( "FATCRM", "X-MacceptStatusFields");
        addressee.insertCustom( "FATCRM", "X-MacceptStatusFields", data.value( "mAcceptStatusFields" ) );
        addressee.removeCustom( "FATCRM", "X-AssignedUserName" );
        addressee.insertCustom( "FATCRM", "X-AssignedUserName", data.value( "assignedTo" ) );
        addressee.removeCustom( "FATCRM", "X-AssignedUserId" );
        addressee.insertCustom( "FATCRM", "X-AssignedUserId", data.value( "assignedToId" ) );
        addressee.removeCustom( "FATCRM", "X-ReportsToUserName" );
        addressee.insertCustom( "FATCRM", "X-ReportsToUserName", data.value( "reportsTo" ) );
        addressee.removeCustom( "FATCRM", "X-ReportsToUserId" );
        addressee.insertCustom( "FATCRM", "X-ReportsToUserId", data.value( "reportsToId" ) );
        addressee.removeCustom( "FATCRM", "X-ModifiedByName" );
        addressee.insertCustom( "FATCRM", "X-ModifiedByName", data.value( "modifiedBy" ) );
        addressee.removeCustom( "FATCRM", "X-DateModified" );
        addressee.insertCustom( "FATCRM", "X-DateModified", data.value( "modifiedDate" ) );
        addressee.removeCustom( "FATCRM", "X-ModifiedUserId" );
        addressee.insertCustom( "FATCRM", "X-ModifiedUserId", data.value( "modifiedUserId" ) );
        addressee.removeCustom( "FATCRM", "X-ModifiedUserName" );
        addressee.insertCustom( "FATCRM", "X-ModifiedUserName", data.value( "modifiedUserName" ) );
        addressee.removeCustom( "FATCRM", "X-Salutation" );
        addressee.insertCustom( "FATCRM", "X-Salutation", data.value( "salutation" ) );
        addressee.removeCustom( "FATCRM", "X-Deleted" );
        addressee.insertCustom( "FATCRM", "X-Deleted", data.value( "deleted" ) );
        addressee.removeCustom( "FATCRM", "X-DoNotCall" );
        addressee.insertCustom( "FATCRM", "X-DoNotCall", data.value( "doNotCall" ) );

        item.setPayload<KABC::Addressee>( addressee );
        item.setRemoteRevision( data.value( "remoteRevision" ) );

        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemModifyJob *job = new ItemModifyJob( item );
        if ( !job->exec() )
            return; //qDebug() << "Error:" << job->errorString();

        clientWindow()->setEnabled( false );
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
        emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
        updateReportToCombo( addressee.custom( "FATCRM", "X-ReportsToUserName" ),
                             addressee.custom( "FATCRM", "X-ReportsToUserId" ) );
    }
}

void ContactsPage::setupModel()
{

    ContactsTreeModel *contactsModel = new ContactsTreeModel( recorder(), this );

    ContactsTreeModel::Columns columns;
    columns << ContactsTreeModel::FullName
            << ContactsTreeModel::Role
            << ContactsTreeModel::Organization
            << ContactsTreeModel::PreferredEmail
            << ContactsTreeModel::PhoneNumber
            << ContactsTreeModel::GivenName;
    contactsModel->setColumns( columns );

    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel( this );
    filterModel->setSourceModel( contactsModel );
    filterModel->addMimeTypeInclusionFilter( KABC::Addressee::mimeType() );
    filterModel->setHeaderGroup( EntityTreeModel::ItemListHeaders );
    FilterProxyModel *filter = new FilterProxyModel( this );
    filter->setSourceModel( filterModel );
    setFilter( filter );
    Page::setupModel();
}
