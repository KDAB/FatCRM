#include "page.h"

#include "sugarclient.h"
#include "enums.h"

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

Page::Page( QWidget *parent, QString mimeType,  QString type )
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
        if ( collection.remoteId() == mType.toLatin1() ) {
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
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w ) {
        AccountDetails *d = dynamic_cast<AccountDetails*>( w->detailsWidget(mDetailsType) );

        if ( d->isEditing() ) {
            if ( !proceedIsOk() ) {
                mUi.treeView->setCurrentIndex( mCurrentIndex );
                return;
            }
        }
        Item item = mUi.treeView->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
        itemChanged(item );
    }
}

void Page::itemChanged( const Item &item )
{
    if ( item.isValid() ) {
        SugarClient *w = dynamic_cast<SugarClient*>( window() );
        AccountDetails *d = dynamic_cast<AccountDetails*>(w->detailsWidget(mDetailsType));
        d->setItem( item );

        mCurrentIndex  = mUi.treeView->selectionModel()->currentIndex();
        connect( d, SIGNAL( modifyItem() ),
                 this, SLOT( slotModifyItem( ) ) );
    }
}

void Page::slotNewClicked()
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w ) {
        AccountDetails *d = dynamic_cast<AccountDetails*>(w->detailsWidget( mDetailsType ) );

        if ( d->isEditing() ) {
            if ( !proceedIsOk() )
                return;
        }
        w->displayDockWidgets();

        d->clearFields();
        connect( d, SIGNAL( saveItem() ),
                 this, SLOT( slotAddItem( ) ) );
        // reset
        d->initialize();
    }
}

void Page::slotAddItem()
{
    addItem();
}

void Page::slotModifyItem()
{
    modifyItem();
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

    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( w )
        w->displayDockWidgets( false );
}

void Page::slotSetCurrent( const QModelIndex& index, int start, int end )
{
    if ( start == end ) {
        QModelIndex newIdx = mUi.treeView->model()->index(start, 0, index);
        mUi.treeView->setCurrentIndex( newIdx );
    }

      if ( treeView()->model()->rowCount() == collection().statistics().count() )
        addAccountsData();
}

void Page::initialize()
{
    mDetailsType = typeToDetailsType( mType );
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
    connect( mUi.treeView->model(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), this, SLOT( slotSetCurrent( const QModelIndex&,int,int ) ) );

    if ( mUi.treeView->model()->rowCount() == mCollection.statistics().count() )
        addAccountsData();

    connect( mUi.treeView->model(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotUpdateDetails( const QModelIndex&, const QModelIndex& ) ) );

    mUi.treeView->reset();
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

DetailsType Page::typeToDetailsType( const QString &type ) const
{
    if ( type == "Accounts" )
        return Account;
    else if ( type == "Opportunities" )
        return Opportunity;
    else if ( type == "Leads" )
        return Lead;
    else if ( type == "Contacts" )
        return Contact;
    else
        return Campaign;
}

