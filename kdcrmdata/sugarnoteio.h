#ifndef SUGARNOTEIO_H
#define SUGARNOTEIO_H

#include "kdcrmdata_export.h"
#include <QXmlStreamReader>

class SugarNote;
class QIODevice;

class KDCRMDATA_EXPORT SugarNoteIO
{
public:
    SugarNoteIO();
    bool readSugarNote(QIODevice *device, SugarNote &note);
    bool writeSugarNote(const SugarNote &note, QIODevice *device);
    QString errorString() const;

private:
    QXmlStreamReader xml;
    void readNote(SugarNote &note);

};
#endif

