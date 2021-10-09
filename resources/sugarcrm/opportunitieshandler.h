/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>
           Jeremy Entressangle <jeremy.entressangle@kdab.com>

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

#ifndef OPPORTUNITIESHANDLER_H
#define OPPORTUNITIESHANDLER_H

#include "modulehandler.h"
#include "kdcrmdata/sugaropportunity.h"
#include <functional>

template <typename U, typename V> class QHash;
class OpportunitiesHandler : public ModuleHandler
{
    Q_OBJECT
public:
    explicit OpportunitiesHandler(SugarSession *session);

    ~OpportunitiesHandler() override;

    Akonadi::Collection handlerCollection() const override;

    int expectedContentsVersion() const override;

    int setEntry(const Akonadi::Item &item, QString &newId, QString &errorMessage) override;

    QString orderByForListing() const override;
    QStringList supportedSugarFields() const override;
    QStringList supportedCRMFields() const override;

    Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection, bool &deleted) override;

    void compare(Akonadi::AbstractDifferencesReporter *reporter,
                 const Akonadi::Item &leftItem, const Akonadi::Item &rightItem) override;

    static KDSoapGenerated::TNS__Name_value_list sugarOpportunityToNameValueList(const SugarOpportunity &opp, QList<KDSoapGenerated::TNS__Name_value> itemList = {});

    static SugarOpportunity nameValueListToSugarOpportunity(const KDSoapGenerated::TNS__Name_value_list &valueList, const QString &id);

private:
    SugarOpportunity::AccessorHash mAccessors;
};

#endif /* OPPORTUNITIESHANDLER_H */
