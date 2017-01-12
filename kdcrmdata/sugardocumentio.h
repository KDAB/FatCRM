/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Tobias Koenig <tobias.koenig@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SUGARDOCUMENTIO_H
#define SUGARDOCUMENTIO_H

#include "kdcrmdata_export.h"
#include <QXmlStreamReader>

class SugarDocument;
class QIODevice;

class KDCRMDATA_EXPORT SugarDocumentIO
{
public:
    SugarDocumentIO();
    bool readSugarDocument(QIODevice *device, SugarDocument &document);
    bool writeSugarDocument(const SugarDocument &document, QIODevice *device);
    QString errorString() const;

private:
    QXmlStreamReader xml;
    void readDocument(SugarDocument &document);

};

#endif /* SUGARDOCUMENTIO_H */

