#include "leadspage.h"
#include "leadstreemodel.h"
#include "leadsfilterproxymodel.h"
#include "sugarclient.h"
#include "enums.h"

#include "kdcrmdata/sugarlead.h"

#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

#include <QDebug>

using namespace Akonadi;

LeadsPage::LeadsPage( QWidget *parent )
    : Page( parent, QString( SugarLead::mimeType() ), Lead )
{
    setupModel();
}

LeadsPage::~LeadsPage()
{
}


void LeadsPage::addItem( const QMap<QString, QString> data )
{

    SugarLead lead;
    lead.setData( data );

    Item item;
    item.setMimeType( SugarLead::mimeType() );
    item.setPayload<SugarLead>( lead );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemCreateJob *job = new ItemCreateJob( item, collection() );
    Q_UNUSED( job );

    clientWindow()->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
}

void LeadsPage::modifyItem(Item &item, const QMap<QString, QString> data)
{
    SugarLead lead;
    if ( item.hasPayload<SugarLead>() ) {
        lead = item.payload<SugarLead>();
    } else
        return;

    lead.setData( data );
    item.setPayload<SugarLead>( lead );
    item.setRemoteRevision( data.value( "remoteRevision" ) );

    // job starts automatically
    // TODO connect to result() signal for error handling
    ItemModifyJob *job = new ItemModifyJob( item );
    if ( !job->exec() )
        return; //qDebug() << "Error:" << job->errorString();

    clientWindow()->setEnabled( false );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ));
    emit statusMessage( tr( "Be patient the data is being saved remotely!..." ) );
}

void LeadsPage::setupModel()
{
    LeadsTreeModel *leadsModel = new LeadsTreeModel( recorder(), this );

    LeadsTreeModel::Columns columns;
    columns << LeadsTreeModel::Name
            << LeadsTreeModel::Status
            << LeadsTreeModel::AccountName
            << LeadsTreeModel::Email
            << LeadsTreeModel::User;
    leadsModel->setColumns( columns );

    // same as for the ContactsTreeModel, not strictly necessary
    EntityMimeTypeFilterModel *filterModel = new EntityMimeTypeFilterModel( this );
    filterModel->setSourceModel( leadsModel );
    filterModel->addMimeTypeInclusionFilter( SugarLead::mimeType() );
    filterModel->setHeaderGroup( EntityTreeModel::ItemListHeaders );

    LeadsFilterProxyModel *filter = new LeadsFilterProxyModel( this );
    filter->setSourceModel( filterModel );
    setFilter( filter );
    Page::setupModel();
}

