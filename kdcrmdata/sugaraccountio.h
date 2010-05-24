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

private:
    static const QString readContent( const QString &line );
};

#endif /* SUGARACCOUNTIO_H */

