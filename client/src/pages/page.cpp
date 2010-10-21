#include "page.h"
#include "enums.h"
#include "sugarclient.h"
#include "kdcrmdata/sugaraccount.h"
#include "kdcrmdata/sugaropportunity.h"
#include "kdcrmdata/sugarcampaign.h"
#include "kdcrmdata/sugarlead.h"

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

Page::Page( QWidget *parent, const QString &mimeType, DetailsType type )
    : QWidget( parent ),
      mMimeType( mimeType ),
      mType( type ),
      mDetailsWidget( 0 ),
      mChangeRecorder( new ChangeRecorder( this ) ),
      mShowDetailsAction( 0 )
{
    mUi.setupUi( this );
    mUi.splitter->setCollapsible( 0, false );
    initialize();
}

Page::~Page()
{
}

void Page::setDetailsWidget( DetailsWidget *widget )
{
    QVBoxLayout* detailLayout = new QVBoxLayout( mUi.detailsWidget );
    detailLayout->setMargin( 0 );
    detailLayout->addWidget( widget );
    mDetailsWidget = widget;
    mShowDetailsAction->setChecked( true );
}

QAction *Page::showDetailsAction( const QString &title ) const
{
    mShowDetailsAction->setText( title );
    return mShowDetailsAction;
}

bool Page::showsDetails() const
{
    return mShowDetailsAction->isChecked();
}

void Page::showDetails( bool on )
{
    mUi.detailsWidget->setVisible( on );
    if ( on ) {
        QMetaObject::invokeMethod( this, "slotEnsureDetailsVisible", Qt::QueuedConnection );
    }
    mShowDetailsAction->setChecked( on );
}

void Page::slotResourceSelectionChanged( const QByteArray &identifier )
{
    if ( mCollection.isValid() ) {
        mChangeRecorder->setCollectionMonitored( mCollection, false );
    }

    mCollection = Collection();

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

    mUi.reloadPB->setEnabled( false );
    mUi.reloadSB->setEnabled( false );
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
        mUi.reloadPB->setEnabled( true );
        mUi.reloadSB->setEnabled( true );
        mChangeRecorder->setCollectionMonitored( mCollection, true );

        // if empty, the collection might not have been loaded yet, try synchronizing
        if ( mCollection.statistics().count() == 0 ) {
            AgentManager::self()->synchronizeCollection( mCollection );
        }

        setupCachePolicy();
    } else {
        mUi.newPB->setEnabled( false );
        mUi.reloadPB->setEnabled( false );
        mUi.reloadSB->setEnabled( false );
    }
}

void Page::slotItemClicked( const QModelIndex &index )
{
    if ( mDetailsWidget != 0 ){
        if ( mDetailsWidget->isEditing() ) {
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
        if ( mDetailsWidget != 0 ) {
            mDetailsWidget->setItem( item );
            connect( mDetailsWidget, SIGNAL( modifyItem() ), this, SLOT( slotModifyItem( ) ) );
        }

        mCurrentIndex  = mUi.treeView->selectionModel()->currentIndex();
    }
}

void Page::slotNewClicked()
{
    if ( mDetailsWidget != 0 ) {
        if ( mDetailsWidget->isEditing() ) {
            if ( !proceedIsOk() )
                return;
        }

        mDetailsWidget->clearFields();
        connect( mDetailsWidget, SIGNAL( saveItem() ), this, SLOT( slotAddItem() ) );
    }
}

void Page::slotAddItem()
{
    if ( mDetailsWidget != 0 ) {
        disconnect( mDetailsWidget, SIGNAL( saveItem() ), this, SLOT( slotAddItem() ) );
        addItem( mDetailsWidget->data() );
    }
}

void Page::slotModifyItem()
{
    const QModelIndex index = mUi.treeView->selectionModel()->currentIndex();
    if ( !index.isValid() )
        return;
    Item item = mUi.treeView->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
    if ( item.isValid() && mDetailsWidget != 0 ) {
        disconnect( mDetailsWidget, SIGNAL( modifyItem() ), this,  SLOT( slotModifyItem() ) );
        mDetailsWidget->reset();
        modifyItem( item, mDetailsWidget->data()  );
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

    if ( mDetailsWidget != 0 ) {
        mDetailsWidget->setItem( Item() );
    }

    if ( mType == Account )
        removeAccountsData( item );
    else if ( mType == Campaign )
        removeCampaignsData( item );
}

void Page::slotSetCurrent( const QModelIndex& index, int start, int end )
{
    if ( start == end ) {
        QModelIndex newIdx = mUi.treeView->model()->index(start, 0, index);
        mUi.treeView->setCurrentIndex( newIdx );
    }

    if ( mUi.treeView->model()->rowCount() == mCollection.statistics().count() ) {
        if ( mType == Account )
            addAccountsData();
        else if ( mType == Campaign )
            addCampaignsData();
        else if ( mType == Contact )
            addContactsData();
    }
}

void Page::initialize()
{
    mClientWindow = dynamic_cast<SugarClient*>( window() );
    mUi.treeView->header()->setResizeMode( QHeaderView::ResizeToContents );

    const QIcon icon = ( style() != 0 ? style()->standardIcon( QStyle::SP_BrowserReload, 0, mUi.reloadPB ) : QIcon() );
    if ( !icon.isNull() ) {
        mUi.reloadPB->setIcon( icon );
    }
    mUi.reloadPB->setEnabled( false );
    mUi.reloadSB->setEnabled( false );

    connect( mUi.clearSearchPB, SIGNAL( clicked() ),
             this, SLOT( slotResetSearch() ) );
    connect( mUi.newPB, SIGNAL( clicked() ),
             this, SLOT( slotNewClicked() ) );
    connect( mUi.removePB, SIGNAL( clicked() ),
             this, SLOT( slotRemoveItem() ) );
    connect( mUi.reloadPB, SIGNAL( clicked() ),
             this, SLOT( slotReloadCollection() ) );
    connect( mUi.reloadSB, SIGNAL( editingFinished() ),
             this, SLOT( slotReloadIntervalChanged() ) );

    // automatically get the full data when items change
    mChangeRecorder->itemFetchScope().fetchFullPayload( true );
    mChangeRecorder->setMimeTypeMonitored( mMimeType );

    connect( mChangeRecorder, SIGNAL( collectionChanged( Akonadi::Collection ) ),
             this, SLOT( slotCollectionChanged( Akonadi::Collection ) ) );

    connect( mUi.treeView, SIGNAL( clicked( const QModelIndex& ) ), this, SLOT( slotItemClicked( const QModelIndex& ) ) );
    connect( mUi.treeView, SIGNAL( activated( const QModelIndex& ) ), this, SLOT( slotShowDetails( const QModelIndex& ) ) );

    mShowDetailsAction = new QAction( this );
    mShowDetailsAction->setCheckable( true );
    connect( mShowDetailsAction, SIGNAL( toggled( bool ) ), this, SLOT( showDetails( bool ) ) );
    connect( mShowDetailsAction, SIGNAL( toggled( bool ) ), this, SIGNAL( showDetailsChanged( bool ) ) );
}

void Page::setupModel()
{
    ItemsTreeModel *model = new ItemsTreeModel( mType, recorder(), this );
    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel( this );
    filterModel->setSourceModel( model );
    filterModel->addMimeTypeInclusionFilter( mimeType() );
    filterModel->setHeaderGroup( EntityTreeModel::ItemListHeaders );
    FilterProxyModel *filter = new FilterProxyModel( this );
    filter->setSourceModel( filterModel );
    setFilter( filter );
    mUi.treeView->setModel( filter );

    connect( mUi.searchLE, SIGNAL( textChanged( const QString& ) ),
             mFilter, SLOT( setFilterString( const QString& ) ) );

    connect( mUi.treeView->model(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), this, SLOT( slotSetCurrent( const QModelIndex&,int,int ) ) );

    connect( mUi.treeView->model(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), this, SLOT( slotUpdateDetails( const QModelIndex&, const QModelIndex& ) ) );
    connect( mUi.treeView->selectionModel(), SIGNAL( currentChanged( QModelIndex, QModelIndex ) ),
             this,  SLOT( slotUpdateDetails( const QModelIndex& ) ) );
}

Details *Page::details() const
{
    return mDetailsWidget->mDetails;
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
    CachePolicy policy = mCollection.cachePolicy();
    if ( !policy.inheritFromParent() ) {
        kDebug() << "Collection" << mCollection.name()
                 << "already has a cache policy. Will not overwrite it.";
    } else {
        policy.setInheritFromParent( false );
        policy.setIntervalCheckTime( 1 ); // Check for new data every minute
        mCollection.setCachePolicy( policy );
        CollectionModifyJob *job = new CollectionModifyJob( mCollection );
        connect( job, SIGNAL( result( KJob* ) ), this, SLOT( cachePolicyJobCompleted( KJob* ) ) );
    }

    mUi.reloadSB->setValue( policy.intervalCheckTime() );
    mUi.reloadSB->setEnabled( true );
}

void Page::slotUpdateDetails( const QModelIndex& topLeft, const QModelIndex& bottomRight )
{
    if ( !mClientWindow->isEnabled() ) {
        do {
            QApplication::restoreOverrideCursor();
        } while ( QApplication::overrideCursor() != 0 );
          mClientWindow->setEnabled( true );
    }
    Q_UNUSED( bottomRight );

    Item item;
    item = mUi.treeView->model()->data( topLeft, EntityTreeModel::ItemRole ).value<Item>();
    itemChanged( item );
}

void Page::slotUpdateDetails( const QModelIndex& index )
{
    if ( index.isValid() ) {
        Item item;
        item = mUi.treeView->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
        itemChanged( item );
    }
}

void Page::slotShowDetails( const QModelIndex& index )
{
    slotUpdateDetails( index );
    slotEnsureDetailsVisible();
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
    mCurrentIndex  = treeView()->selectionModel()->currentIndex();
    if ( mCurrentIndex.isValid() )
        slotItemClicked( mCurrentIndex );
    else
        slotNewClicked();
}

void Page::slotResetSearch()
{
    mUi.searchLE->clear();
}

void Page::slotReloadCollection()
{
    if ( mCollection.isValid() ) {
        AgentManager::self()->synchronizeCollection( mCollection );
    }
}

void Page::slotReloadIntervalChanged()
{
    const int value = mUi.reloadSB->value();
    kDebug() << "value=" << value;

    if ( mCollection.isValid() ) {
        CachePolicy policy = mCollection.cachePolicy();
        policy.setInheritFromParent( false );
        policy.setIntervalCheckTime( value == 0 ? -1 : value );
        mCollection.setCachePolicy( policy );
        CollectionModifyJob *job = new CollectionModifyJob( mCollection );
        connect( job, SIGNAL( result( KJob* ) ), this, SLOT( cachePolicyJobCompleted( KJob* ) ) );
    }
}

void Page::slotCollectionChanged( const Akonadi::Collection &collection )
{
    if ( mCollection.isValid() && collection == mCollection ) {
        mCollection = collection;

        const CachePolicy policy = mCollection.cachePolicy();
        if ( policy.inheritFromParent() ) {
            mUi.reloadSB->setValue( 0 );
        } else {
            mUi.reloadSB->setValue( policy.intervalCheckTime() );
        }
    }
}

void Page::slotEnsureDetailsVisible()
{
    if ( mShowDetailsAction->isChecked() ) {
        QList<int> splitterSizes = mUi.splitter->sizes();
        if ( splitterSizes[ 1 ] == 0 ) {
            splitterSizes[ 1 ] = mUi.splitter->height() / 2;
            mUi.splitter->setSizes( splitterSizes );
        }
    } else {
        mShowDetailsAction->setChecked( true );
    }
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
    DetailsWidget *ad = mClientWindow->detailsWidget(Account);
    ad->addAccountData( name, id );
    DetailsWidget *cd = mClientWindow->detailsWidget(Contact);
    cd->addAccountData( name, id );
    DetailsWidget *od = mClientWindow->detailsWidget(Opportunity);
    od->addAccountData( name, id );
}

void Page::updateAssignedToCombo( const QString& name, const QString& id )
{
    DetailsWidget *ad = mClientWindow->detailsWidget(Account);
    ad->addAssignedToData( name, id );
    DetailsWidget *cd = mClientWindow->detailsWidget(Contact);
    cd->addAssignedToData( name, id );
    DetailsWidget *od = mClientWindow->detailsWidget(Opportunity);
    od->addAssignedToData( name, id );
    DetailsWidget *cad = mClientWindow->detailsWidget(Campaign);
    cad->addAssignedToData( name, id );
    DetailsWidget *ld = mClientWindow->detailsWidget(Lead);
    ld->addAssignedToData( name, id );
}

void Page::updateCampaignCombo( const QString& name, const QString& id )
{
    DetailsWidget *ad = mClientWindow->detailsWidget(Account);
    ad->addCampaignData( name, id );
    DetailsWidget *cd = mClientWindow->detailsWidget(Contact);
    cd->addCampaignData( name, id );
    DetailsWidget *od = mClientWindow->detailsWidget(Opportunity);
    od->addCampaignData( name, id );
    DetailsWidget *ld = mClientWindow->detailsWidget(Lead);
    ld->addCampaignData( name, id );
}

void Page::updateReportToCombo( const QString& name, const QString& id )
{
    DetailsWidget *cd = mClientWindow->detailsWidget(Contact);
    cd->addReportsToData( name, id );
}

void Page::addAccountsData()
{
    QModelIndex index;
    Item item;
    SugarAccount account;
    for ( int i = 0; i <  mUi.treeView->model()->rowCount(); ++i ) {
        index  =  mUi.treeView->model()->index( i, 0 );
        item = mUi.treeView->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
        if ( item.hasPayload<SugarAccount>() ) {
            account = item.payload<SugarAccount>();
            updateAccountCombo( account.name(), account.id() );
            updateAssignedToCombo( account.assignedUserName(), account.assignedUserId() );
        }
    }
}

void Page::addCampaignsData()
{
    QModelIndex index;
    Item item;
    SugarCampaign campaign;
    for ( int i = 0; i <  mUi.treeView->model()->rowCount(); ++i ) {
        index  =  mUi.treeView->model()->index( i, 0 );
        item = mUi.treeView->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
        if ( item.hasPayload<SugarCampaign>() ) {
            campaign = item.payload<SugarCampaign>();
            updateCampaignCombo( campaign.name(), campaign.id() );
        }
    }
}

void Page::addContactsData()
{
    QString fullName;
    QModelIndex index;
    Item item;
    KABC::Addressee addressee;
    for ( int i = 0; i <  mUi.treeView->model()->rowCount(); ++i ) {
        index  =  mUi.treeView->model()->index( i, 0 );
        item = mUi.treeView->model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
        if ( item.hasPayload<KABC::Addressee>() ) {
            addressee = item.payload<KABC::Addressee>();
            fullName = addressee.givenName() + " " + addressee.familyName();
            updateReportToCombo( fullName, addressee.custom( "FATCRM", "X-ContactId" ) );
        }
    }
}

void Page::removeAccountsData( Akonadi::Item &item )
{
    DetailsWidget *ad = mClientWindow->detailsWidget(Account);
    DetailsWidget *cd = mClientWindow->detailsWidget(Contact);
    DetailsWidget *od = mClientWindow->detailsWidget(Opportunity);

    if ( item.hasPayload<SugarAccount>() ) {
        SugarAccount account;
        account = item.payload<SugarAccount>();
        ad->removeAccountData( account.name() );
        cd->removeAccountData( account.name() );
        od->removeAccountData( account.name() );
    }
}

void Page::removeCampaignsData( Akonadi::Item &item )
{
    DetailsWidget *ad = mClientWindow->detailsWidget(Account);
    DetailsWidget *cd = mClientWindow->detailsWidget(Contact);
    DetailsWidget *od = mClientWindow->detailsWidget(Opportunity);
    DetailsWidget *ld = mClientWindow->detailsWidget(Lead);
    if ( item.hasPayload<SugarCampaign>() ) {
        SugarCampaign campaign;
        campaign = item.payload<SugarCampaign>();
        ad->removeCampaignData( campaign.name() );
        cd->removeCampaignData( campaign.name() );
        od->removeCampaignData( campaign.name() );
        ld->removeCampaignData( campaign.name() );
    }
}
