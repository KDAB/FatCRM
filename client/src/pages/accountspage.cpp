#include "accountspage.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "sugarclient.h"

#include "kdcrmdata/sugaraccount.h"

#include <akonadi/entitymimetypefiltermodel.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

#include <QDebug>


using namespace Akonadi;

AccountsPage::AccountsPage( QWidget *parent )
    : Page( parent, QString( SugarAccount::mimeType() ), Account )
{
    setupModel();
}

AccountsPage::~AccountsPage()
{
}

void AccountsPage::addItem( const QMap<QString, QString> &data )
{
    SugarAccount account;
    account.setData( data );

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
    updateAssignedToCombo( account.assignedUserName(), account.assignedUserId() );
}

void AccountsPage::modifyItem( Item &item, const QMap<QString, QString> &data  )
{
    SugarAccount account;
    if ( item.hasPayload<SugarAccount>() ) {
        account = item.payload<SugarAccount>();
    } else
        return;

    account.setData( data );
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
    updateAssignedToCombo( account.assignedUserName(), account.assignedUserId() );
}
