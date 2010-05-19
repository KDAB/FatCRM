#ifndef SUGARACCOUNTIO_H
#define SUGARACCOUNTIO_H

#include "kdcrmdata_export.h"

class SugarAccount;
class QIODevice;

class KDCRMDATA_EXPORT SugarAccountIO
{
public:
    static bool readSugarAccount( QIODevice *device, SugarAccount &account );
    static bool writeSugarAccount( const SugarAccount &account, QIODevice *device );
};

#endif /* SUGARACCOUNTIO_H */

