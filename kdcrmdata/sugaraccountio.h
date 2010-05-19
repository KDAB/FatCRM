#ifndef SUGARACCOUNTIO_H
#define SUGARACCOUNTIO_H

class SugarAccount;
class QIODevice;

class SugarAccountIO
{
public:
    static bool readSugarAccount( QIODevice *device, SugarAccount &account );
    static bool writeSugarAccount( const SugarAccount &account, QIODevice *device );
};

#endif /* SUGARACCOUNTIO_H */

