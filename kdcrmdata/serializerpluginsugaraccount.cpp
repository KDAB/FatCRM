#include "serializerpluginsugaraccount.h"

#include "sugaraccount.h"
#include "sugaraccountio.h"

#include <akonadi/item.h>

#include <QtCore/qplugin.h>

using namespace Akonadi;

bool SerializerPluginSugarAccount::deserialize( Item& item, const QByteArray& label, QIODevice& data, int version )
{
    Q_UNUSED( version );
    SugarAccount sugarAccount;
    if ( item.hasPayload<SugarAccount>() )
        sugarAccount = item.payload<SugarAccount>();

    if ( !SugarAccountIO::readSugarAccount( &data, sugarAccount ) )
        return false;

    item.setPayload<SugarAccount>( sugarAccount );

    return true;
}

void SerializerPluginSugarAccount::serialize( const Item& item, const QByteArray& label, QIODevice& data, int &version )
{
    Q_UNUSED( version );

    if ( !item.hasPayload<SugarAccount>() )
        return;

    const SugarAccount sugarAccount = item.payload<SugarAccount>();
    SugarAccountIO::writeSugarAccount( sugarAccount, &data );
}

Q_EXPORT_PLUGIN2( akonadi_serializer_sugaraccount, Akonadi::SerializerPluginSugarAccount )

#include "serializerpluginsugaraccount.moc"
