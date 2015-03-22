#include "serializerpluginsugarlead.h"

#include "sugarlead.h"
#include "sugarleadio.h"

#include <akonadi/item.h>

#include <QtCore/qplugin.h>

using namespace Akonadi;

bool SerializerPluginSugarLead::deserialize(Item &item, const QByteArray &label, QIODevice &data, int version)
{
    Q_UNUSED(version);

    if (label != Item::FullPayload) {
        return false;
    }

    SugarLead sugarLead;
    SugarLeadIO io;
    if (!io.readSugarLead(&data, sugarLead)) {
        return false;
    }

    item.setPayload<SugarLead>(sugarLead);

    return true;
}

void SerializerPluginSugarLead::serialize(const Item &item, const QByteArray &label, QIODevice &data, int &version)
{
    Q_UNUSED(version);

    if (label != Item::FullPayload || !item.hasPayload<SugarLead>()) {
        return;
    }

    const SugarLead sugarLead = item.payload<SugarLead>();
    SugarLeadIO io;
    io.writeSugarLead(sugarLead, &data);
}

Q_EXPORT_PLUGIN2(akonadi_serializer_sugarlead, Akonadi::SerializerPluginSugarLead)

#include "serializerpluginsugarlead.moc"
