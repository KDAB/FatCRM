#include "serializerpluginsugarcampaign.h"

#include "sugarcampaign.h"
#include "sugarcampaignio.h"

#include <akonadi/item.h>

#include <QtCore/qplugin.h>

using namespace Akonadi;

bool SerializerPluginSugarCampaign::deserialize( Item& item, const QByteArray& label, QIODevice& data, int version )
{
    Q_UNUSED( version );

    if ( label != Item::FullPayload )
        return false;

    SugarCampaign sugarCampaign;
    SugarCampaignIO io;
    if ( !io.readSugarCampaign( &data, sugarCampaign ) )
        return false;

    item.setPayload<SugarCampaign>( sugarCampaign );

    return true;
}

void SerializerPluginSugarCampaign::serialize( const Item& item, const QByteArray& label, QIODevice& data, int &version )
{
    Q_UNUSED( version );

    if ( label != Item::FullPayload || !item.hasPayload<SugarCampaign>() )
        return;

    const SugarCampaign sugarCampaign = item.payload<SugarCampaign>();
    SugarCampaignIO io;
    io.writeSugarCampaign( sugarCampaign, &data );
}

Q_EXPORT_PLUGIN2( akonadi_serializer_sugarcampaign, Akonadi::SerializerPluginSugarCampaign )

#include "serializerpluginsugarcampaign.moc"

