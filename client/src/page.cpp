#include "page.h"
#include "enums.h"

#include "sugarclient.h"
//#include "accountsfilterproxymodel.h"

#include <akonadi/contact/contactstreemodel.h>
#include <akonadi/contact/contactsfilterproxymodel.h>
#include <akonadi/agentmanager.h>
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

Page::Page( QWidget *parent, QString mimeType,  DetailsType type )
    : QWidget( parent ),
      mMimeType( mimeType ),
      mType( type ),
      mChangeRecorder( new ChangeRecorder( this ) )
{
    mUi.setupUi( this );
    initialize();
}

Page::~Page()
{
}

void Page::slotResourceSelectionChanged( const QByteArray &identifier )
{
    if ( mCollection.isValid() ) {
        mChangeRecorder->setCollectionMonitored( mCollection, false );
    }

    /*
     * Look for the wanted collection explicitly by listing all collections
     * of the currently selected resource, filtering by MIME type.
     * include statistics to get the number of items in each collection
     */
    CollectionFetchJob *job = new CollectionFetchJob( Collection::root(), CollectionFetchJob::Recursive );
    job->fetchScope().setResource( identifier );
    job->fetchScope().setContentMimeTypes( QStringList() << mMimeType.toLatin1() );
    job->fetchScope().setIncludeStatistics( true );
    connect( job, SIGNAL( result( KJob* ) ),
             this, SLOT( slotCollectionFetchResult( KJob* ) ) );
}

void Page::slotCollectionFetchResult( KJob *job )
{

    CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob*>( job );

    // look for the collection
    Q_FOREACH( const Collection &collection, fetchJob->collections() ) {
        if ( collection.remoteId() == typeToString( mType ).toLatin1() ) {
            mCollection = collection;
            break;
        }
    }

    if ( mCollection.isValid() ) {
        mUi.newPB->setEnabled( true );
        mChangeRecorder->setCollectionMonitored( mCollection, true );

        // if empty, the collection might not have been loaded yet, try synchronizing
        if ( mCollection.statistics().count() == 0 ) {
            AgentManager::self()->synchronizeCollection( mCollection );
        }

        setupCachePolicy();
    } else {
        mUi.newPB->setEnabled( false );
    }
}

void Page::slotItemClicked( const QModelIndex &index )
{
    AccountDetails *d = dynamic_cast<AccountDetails*>( mClientWindow->detailsWidget(mType) );

    if ( d->isEditing() ) {
        if ( !proceedIsOk() ) {
            mUi.treeView->setCurrentIndex( mCurrentIndex );
            return;
        }
    }
    Item item = mUi.treeView->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
    itemChanged(item );
}

void Page::itemChanged( const Item &item )
{
    if ( item.isValid() ) {
        AccountDetails *d = dynamic_cast<AccountDetails*>(mClientWindow->detailsWidget(mType));
        d->setItem( item );

        mCurrentIndex  = mUi.treeView->selectionModel()->currentIndex();
        connect( d, SIGNAL( modifyItem() ),
                 this, SLOT( slotModifyItem( ) ) );
    }
}

void Page::slotNewClicked()
{
    AccountDetails *d = dynamic_cast<AccountDetails*>(mClientWindow->detailsWidget(mType));

    if ( d->isEditing() ) {
        if ( !proceedIsOk() )
            return;
    }
    mClientWindow->displayDockWidgets();

    d->clearFields();
    connect( d, SIGNAL( saveItem() ),
             this, SLOT( slotAddItem( ) ) );
    // reset
    d->initialize();
}

void Page::slotAddItem()
{
    AccountDetails *d = dynamic_cast<AccountDetails*>(mClientWindow->detailsWidget(mType));
    disconnect( d, SIGNAL( saveItem() ),
                this, SLOT( slotAddItem( ) ) );
    addItem(d->accountData() );
}

void Page::slotModifyItem()
{
    const QModelIndex index = mUi.treeView->selectionModel()->currentIndex();
    if ( !index.isValid() )
        return;
    Item item = mUi.treeView->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
    if ( item.isValid() ) {
        AccountDetails *d = dynamic_cast<AccountDetails*>(mClientWindow->detailsWidget(mType));
        disconnect( d, SIGNAL( modifyItem() ),
                this, SLOT( slotModifyItem( ) ) );
        modifyItem( item, d->accountData()  );
        d->reset();
    }
}

void Page::slotRemoveItem()
{
    const QModelIndex index = mUi.treeView->selectionModel()->currentIndex();
    if ( !index.isValid() )
        return;

    Item item = mUi.treeView->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();

    QMessageBox msgBox;
    msgBox.setWindowTitle( tr( "SugarClient - Delete record" ) );
    msgBox.setText( QString( "The selected item will be removed permanentely!" ) );
    msgBox.setInformativeText( tr( "Are you sure you want to delete it?" ) );
    msgBox.setStandardButtons( QMessageBox::Yes |
                               QMessageBox::Cancel );
    msgBox.setDefaultButton( QMessageBox::Cancel );
    int ret = msgBox.exec();
    if ( ret == QMessageBox::Cancel )
        return;

    if ( item.isValid() ) {
        // job starts automatically
        // TODO connect to result() signal for error handling
        ItemDeleteJob *job = new ItemDeleteJob( item );
        Q_UNUSED( job );
    }
    const QModelIndex newIndex = mUi.treeView->selectionModel()->currentIndex();
    if ( !newIndex.isValid() )
        mUi.removePB->setEnabled( false );

        mClientWindow->displayDockWidgets( false );
    if ( typeToString( mType ) == "Accounts" )
        removeAccountsData( item );
}

void Page::slotSetCurrent( const QModelIndex& index, int start, int end )
{
    if ( start == end ) {
        QModelIndex newIdx = mUi.treeView->model()->index(start, 0, index);
        mUi.treeView->setCurrentIndex( newIdx );
    }

      if ( mUi.treeView->model()->rowCount() == mCollection.statistics().count() ) {
          qDebug() << "called ....";
        if ( mType == Account )
            addAccountsData();
      }
}

void Page::initialize()
{
    mClientWindow = dynamic_cast<SugarClient*>( window() );
    mUi.treeView->header()->setResizeMode( QHeaderView::ResizeToContents );

    connect( mUi.clearSearchPB, SIGNAL( clicked() ),
             this, SLOT( slotResetSearch() ) );
    connect( mUi.newPB, SIGNAL( clicked() ),
             this, SLOT( slotNewClicked() ) );
    connect( mUi.removePB, SIGNAL( clicked() ),
             this, SLOT( slotRemoveItem() ) );

    // automatically get the full data when items change
    mChangeRecorder->itemFetchScope().fetchFullPayload( true );

    connect( mUi.treeView, SIGNAL( clicked( const QModelIndex& ) ), this, SLOT( slotItemClicked( const QModelIndex& ) ) );
}

void Page::setupModel()
{
    mUi.treeView->setModel( mFilter );

    connect( mUi.searchLE, SIGNAL( textChanged( const QString& ) ),
             mFilter, SLOT( setFilterString( const QString& ) ) );

    connect( mUi.treeView->model(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), this, SLOT( slotSetCurrent( const QModelIndex&,int,int ) ) );

    connect( mUi.treeView->model(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotUpdateDetails( const QModelIndex&, const QModelIndex& ) ) );
}

void Page::syncronize()
{
    if ( mUi.treeView->model() != 0
         && mUi.treeView->model()->rowCount() > 0 )
        AgentManager::self()->synchronizeCollection( mCollection );

}

void Page::cachePolicyJobCompleted( KJob* job)
{
    if ( job->error() )
        emit statusMessage( tr("Error when setting cachepolicy: %1").arg( job->errorString() ) );
    else
        emit statusMessage( tr("Cache policy set") );

}

void Page::setupCachePolicy()
{
    CachePolicy policy;
    policy.setIntervalCheckTime( 1 ); // Check for new data every minute
    policy.setInheritFromParent( false );
    mCollection.setCachePolicy( policy );
    CollectionModifyJob *job = new CollectionModifyJob( mCollection );
    connect( job, SIGNAL( result( KJob* ) ), this, SLOT( cachePolicyJobCompleted( KJob* ) ) );
}

void Page::slotUpdateDetails( const QModelIndex& topLeft, const QModelIndex& bottomRight )
{
    Q_UNUSED( bottomRight );
    Item item;
    item = mUi.treeView->model()->data( topLeft, EntityTreeModel::ItemRole ).value<Item>();
    itemChanged( item );
}

bool Page::proceedIsOk()
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

void Page::slotSetItem()
{
    mCurrentIndex  = mUi.treeView->selectionModel()->currentIndex();
    if ( mCurrentIndex.isValid() )
        slotItemClicked( mCurrentIndex );
    else
        slotNewClicked();
}

void Page::slotResetSearch()
{
    mUi.searchLE->clear();
}

QString Page::typeToString( const DetailsType &type ) const
{
    if ( type == Account )
        return QString( "Accounts" );
    else if ( type == Opportunity )
        return QString( "Opportunities" );
    else if ( type == Lead )
        return QString( "Leads" );
    else if ( type == Contact )
        return QString( "Contacts" );
    else if ( type == Campaign )
        return QString( "Campaigns" );
    else
        return QString();
}

void Page::updateAccountCombo( const QString& name, const QString& id )
{
    AccountDetails *ad = dynamic_cast<AccountDetails*>( mClientWindow->detailsWidget( Account ) );
    ContactDetails *cd = dynamic_cast<ContactDetails*>( mClientWindow->detailsWidget( Contact ) );
    OpportunityDetails *od =dynamic_cast<OpportunityDetails*>( mClientWindow->detailsWidget( Opportunity ) );
    ad->addAccountData( name, id );
    cd->addAccountData( name, id );
    od->addAccountData( name, id );
}

void Page::addAccountsData()
{

    AccountDetails *ad = dynamic_cast<AccountDetails*>( mClientWindow->detailsWidget( Account ) );
    ContactDetails *cd = dynamic_cast<ContactDetails*>( mClientWindow->detailsWidget( Contact ) );
    OpportunityDetails *od =dynamic_cast<OpportunityDetails*>( mClientWindow->detailsWidget( Opportunity ) );
    QModelIndex index;
    Item item;
    SugarAccount account;
    for ( int i = 0; i <  mUi.treeView->model()->rowCount(); ++i ) {
       index  =  mUi.treeView->model()->index( i, 0 );
       item = mUi.treeView->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
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

void Page::removeAccountsData( Akonadi::Item &item )
{
    AccountDetails *ad = dynamic_cast<AccountDetails*>( mClientWindow->detailsWidget( Account ) );
    ContactDetails *cd = dynamic_cast<ContactDetails*>( mClientWindow->detailsWidget( Contact ) );
    OpportunityDetails *od =dynamic_cast<OpportunityDetails*>( mClientWindow->detailsWidget( Opportunity ) );

    if ( item.hasPayload<SugarAccount>() ) {
        SugarAccount account;
        account = item.payload<SugarAccount>();
        ad->removeAccountData( account.name() );
        cd->removeAccountData( account.name() );
        od->removeAccountData( account.name() );
    }
}
