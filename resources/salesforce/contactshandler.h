/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef CONTACTSHANDLER_H
#define CONTACTSHANDLER_H

#include "modulehandler.h"

class AccessorPair;

template <typename U, typename V> class QHash;

class ContactsHandler : public ModuleHandler
{
public:
    ContactsHandler();

    ~ContactsHandler();

    QStringList supportedFields() const;

    virtual void setDescriptionResult(const TNS__DescribeSObjectResult &description);

    Akonadi::Collection collection() const;

    virtual void listEntries(const TNS__QueryLocator &locator, SforceService *soap);

    virtual bool setEntry(const Akonadi::Item &item, SforceService *soap);

    virtual Akonadi::Item::List itemsFromListEntriesResponse(const TNS__QueryResult &queryResult,
            const Akonadi::Collection &parentCollection);
private:
    typedef QHash<QString, AccessorPair> AccessorHash;
    AccessorHash *mAccessors;
};

#endif
