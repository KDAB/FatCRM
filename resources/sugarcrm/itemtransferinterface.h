/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef ITEMTRANSFERINTERFACE_H
#define ITEMTRANSFERINTERFACE_H

#include <QObject>

class SugarSession;

class ItemTransferInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.kdab.SugarCRM.ItemTransfer")

public:
    explicit ItemTransferInterface(QObject *parent = nullptr);

    void setSession(SugarSession *session);

public Q_SLOTS:
    /**
     * Downloads the document via SOAP and returns the path to a local temp file.
     * The caller is responsible to clean up the temp file.
     */
    Q_SCRIPTABLE QString downloadDocumentRevision(const QString &documentRevisionId) const;

    /**
     * Creates a new document via SOAP and returns the document id or an empty
     * string in case of an error.
     */
    Q_SCRIPTABLE QString uploadDocument(const QString &documentName, const QString &statusId, const QString &description, const QString &localFilePath) const;

    /**
     * Links the two items together name.
     */
    Q_SCRIPTABLE bool linkItem(const QString &sourceItemId, const QString &sourceModuleName,
                               const QString &targetItemId, const QString &targetModuleName) const;

private:
    SugarSession *mSession;
};

#endif
