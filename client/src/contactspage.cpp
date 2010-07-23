#include "contactspage.h"
#include "sugarclient.h"
#include "enums.h"

#include <akonadi/contact/contactstreemodel.h>
#include <akonadi/contact/contactsfilterproxymodel.h>
#include <akonadi/agentmanager.h>
#include <akonadi/changerecorder.h>
#include <akonadi/collection.h>
#include <akonadi/collectionfetchjob.h>
#include <akonadi/collectionfetchscope.h>
#include <akonadi/collectionstatistics.h>
#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemdeletejob.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/itemmodifyjob.h>
#include <akonadi/cachepolicy.h>
#include <akonadi/collectionmodifyjob.h>

#include <kabc/addressee.h>
#include <kabc/address.h>

#include <QMessageBox>

using namespace Akonadi;

ContactsPage::ContactsPage( QWidget *parent )
    : QWidget( parent ),
      mChangeRecorder( new ChangeRecorder( this ) )
{
    mUi.setupUi( this );
    initialize();
}

ContactsPage::~ContactsPage()
{
}

void ContactsPage::slotResourceSelectionChanged( const QByteArray &identifier )
{
    if ( mContactsCollection.isValid() ) {
        mChangeRecorder->setCollectionMonitored( mContactsCollection, false );
    }

    /*
     * Look for the "Contacts" collection explicitly by listing all collections
     * of the currently selected resource, filtering by MIME type.
     * include statistics to get the number of items in each collection
     */
    CollectionFetchJob *job = new CollectionFetchJob( Collection::root(), CollectionFetchJob::Recursive );
    job->fetchScope().setResource( identifier );
    job->fetchScope().setContentMimeTypes( QStringList() << KABC::Addressee::mimeType() );
    job->fetchScope().setIncludeStatistics( true );
    connect( job, SIGNAL( result( KJob* ) ),
             this, SLOT( slotCollectionFetchResult( KJob* ) ) );
}

void ContactsPage::slotCollectionFetchResult( KJob *job )
{

    CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob*>( job );

    // look for the "Contacts" collection
    Q_FOREACH( const Collection &collection, fetchJob->collections() ) {
        if ( collection.remoteId() == QLatin1String( "Contacts" ) ) {
            mContactsCollection = collection;
            break;
        }
    }

    if ( mContactsCollection.isValid() ) {
        mUi.newContactPB->setEnabled( true );
        mChangeRecorder->setCollectionMonitored( mContactsCollection, true );

        // if empty, the collection might not have been loaded yet, try synchronizing
        if ( mContactsCollection.statistics().count() == 0 ) {
            AgentManager::self()->synchronizeCollection( mContactsCollection );
        }

        setupCachePolicy();
    } else {
        mUi.newContactPB->setEnabled( false );
    }
}

void ContactsPage::slotContactChanged( const Item &item )
{
    if ( item.isValid() && item.hasPayload<KABC::Addressee>() ) {
        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        if ( w ) {
            ContactDetails *cd = dynamic_cast<ContactDetails*>(w->detailsWidget( Contact ));
            if ( cd->isEditing() ) {
                if ( !proceedIsOk() ) {
                    disconnect( mUi.contactsTV, SIGNAL( currentChanged( Akonadi::Item ) ), this, SLOT( slotContactChanged( Akonadi::Item ) ) );
                    mUi.contactsTV->selectionModel()->setCurrentIndex( mCurrentIndex,  QItemSelectionModel::NoUpdate );
                    connect( mUi.contactsTV, SIGNAL( currentChanged( Akonadi::Item ) ), this, SLOT( slotContactChanged( Akonadi::Item ) ) );
                    return;
                }
            }
            cd->setItem( item );
            mCurrentIndex  = mUi.contactsTV->selectionModel()->currentIndex();
            connect( cd, SIGNAL( modifyContact() ),
                     this, SLOT( slotModifyContact( ) ) );
        }
    }
}

void ContactsPage::slotNewContactClicked()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w ) {
        w->displayDockWidgets();
        ContactDetails *cd = dynamic_cast<ContactDetails*>(w->detailsWidget( Contact ) );
        cd->clearFields();
        connect( cd, SIGNAL( saveContact() ),
                 this, SLOT( slotAddContact( ) ) );
        // reset
        cd->initialize();
    }
}

void ContactsPage::slotAddContact()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    QMap<QString, QString> data;
    ContactDetails *cd = dynamic_cast<ContactDetails*>( w->detailsWidget( Contact ) );
    data = cd->contactData();
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
    ItemCreateJob *job = new ItemCreateJob( item, mContactsCollection );
    Q_UNUSED( job );
    disconnect( cd, SIGNAL( saveContact() ),
                 this, SLOT( slotAddContact( ) ) );
}

void ContactsPage::slotModifyContact()
{
    const QModelIndex index = mUi.contactsTV->selectionModel()->currentIndex();
    Item item = mUi.contactsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    if ( item.isValid() ) {
        KABC::Addressee addressee;
        if ( item.hasPayload<KABC::Addressee>() ) {
            addressee = item.payload<KABC::Addressee>();
        }

        SugarClient *w = dynamic_cast<SugarClient*>( window() );

        QMap<QString, QString> data;
        ContactDetails *cd = dynamic_cast<ContactDetails*>( w->detailsWidget( Contact ) );
        data = cd->contactData();
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

        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemModifyJob *job = new ItemModifyJob( item );
        Q_UNUSED( job );
        cd->initialize();
    }
}

void ContactsPage::slotRemoveContact()
{
    const QModelIndex index = mUi.contactsTV->selectionModel()->currentIndex();
    Item item = mUi.contactsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    if ( item.isValid() ) {
        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemDeleteJob *job = new ItemDeleteJob( item );
        Q_UNUSED( job );
    }
    const QModelIndex newIndex = mUi.contactsTV->selectionModel()->currentIndex();
    if ( !index.isValid() )
        mUi.removeContactPB->setEnabled( false );
}

void ContactsPage::slotSetCurrent( const QModelIndex& index, int start, int end )
{
    if ( start == end ) {
        QModelIndex newIdx = mUi.contactsTV->model()->index(start, 0, index);
        mUi.contactsTV->setCurrentIndex( newIdx );
    }
     //model items are loaded
     if ( mUi.contactsTV->model()->rowCount() == mContactsCollection.statistics().count() )
         addAccountsData();
}

void ContactsPage::addAccountsData()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    ContactDetails *cd = dynamic_cast<ContactDetails*>( w->detailsWidget( Contact ) );
    QModelIndex index;
    Item item;
    KABC::Addressee addressee;
    for ( int i = 0; i <  mUi.contactsTV->model()->rowCount(); ++i ) {
       index  =  mUi.contactsTV->model()->index( i, 0 );
       item = mUi.contactsTV->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
       if ( item.hasPayload<KABC::Addressee>() ) {
           addressee = item.payload<KABC::Addressee>();
           QString fullName = addressee.givenName() + " " + addressee.familyName();
           cd->addReportsToData( fullName, addressee.custom( "FATCRM", "X-ContactId") );
           cd->addAssignedToData(  addressee.custom( "FATCRM", "X-AssignedUserName" ), addressee.custom( "FATCRM", "X-AssignedUserId" ) );
       }
    }
}

void ContactsPage::initialize()
{

    mUi.contactsTV->header()->setResizeMode( QHeaderView::ResizeToContents );

    connect( mUi.newContactPB, SIGNAL( clicked() ),
             this, SLOT( slotNewContactClicked() ) );
    connect( mUi.removeContactPB, SIGNAL( clicked() ),
             this, SLOT( slotRemoveContact() ) );

    // automatically get the full data when items change
    mChangeRecorder->itemFetchScope().fetchFullPayload( true );

    /*
     * convenience model for contacts, allowing us to easily specify the columns
     * to show
     * could use an Akonadi::ItemModel instead because we don't have a tree of
     * collections but only a single one
     */
    ContactsTreeModel *contactsModel = new ContactsTreeModel( mChangeRecorder, this );

    ContactsTreeModel::Columns columns;
    columns << ContactsTreeModel::FullName
            << ContactsTreeModel::Role
            << ContactsTreeModel::Organization
            << ContactsTreeModel::PreferredEmail
            << ContactsTreeModel::PhoneNumbers
            << ContactsTreeModel::GivenName;
    contactsModel->setColumns( columns );


    // same as for the ContactsTreeModel, not strictly necessary
    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel( this );
    filterModel->setSourceModel( contactsModel );
    filterModel->addMimeTypeInclusionFilter( KABC::Addressee::mimeType() );
    filterModel->setHeaderGroup( EntityTreeModel::ItemListHeaders );

    ContactsFilterProxyModel *filter = new ContactsFilterProxyModel( this );
    filter->setSourceModel( filterModel );
    mUi.contactsTV->setModel( filter );

    connect( mUi.searchLE, SIGNAL( textChanged( const QString& ) ),
             filter, SLOT( setFilterString( const QString& ) ) );

    connect( mUi.contactsTV, SIGNAL( currentChanged( Akonadi::Item ) ), this, SLOT( slotContactChanged( Akonadi::Item ) ) );

    connect( mUi.contactsTV->model(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), SLOT( slotSetCurrent( const QModelIndex&,int,int ) ) );

    connect( mUi.contactsTV->model(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotUpdateItemDetails( const QModelIndex&, const QModelIndex& ) ) );

}

void ContactsPage::syncronize()
{
    AgentManager::self()->synchronizeCollection( mContactsCollection );

}

void ContactsPage::cachePolicyJobCompleted( KJob* job)
{
    if ( job->error() )
        emit statusMessage( tr("Error when setting cachepolicy: %1").arg( job->errorString() ) );
    else
        emit statusMessage( tr("Cache policy set") );

}

void ContactsPage::setupCachePolicy()
{
    CachePolicy policy;
    policy.setIntervalCheckTime( 1 ); // Check for new data every minute
    policy.setInheritFromParent( false );
    mContactsCollection.setCachePolicy( policy );
    CollectionModifyJob *job = new CollectionModifyJob( mContactsCollection );
    connect( job, SIGNAL( result( KJob* ) ), this, SLOT( cachePolicyJobCompleted( KJob* ) ) );
}

void ContactsPage::slotUpdateItemDetails( const QModelIndex& topLeft, const QModelIndex& bottomRight )
{
    Q_UNUSED( bottomRight );
    Item item;
    KABC::Addressee addressee;
    item = mUi.contactsTV->model()->data( topLeft, EntityTreeModel::ItemRole ).value<Item>();
    slotContactChanged( item );
}

bool ContactsPage::proceedIsOk()
{
    bool proceed = true;
    QMessageBox msgBox;
    msgBox.setText( tr( "The current item has been modified." ) );
    msgBox.setInformativeText( tr( "Do you want to save your changes?" ) );
    msgBox.setStandardButtons( QMessageBox::Save |
                               QMessageBox::Discard );
    msgBox.setDefaultButton( QMessageBox::Save );
    int ret = msgBox.exec();
    if ( ret == QMessageBox::Save )
        proceed = false;
    return proceed;
}
