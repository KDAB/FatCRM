#include "accountspage.h"
#include "accountstreemodel.h"
#include "accountsfilterproxymodel.h"
#include "sugarclient.h"
#include "enums.h"

#include <akonadi/agentmanager.h>
#include <akonadi/changerecorder.h>
#include <akonadi/collection.h>
#include <akonadi/collectionfetchjob.h>
#include <akonadi/collectionfetchscope.h>
#include <akonadi/collectionstatistics.h>
#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemdeletejob.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/itemmodifyjob.h>
#include <akonadi/cachepolicy.h>
#include <akonadi/collectionmodifyjob.h>

#include "kdcrmdata/sugaraccount.h"

#include <QMessageBox>
#include <QItemSelectionModel>
#include <QDebug>


using namespace Akonadi;

AccountsPage::AccountsPage( QWidget *parent )
    : Page( parent, QString( "application/x-vnd.kdab.crm.account" ), QString( "Accounts" ))
{
    setupModel();
}

AccountsPage::~AccountsPage()
{
}

void AccountsPage::addItem()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    AccountDetails *ad = dynamic_cast<AccountDetails*>( w->detailsWidget( Account ) );
    QMap<QString, QString> data;
    data = ad->accountData();
    SugarAccount account;
    account.setName( data.value( "name" ) );
    account.setDateEntered( data.value( "dateEntered" ) );
    account.setDateModified( data.value( "dateModified" ) );
    account.setModifiedUserId( data.value( "modifiedUserId" ) );
    account.setModifiedByName( data.value( "modifiedByName" ) );
    account.setCreatedBy( data.value( "createdBy" ) ); // id
    account.setCreatedByName( data.value( "createdByName" ) );
    account.setDescription( data.value( "description" ) );
    account.setDeleted( data.value( "deleted" ) );
    account.setAssignedUserId( data.value( "assignedUserId" ) );
    account.setAssignedUserName( data.value( "assignedUserName" ) );
    account.setAccountType( data.value( "accountType" ) );
    account.setIndustry( data.value( "industry" ) );
    account.setAnnualRevenue( data.value( "annualRevenue" ) );
    account.setPhoneFax( data.value( "phoneFax" ) );
    account.setBillingAddressStreet( data.value( "billingAddressStreet" ) );
    account.setBillingAddressCity( data.value( "billingAddressCity" ) );
    account.setBillingAddressState( data.value( "billingAddressState" ) );
    account.setBillingAddressPostalcode( data.value( "billingAddressPostalcode" ) );
    account.setBillingAddressCountry( data.value( "billingAddressCountry" ) );
    account.setRating( data.value( "rating" ) );
    account.setPhoneOffice( data.value( "phoneOffice" ) );
    account.setPhoneAlternate( data.value( "phoneAlternate" ) );
    account.setWebsite( data.value( "website" ) );
    account.setOwnership( data.value( "ownership" ) );
    account.setEmployees( data.value( "employees" ) );
    account.setTyckerSymbol( data.value( "tyckerSymbol" ) );
    account.setShippingAddressStreet( data.value( "shippingAddressStreet" ) );
    account.setShippingAddressCity( data.value( "shippingAddressCity" ) );
    account.setShippingAddressState( data.value( "shippingAddressState" ) );
    account.setShippingAddressPostalcode( data.value( "shippingAddressPostalcode" ) );
    account.setShippingAddressCountry( data.value( "shippingAddressCountry" ) );
    account.setEmail1( data.value( "email1" ) );
    account.setParentId( data.value( "parentId" ) );
    account.setParentName( data.value( "parentName" ) );
    account.setSicCode( data.value( "sicCode" ) );
    account.setCampaignId( data.value( "campaignId" ) );
    account.setCampaignName( data.value( "campaignName" ) );

    Item item;
    item.setMimeType( SugarAccount::mimeType() );
    item.setPayload<SugarAccount>( account );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob( item, collection() );
    Q_UNUSED( job );
    w->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
    // update Account combos for:
    // accounts - contacts and opportunity details
    updateAccountCombo( account.name(), account.id());
    disconnect( ad, SIGNAL( saveAccount() ),
                 this, SLOT( slotAddAccount( ) ) );
}

void AccountsPage::modifyItem()
{

    const QModelIndex index = treeView()->selectionModel()->currentIndex();
    Item item = treeView()->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    if ( item.isValid() ) {
        SugarAccount account;
        if ( item.hasPayload<SugarAccount>() ) {
            account = item.payload<SugarAccount>();
        }

        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        AccountDetails *ad = dynamic_cast<AccountDetails*>( w->detailsWidget( Account ) );

        disconnect( ad, SIGNAL( modifyAccount() ),
                this, SLOT( slotModifyAccount( ) ) );
        QMap<QString, QString> data;
        data = ad->accountData();

        account.setName( data.value( "name" ) );
        account.setDateEntered( data.value( "dateEntered" ) );
        account.setDateModified( data.value( "dateModified" ) );
        account.setModifiedUserId( data.value( "modifiedUserId" ) );
        account.setModifiedByName( data.value( "modifiedByName" ) );
        account.setCreatedBy( data.value( "createdBy" ) ); // id
        account.setCreatedByName( data.value( "createdByName" ) );
        account.setDescription( data.value( "description" ) );
        account.setDeleted( data.value( "deleted" ) );
        account.setAssignedUserId( data.value( "assignedUserId" ) );
        account.setAssignedUserName( data.value( "assignedUserName" ) );
        account.setAccountType( data.value( "accountType" ) );
        account.setIndustry( data.value( "industry" ) );
        account.setAnnualRevenue( data.value( "annualRevenue" ) );
        account.setPhoneFax( data.value( "phoneFax" ) );
        account.setBillingAddressStreet( data.value( "billingAddressStreet" ) );
        account.setBillingAddressCity( data.value( "billingAddressCity" ) );
        account.setBillingAddressState( data.value( "billingAddressState" ) );
        account.setBillingAddressPostalcode( data.value( "billingAddressPostalcode" ) );
        account.setBillingAddressCountry( data.value( "billingAddressCountry" ) );
        account.setRating( data.value( "rating" ) );
        account.setPhoneOffice( data.value( "phoneOffice" ) );
        account.setPhoneAlternate( data.value( "phoneAlternate" ) );
        account.setWebsite( data.value( "website" ) );
        account.setOwnership( data.value( "ownership" ) );
        account.setEmployees( data.value( "employees" ) );
        account.setTyckerSymbol( data.value( "tyckerSymbol" ) );
        account.setShippingAddressStreet( data.value( "shippingAddressStreet" ) );
        account.setShippingAddressCity( data.value( "shippingAddressCity" ) );
        account.setShippingAddressState( data.value( "shippingAddressState" ) );
        account.setShippingAddressPostalcode( data.value( "shippingAddressPostalcode" ) );
        account.setShippingAddressCountry( data.value( "shippingAddressCountry" ) );
        account.setEmail1( data.value( "email1" ) );
        account.setParentId( data.value( "parentId" ) );
        account.setParentName( data.value( "parentName" ) );
        account.setSicCode( data.value( "sicCode" ) );
        account.setCampaignId( data.value( "campaignId" ) );
        account.setCampaignName( data.value( "campaignName" ) );

        item.setPayload<SugarAccount>( account );
        item.setRemoteRevision( data.value( "remoteRevision" ) );
        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemModifyJob *job = new ItemModifyJob( item );

        if ( !job->exec() )
            return; //qDebug() << "Error:" << job->errorString();

        w->setEnabled( false );
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
        emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
        // update Account combos for:
        // accounts - contacts and opportunity details
        updateAccountCombo( account.name(), account.id());
        ad->reset();

    }
}


void AccountsPage::removeAccountsData( const Item &item )
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    AccountDetails *ad = dynamic_cast<AccountDetails*>( w->detailsWidget( Account ) );
    ContactDetails *cd = dynamic_cast<ContactDetails*>( w->detailsWidget( Contact ) );
    OpportunityDetails *od =dynamic_cast<OpportunityDetails*>( w->detailsWidget( Opportunity ) );

    if ( item.hasPayload<SugarAccount>() ) {
        SugarAccount account;
        account = item.payload<SugarAccount>();
        ad->removeAccountData( account.name() );
        cd->removeAccountData( account.name() );
        od->removeAccountData( account.name() );
    }

}

void AccountsPage::addAccountsData()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    AccountDetails *ad = dynamic_cast<AccountDetails*>( w->detailsWidget( Account ) );
    ContactDetails *cd = dynamic_cast<ContactDetails*>( w->detailsWidget( Contact ) );
    OpportunityDetails *od =dynamic_cast<OpportunityDetails*>( w->detailsWidget( Opportunity ) );
    QModelIndex index;
    Item item;
    SugarAccount account;
    for ( int i = 0; i <  treeView()->model()->rowCount(); ++i ) {
       index  =  treeView()->model()->index( i, 0 );
       item = treeView()->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
       if ( item.hasPayload<SugarAccount>() ) {
           account = item.payload<SugarAccount>();
           ad->addAccountData( account.name(), account.id() );
           cd->addAccountData( account.name(), account.id() );
           od->addAccountData( account.name(), account.id() );
           // code below should be executed from
           // their own pages when implemented
           ad->addAssignedToData( account.assignedUserName(), account.assignedUserId() );
       }
    }
}

void AccountsPage::updateAccountCombo( const QString& name, const QString& id )
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    AccountDetails *ad = dynamic_cast<AccountDetails*>( w->detailsWidget( Account ) );
    ContactDetails *cd = dynamic_cast<ContactDetails*>( w->detailsWidget( Contact ) );
    OpportunityDetails *od =dynamic_cast<OpportunityDetails*>( w->detailsWidget( Opportunity ) );
    ad->addAccountData( name, id );
    cd->addAccountData( name, id );
    od->addAccountData( name, id );
}

void AccountsPage::setupModel()
{

    AccountsTreeModel *accountsModel = new AccountsTreeModel( recorder(), this );

    AccountsTreeModel::Columns columns;
    columns << AccountsTreeModel::Name
            << AccountsTreeModel::City
            << AccountsTreeModel::Country
            << AccountsTreeModel::Phone
            << AccountsTreeModel::Email
            << AccountsTreeModel::CreatedBy;
    accountsModel->setColumns( columns );

    // same as for the ContactsTreeModel, not strictly necessary
    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel( this );
    filterModel->setSourceModel( accountsModel );
    filterModel->addMimeTypeInclusionFilter( SugarAccount::mimeType() );
    filterModel->setHeaderGroup( EntityTreeModel::ItemListHeaders );

    AccountsFilterProxyModel *filter = new AccountsFilterProxyModel( this );
    filter->setSourceModel( filterModel );
    treeView()->setModel( filter );

    connect( search(), SIGNAL( textChanged( const QString& ) ),
             filter, SLOT( setFilterString( const QString& ) ) );

    connect(treeView()->model(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), SLOT( slotSetCurrent( const QModelIndex&,int,int ) ) );

    connect( treeView()->model(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotUpdateDetails( const QModelIndex&, const QModelIndex& ) ) );
    treeView()->reset();
}
