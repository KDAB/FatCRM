/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "campaigndataextractor.h"

#include "kdcrmdata/sugarcampaign.h"

CampaignDataExtractor::CampaignDataExtractor(QObject *parent)
    : ItemDataExtractor(parent)
{

}

CampaignDataExtractor::~CampaignDataExtractor()
{

}

QString CampaignDataExtractor::itemAddress() const
{
    return QString("?action=DetailView&module=Campaigns&record=");
}

QString CampaignDataExtractor::idForItem(const Akonadi::Item &item) const
{
    if (item.hasPayload<SugarCampaign>()) {
        const SugarCampaign campaign = item.payload<SugarCampaign>();
        return campaign.id();
    }
    return QString();
}
