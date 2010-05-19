#ifndef SERIALIZERPLUGINSUGARACCOUNT_H
#define SERIALIZERPLUGINSUGARACCOUNT_H

#include <QtCore/QObject>
#include <QtCore/QObject>

#include <akonadi/itemserializerplugin.h>

namespace Akonadi {

class SerializerPluginSugarAccount : public QObject, public ItemSerializerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Akonadi::ItemSerializerPlugin )

public:
    bool deserialize( Item& item, const QByteArray& label, QIODevice& data, int version );
    void serialize( const Item& item, const QByteArray& label, QIODevice& data, int &version );
};

}

#endif /* SERIALIZERPLUGINSUGARACCOUNT_H */

