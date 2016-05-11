/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef ITEMDOWNLOADINTERFACE_H
#define ITEMDOWNLOADINTERFACE_H

#include <QObject>

class SugarCRMResource;

class ItemDownloadInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.kdab.SugarCRM.ItemDownload")

public:
    explicit ItemDownloadInterface(SugarCRMResource *resource);

public Q_SLOTS:
    /**
     * Downloads the document via SOAP and returns the path to a local temp file.
     * The caller is responsible to clean up the temp file.
     */
    Q_SCRIPTABLE QString downloadDocumentRevision(const QString &documentRevisionId) const;

private:
    SugarCRMResource *const mResource;
};

#endif
