#include "campaignspage.h"

#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "sugarclient.h"

#include "kdcrmdata/sugarcampaign.h"

#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

using namespace Akonadi;

CampaignsPage::CampaignsPage( QWidget *parent )
    : Page( parent, QString( SugarCampaign::mimeType() ), Campaign )
{
    setupModel();
}

CampaignsPage::~CampaignsPage()
{
}

void CampaignsPage::addItem( const QMap<QString, QString> &data )
{
    SugarCampaign campaign;
    campaign.setData( data );
    Item item;
    item.setMimeType( SugarCampaign::mimeType() );
    item.setPayload<SugarCampaign>( campaign );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob( item, collection() );
    Q_UNUSED( job );

    clientWindow()->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
    updateCampaignCombo( campaign.name(), campaign.id() );
}

void CampaignsPage::modifyItem(Item &item, const QMap<QString, QString> &data)
{
    SugarCampaign campaign;
    if ( item.hasPayload<SugarCampaign>() ) {
        campaign = item.payload<SugarCampaign>();
    } else
        return;

    campaign.setData( data );

    item.setPayload<SugarCampaign>( campaign );
    item.setRemoteRevision( data.value( "remoteRevision" ) );
    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemModifyJob *job = new ItemModifyJob( item );
    if ( !job->exec() )
        return; //qDebug() << "Error:" << job->errorString();

    clientWindow()->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
    updateCampaignCombo( campaign.name(), campaign.id() );
}

void CampaignsPage::setupModel()
{
    ItemsTreeModel *campaignsModel = new ItemsTreeModel( recorder(), this );

    ItemsTreeModel::Columns columns;
    columns << ItemsTreeModel::Campaign
            << ItemsTreeModel::Status
            << ItemsTreeModel::Type
            << ItemsTreeModel::EndDate
            << ItemsTreeModel::User;
    campaignsModel->setColumns( columns );

    // same as for the ContactsTreeModel, not strictly necessary
    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel( this );
    filterModel->setSourceModel( campaignsModel );
    filterModel->addMimeTypeInclusionFilter( SugarCampaign::mimeType() );
    filterModel->setHeaderGroup( EntityTreeModel::ItemListHeaders );

    FilterProxyModel *filter = new FilterProxyModel( this );
    filter->setSourceModel( filterModel );
    setFilter( filter );
    Page::setupModel();
}
