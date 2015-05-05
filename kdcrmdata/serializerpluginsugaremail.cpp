#include "serializerpluginsugaremail.h"

#include "sugaremail.h"
#include "sugaremailio.h"

#include <akonadi/item.h>

#include <QtCore/qplugin.h>

using namespace Akonadi;

bool SerializerPluginSugarEmail::deserialize(Item &item, const QByteArray &label, QIODevice &data, int version)
{
    Q_UNUSED(version);

    if (label != Item::FullPayload) {
        return false;
    }

    SugarEmail sugarEmail;
    SugarEmailIO io;
    if (!io.readSugarEmail(&data, sugarEmail)) {
        return false;
    }

    item.setPayload<SugarEmail>(sugarEmail);

    return true;
}

void SerializerPluginSugarEmail::serialize(const Item &item, const QByteArray &label, QIODevice &data, int &version)
{
    Q_UNUSED(version);

    if (label != Item::FullPayload || !item.hasPayload<SugarEmail>()) {
        return;
    }

    const SugarEmail sugarEmail = item.payload<SugarEmail>();
    SugarEmailIO io;
    io.writeSugarEmail(sugarEmail, &data);
}

Q_EXPORT_PLUGIN2(akonadi_serializer_sugaremail, Akonadi::SerializerPluginSugarEmail)

#include "serializerpluginsugaremail.moc"

