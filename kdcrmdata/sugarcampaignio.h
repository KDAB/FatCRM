#ifndef SUGARCAMPAIGNIO_H
#define SUGARCAMPAIGNIO_H

#include "kdcrmdata_export.h"
#include <QXmlStreamReader>

class SugarCampaign;
class QIODevice;

class KDCRMDATA_EXPORT SugarCampaignIO
{
public:
    SugarCampaignIO();
    bool readSugarCampaign( QIODevice *device, SugarCampaign &campaign );
    bool writeSugarCampaign( const SugarCampaign &campaign, QIODevice *device );
    QString errorString() const;

private:
    QXmlStreamReader xml;
    void readCampaign(SugarCampaign &campaign);

};
#endif /* SUGARCAMPAIGNIO_H */

