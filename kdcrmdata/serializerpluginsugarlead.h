#ifndef SERIALIZERPLUGINSUGARLEAD_H
#define SERIALIZERPLUGINSUGARLEAD_H

#include <QtCore/QObject>

#include <akonadi/itemserializerplugin.h>

namespace Akonadi
{

class SerializerPluginSugarLead : public QObject, public ItemSerializerPlugin
{
    Q_OBJECT
    Q_INTERFACES(Akonadi::ItemSerializerPlugin)

public:
    bool deserialize(Item &item, const QByteArray &label, QIODevice &data, int version);
    void serialize(const Item &item, const QByteArray &label, QIODevice &data, int &version);
};

}

#endif /* SERIALIZERPLUGINSUGARLEAD_H */

