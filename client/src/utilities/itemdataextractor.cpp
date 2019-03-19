/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

ItemDataExtractor::ItemDataExtractor(QObject *parent) :
    QObject(parent)
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

ItemDataExtractor *ItemDataExtractor::createDataExtractor(DetailsType type, QObject *parent)
{
    switch(type) {
    case Account:
        return new AccountDataExtractor(parent);
    case Opportunity:
        return new OpportunityDataExtractor(parent);
    case Lead:
        return new LeadDataExtractor(parent);
    case Contact:
        return new ContactDataExtractor(parent);
    case Campaign:
        return new CampaignDataExtractor(parent);
    default:
        return nullptr;
    }
}
