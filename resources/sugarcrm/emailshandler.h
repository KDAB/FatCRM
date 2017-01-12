/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef EMAILSHANDLER_H
#define EMAILSHANDLER_H

#include "modulehandler.h"
#include "kdcrmdata/sugaremail.h"


template <typename U, typename V> class QHash;

class EmailsHandler : public ModuleHandler
{
public:
    explicit EmailsHandler(SugarSession *session);

    ~EmailsHandler();

    Akonadi::Collection handlerCollection() const override;

    bool setEntry(const Akonadi::Item &item) Q_DECL_OVERRIDE;

    QString queryStringForListing() const override;
    QString orderByForListing() const override;
    QStringList supportedSugarFields() const override;
    QStringList supportedCRMFields() const override;

    int expectedContentsVersion() const override;

    virtual bool needsExtraInformation() const override { return true; }
    virtual void getExtraInformation(Akonadi::Item::List &items) override;

    Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection) override;

    void compare(Akonadi::AbstractDifferencesReporter *reporter,
                 const Akonadi::Item &leftItem, const Akonadi::Item &rightItem) override;

private:
    SugarEmail::AccessorHash mAccessors;
};

#endif
