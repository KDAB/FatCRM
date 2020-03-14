/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

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

#ifndef MODULEHANDLER_H
#define MODULEHANDLER_H

#include <AkonadiCore/Item>

#include <QStringList>

namespace Akonadi
{
class Collection;
}

namespace KDSoapGenerated {
class SforceService;
class TNS__DescribeSObjectResult;
class TNS__QueryLocator;
class TNS__QueryResult;
}

class SalesforceModuleHandler
{
public:
    explicit SalesforceModuleHandler(const QString &moduleName);

    virtual ~SalesforceModuleHandler();

    QString moduleName() const;

    QStringList availableFields() const;

    virtual QStringList supportedFields() const = 0;

    virtual void setDescriptionResult(const KDSoapGenerated::TNS__DescribeSObjectResult &description);

    virtual Akonadi::Collection collection() const = 0;

    virtual void listEntries(const KDSoapGenerated::TNS__QueryLocator &locator, KDSoapGenerated::SforceService *soap) = 0;

    virtual bool setEntry(const Akonadi::Item &item, KDSoapGenerated::SforceService *soap) = 0;

    virtual Akonadi::Item::List itemsFromListEntriesResponse(const KDSoapGenerated::TNS__QueryResult &queryResult,
            const Akonadi::Collection &parentCollection) = 0;

protected:
    QString mModuleName;

    QStringList mAvailableFields;
};

#endif
