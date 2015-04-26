#include "serializerpluginsugarnote.h"

#include "sugarnote.h"
#include "sugarnoteio.h"

#include <akonadi/item.h>

#include <QtCore/qplugin.h>

using namespace Akonadi;

bool SerializerPluginSugarNote::deserialize(Item &item, const QByteArray &label, QIODevice &data, int version)
{
    Q_UNUSED(version);

    if (label != Item::FullPayload) {
        return false;
    }

    SugarNote sugarNote;
    SugarNoteIO io;
    if (!io.readSugarNote(&data, sugarNote)) {
        return false;
    }

    item.setPayload<SugarNote>(sugarNote);

    return true;
}

void SerializerPluginSugarNote::serialize(const Item &item, const QByteArray &label, QIODevice &data, int &version)
{
    Q_UNUSED(version);

    if (label != Item::FullPayload || !item.hasPayload<SugarNote>()) {
        return;
    }

    const SugarNote sugarNote = item.payload<SugarNote>();
    SugarNoteIO io;
    io.writeSugarNote(sugarNote, &data);
}

Q_EXPORT_PLUGIN2(akonadi_serializer_sugarnote, Akonadi::SerializerPluginSugarNote)

#include "serializerpluginsugarnote.moc"

