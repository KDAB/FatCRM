#ifndef SUGARACCOUNTIO_H
#define SUGARACCOUNTIO_H

#include "kdcrmdata_export.h"
#include <QXmlStreamReader>

class SugarAccount;
class QIODevice;

class KDCRMDATA_EXPORT SugarAccountIO
{
public:
    SugarAccountIO();
    bool readSugarAccount(QIODevice *device, SugarAccount &account);
    bool writeSugarAccount(const SugarAccount &account, QIODevice *device);
    QString errorString() const;

private:
    QXmlStreamReader xml;
    void readAccount(SugarAccount &account);

};

#endif /* SUGARACCOUNTIO_H */

