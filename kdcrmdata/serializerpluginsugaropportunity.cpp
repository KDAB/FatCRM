#include "serializerpluginsugaropportunity.h"

#include "sugaropportunity.h"
#include "sugaropportunityio.h"

#include <akonadi/item.h>

#include <QtCore/qplugin.h>

using namespace Akonadi;

bool SerializerPluginSugarOpportunity::deserialize( Item& item, const QByteArray& label, QIODevice& data, int version )
{
    Q_UNUSED( version );

    if ( label != Item::FullPayload )
        return false;

    SugarOpportunity sugarOpportunity;
    SugarOpportunityIO io;
    if ( !io.readSugarOpportunity( &data, sugarOpportunity ) )
        return false;

    item.setPayload<SugarOpportunity>( sugarOpportunity );

    return true;
}

void SerializerPluginSugarOpportunity::serialize( const Item& item, const QByteArray& label, QIODevice& data, int &version )
{
    Q_UNUSED( version );

    if ( label != Item::FullPayload || !item.hasPayload<SugarOpportunity>() )
        return;

    const SugarOpportunity sugarOpportunity = item.payload<SugarOpportunity>();
    SugarOpportunityIO io;
    io.writeSugarOpportunity( sugarOpportunity, &data );
}

Q_EXPORT_PLUGIN2( akonadi_serializer_sugaropportunity, Akonadi::SerializerPluginSugarOpportunity )

#include "serializerpluginsugaropportunity.moc"

