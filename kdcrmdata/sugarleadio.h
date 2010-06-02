#ifndef SUGARLEADIO_H
#define SUGARLEADIO_H

#include "kdcrmdata_export.h"
#include <QXmlStreamReader>

class SugarLead;
class QIODevice;

class KDCRMDATA_EXPORT SugarLeadIO
{
public:
    SugarLeadIO();
    bool readSugarLead( QIODevice *device, SugarLead &lead );
    bool writeSugarLead( const SugarLead &lead, QIODevice *device );
    QString errorString() const;

private:
    QXmlStreamReader xml;
    void readLead(SugarLead &lead);

};

#endif /* SUGARLEADIO_H */

