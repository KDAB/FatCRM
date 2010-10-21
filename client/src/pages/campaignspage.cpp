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
    Item item;
    details()->updateItem( item, data );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob( item, collection() );
    Q_UNUSED( job );

    clientWindow()->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
    const SugarCampaign campaign = item.payload<SugarCampaign>();
    updateCampaignCombo( campaign.name(), campaign.id() );
}

void CampaignsPage::modifyItem(Item &item, const QMap<QString, QString> &data)
{
    details()->updateItem( item, data );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemModifyJob *job = new ItemModifyJob( item );
    if ( !job->exec() )
        return; //qDebug() << "Error:" << job->errorString();

    clientWindow()->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
    const SugarCampaign campaign = item.payload<SugarCampaign>();
    updateCampaignCombo( campaign.name(), campaign.id() );
}
