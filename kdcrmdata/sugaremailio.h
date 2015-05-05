#ifndef SUGAREMAILIO_H
#define SUGAREMAILIO_H

#include "kdcrmdata_export.h"
#include <QXmlStreamReader>

class SugarEmail;
class QIODevice;

class KDCRMDATA_EXPORT SugarEmailIO
{
public:
    SugarEmailIO();
    bool readSugarEmail(QIODevice *device, SugarEmail &email);
    bool writeSugarEmail(const SugarEmail &email, QIODevice *device);
    QString errorString() const;

private:
    QXmlStreamReader xml;
    void readEmail(SugarEmail &email);

};
#endif

