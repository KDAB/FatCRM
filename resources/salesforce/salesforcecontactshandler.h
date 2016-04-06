/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef SALESFORCE_CONTACTSHANDLER_H
#define SALESFORCE_CONTACTSHANDLER_H

#include "salesforcemodulehandler.h"

class ContactAccessorPair;

template <typename U, typename V> class QHash;

// Salesforce Contact Handler
class SalesforceContactsHandler : public SalesforceModuleHandler
{
public:
    SalesforceContactsHandler();

    ~SalesforceContactsHandler();

    QStringList supportedFields() const Q_DECL_OVERRIDE;

    void setDescriptionResult(const KDSoapGenerated::TNS__DescribeSObjectResult &description) Q_DECL_OVERRIDE;

    Akonadi::Collection collection() const Q_DECL_OVERRIDE;

    void listEntries(const KDSoapGenerated::TNS__QueryLocator &locator, KDSoapGenerated::SforceService *soap) Q_DECL_OVERRIDE;

    bool setEntry(const Akonadi::Item &item, KDSoapGenerated::SforceService *soap) Q_DECL_OVERRIDE;

    Akonadi::Item::List itemsFromListEntriesResponse(const KDSoapGenerated::TNS__QueryResult &queryResult,
            const Akonadi::Collection &parentCollection) Q_DECL_OVERRIDE;
private:
    typedef QHash<QString, ContactAccessorPair> ContactAccessorHash;
    ContactAccessorHash *mAccessors;
};

#endif
