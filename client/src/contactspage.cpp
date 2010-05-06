#include "contactspage.h"

#include "sugarclient.h"

#include <akonadi/contact/contactstreemodel.h>
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
            w->contactDetailsWidget()->setItem( item );
            mUi.modifyContactPB->setEnabled( true );

        }
        emit contactItemChanged();
    }
}

void ContactsPage::slotNewContactClicked()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w ) {
        ContactDetails *cd = w->contactDetailsWidget();
        cd->clearFields();
        cd->enableFields();
        connect( cd, SIGNAL( saveContact() ),
                 this, SLOT( slotAddContact( ) ) );
    }
    emit contactItemChanged();
}

void ContactsPage::slotModifyContactClicked()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w ) {
        ContactDetails *cd = w->contactDetailsWidget();
        cd->enableFields();
        cd->setModifyFlag();
        connect( cd, SIGNAL( modifyContact() ),
                 this, SLOT( slotModifyContact() ) );
    }
}

void ContactsPage::slotAddContact()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    QMap<QString, QString> data;
    data = w->contactDetailsWidget()->contactData();
    KABC::Addressee addressee;
    addressee.setGivenName( data.value( "firstName" ) );
    addressee.setFamilyName( data.value( "lastName" ) );
    addressee.setTitle( data.value( "title" ) );
    addressee.setDepartment( data.value( "department" ) );
    addressee.setOrganization( data.value( "accountName" ) );
    addressee.insertEmail( data.value( "primaryEmail" ), true );
    addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "homePhone" ) , KABC::PhoneNumber::Home ) );
    addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "mobilePhone" ) , KABC::PhoneNumber::Cell ) );
    addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "officePhone" ) , KABC::PhoneNumber::Work ) );

    // Pending (michel)
    // check for the type we want to use (e.g primary == Work? etc ... )
    KABC::Address primaryAddress;
    primaryAddress.setType( KABC::Address::Work );
    primaryAddress.setStreet( data.value( "primaryAddress" ) );
    primaryAddress.setLocality( data.value( "city" ) );
    primaryAddress.setRegion( data.value( "state" ) );
    primaryAddress.setPostalCode( data.value( "postalCode" ) );
    primaryAddress.setCountry( data.value( "Country" ) );
    addressee.insertAddress( primaryAddress );

    KABC::Address otherAddress;
    otherAddress.setType( KABC::Address::Pref );
    otherAddress.setStreet( data.value( "otherAddress" ) );
    otherAddress.setLocality( data.value( "otherCity" ) );
    otherAddress.setRegion( data.value( "otherState" ) );
    otherAddress.setPostalCode( data.value( "otherPostalCode" ) );
    otherAddress.setCountry( data.value( "otherCountry" ) );
    addressee.insertAddress( otherAddress );

    Item item;
    item.setMimeType( KABC::Addressee::mimeType() );
    item.setPayload<KABC::Addressee>( addressee );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob( item, mContactsCollection );
    Q_UNUSED( job );

    disconnect( w->contactDetailsWidget(), SIGNAL( saveContact() ),
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
        w->contactDetailsWidget()->disableFields();

        QMap<QString, QString> data;
        data = w->contactDetailsWidget()->contactData();
        addressee.setGivenName( data.value( "firstName" ) );
        addressee.setFamilyName( data.value( "lastName" ) );
        addressee.setTitle( data.value( "title" ) );
        addressee.setDepartment( data.value( "department" ) );
        addressee.setOrganization( data.value( "accountName" ) );
        addressee.insertEmail(data.value( "primaryEmail" ), true );
        addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "homePhone" ), KABC::PhoneNumber::Home ) );
        addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "mobilePhone" ), KABC::PhoneNumber::Cell ) );
        addressee.insertPhoneNumber( KABC::PhoneNumber( data.value( "officePhone" ), KABC::PhoneNumber::Work ) );

        // Pending (michel)
        // check for the type we want to use (e.g primary == Work? etc ... )
        KABC::Address primaryAddress;
        primaryAddress.setType( KABC::Address::Work );
        primaryAddress.setStreet( data.value( "primaryAddress" ) );
        primaryAddress.setLocality( data.value( "city" ) );
        primaryAddress.setRegion( data.value( "state" ) );
        primaryAddress.setPostalCode( data.value( "postalCode" ) );
        primaryAddress.setCountry( data.value( "Country" ) );
        addressee.insertAddress( primaryAddress );

        KABC::Address otherAddress;
        otherAddress.setType( KABC::Address::Pref );
        otherAddress.setStreet( data.value( "otherAddress" ) );
        otherAddress.setLocality( data.value( "otherCity" ) );
        otherAddress.setRegion( data.value( "otherState" ) );
        otherAddress.setPostalCode( data.value( "otherPostalCode" ) );
        otherAddress.setCountry( data.value( "otherCountry" ) );
        addressee.insertAddress( otherAddress );

        item.setPayload<KABC::Addressee>( addressee );

        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemModifyJob *job = new ItemModifyJob( item );
        Q_UNUSED( job );

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
    if ( !index.isValid() ) {
        mUi.modifyContactPB->setEnabled( false );
        mUi.removeContactPB->setEnabled( false );
    }
}

void ContactsPage::slotSetCurrent( const QModelIndex& index, int start, int end )
{
    if ( start == end ) {
        QModelIndex newIdx = mUi.contactsTV->model()->index(start, 0, index);
        mUi.contactsTV->setCurrentIndex( newIdx );

        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        if ( w )
            w->contactDetailsWidget()->disableFields();
    }
}

void ContactsPage::slotSearchItem( const QString& text )
{
    mUi.contactsTV->keyboardSearch( text );
}

void ContactsPage::slotFilterChanged( const QString& filterText )
{
    Q_UNUSED( filterText );
    qDebug() << "Sorry, ContactsPage::slotFilterChanged(), NIY";
}

void ContactsPage::initialize()
{
    QStringList filtersLabels;
    filtersLabels << QString( "All Contacts" )
                  << QString( "Birthdays this month" );
    mUi.filtersCB->addItems( filtersLabels );
    mUi.contactsTV->header()->setResizeMode( QHeaderView::ResizeToContents );

    connect( mUi.newContactPB, SIGNAL( clicked() ),
             this, SLOT( slotNewContactClicked() ) );
    connect( mUi.modifyContactPB, SIGNAL( clicked() ),
             this, SLOT( slotModifyContactClicked() ) );
    connect( mUi.removeContactPB, SIGNAL( clicked() ),
             this, SLOT( slotRemoveContact() ) );
    connect( mUi.filtersCB, SIGNAL( currentIndexChanged( const QString& ) ),
             this,  SLOT( slotFilterChanged( const QString& ) ) );

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

    mUi.contactsTV->setModel( filterModel );

    connect( mUi.searchLE, SIGNAL( textChanged( const QString& ) ),
             this, SLOT( slotSearchItem( const QString& ) ) );
    connect( mUi.contactsTV, SIGNAL( currentChanged( Akonadi::Item ) ), this, SLOT( slotContactChanged( Akonadi::Item ) ) );
    connect( mUi.contactsTV->model(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), SLOT( slotSetCurrent( const QModelIndex&,int,int ) ) );
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

