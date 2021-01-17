/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Michel Boyer de la Giroday <michel.giroday@kdab.com>

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

#include "itemdataextractor.h"

#include "accountdataextractor.h"
#include "campaigndataextractor.h"
#include "contactdataextractor.h"
#include "leaddataextractor.h"
#include "opportunitydataextractor.h"

ItemDataExtractor::ItemDataExtractor()
{

}

ItemDataExtractor::~ItemDataExtractor()
{

}

QUrl ItemDataExtractor::itemUrl(const QString &resourceBaseUrl, const Akonadi::Item &item) const
{
    const QString itemId = idForItem(item);
    return itemUrl(resourceBaseUrl, itemId);
}

QUrl ItemDataExtractor::itemUrl(const QString &resourceBaseUrl, const QString &itemId) const
{
    if (resourceBaseUrl.isEmpty() || itemId.isEmpty())
        return QUrl();
    return QUrl(resourceBaseUrl + itemAddress() + itemId);
}

std::unique_ptr<ItemDataExtractor> ItemDataExtractor::createDataExtractor(DetailsType type)
{
    using Type = std::unique_ptr<ItemDataExtractor>;

    switch(type) {
    case DetailsType::Account:
        return Type(new AccountDataExtractor);
    case DetailsType::Opportunity:
        return Type(new OpportunityDataExtractor);
    case DetailsType::Lead:
        return Type(new LeadDataExtractor);
    case DetailsType::Contact:
        return Type(new ContactDataExtractor);
    case DetailsType::Campaign:
        return Type(new CampaignDataExtractor);
    }
    return nullptr;
}
