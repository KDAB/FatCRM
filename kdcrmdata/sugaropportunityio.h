#ifndef SUGAROPPORTUNITYIO_H
#define SUGAROPPORTUNITYIO_H

#include "kdcrmdata_export.h"
#include <QXmlStreamReader>

class SugarOpportunity;
class QIODevice;

class KDCRMDATA_EXPORT SugarOpportunityIO
{
public:
    SugarOpportunityIO();
    bool readSugarOpportunity( QIODevice *device, SugarOpportunity &opportunity );
    bool writeSugarOpportunity( const SugarOpportunity &opportunity, QIODevice *device );
    QString errorString() const;

private:
    QXmlStreamReader xml;
    void readOpportunity(SugarOpportunity &opportunity);

};

#endif /* SUGAROPPORTUNITYIO_H */

