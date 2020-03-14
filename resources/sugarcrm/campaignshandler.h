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

#ifndef CAMPAIGNSHANDLER_H
#define CAMPAIGNSHANDLER_H

#include "modulehandler.h"
#include "kdcrmdata/sugarcampaign.h"

class CampaignsHandler : public ModuleHandler
{
public:
    explicit CampaignsHandler(SugarSession *session);

    Akonadi::Collection handlerCollection() const override;

    QString orderByForListing() const override;
    QStringList supportedSugarFields() const override;

    QStringList supportedCRMFields() const override;

    static KDSoapGenerated::TNS__Name_value_list sugarCampaignToNameValueList(const SugarCampaign &campaign, QList<KDSoapGenerated::TNS__Name_value> itemList = {});
    int setEntry(const Akonadi::Item &item, QString &newId, QString &errorMessage) override;

    Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection, bool &deleted) override;

    void compare(Akonadi::AbstractDifferencesReporter *reporter,
                 const Akonadi::Item &leftItem, const Akonadi::Item &rightItem) override;
};

#endif /* CAMPAIGNSHANDLER_H */
