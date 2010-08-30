#include "accountspage.h"
#include "accountstreemodel.h"
#include "accountsfilterproxymodel.h"
#include "sugarclient.h"

#include "kdcrmdata/sugaraccount.h"

#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

#include <QDebug>


using namespace Akonadi;

AccountsPage::AccountsPage( QWidget *parent )
    : Page( parent, QString( "application/x-vnd.kdab.crm.account" ), Account )
{
    setupModel();
}

AccountsPage::~AccountsPage()
{
}

void AccountsPage::addItem( const QMap<QString, QString> data )
{
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
    item.setMimeType( mimeType() );
    item.setPayload<SugarAccount>( account );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob( item, collection() );
    Q_UNUSED( job );
    clientWindow()->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
    // update Account combos for:
    // accounts - contacts and opportunity details
    updateAccountCombo( account.name(), account.id());
}

void AccountsPage::modifyItem( Item &item, const QMap<QString, QString> data  )
{
    SugarAccount account;
    if ( item.hasPayload<SugarAccount>() ) {
        account = item.payload<SugarAccount>();
    } else
        return;

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

    clientWindow()->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
    // update Account combos for:
    // accounts - contacts and opportunity details
    updateAccountCombo( account.name(), account.id());
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
    setFilter( filter );
    Page::setupModel();
}
