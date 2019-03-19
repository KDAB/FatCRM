/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "campaignspage.h"

#include "campaigndataextractor.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"
#include "referenceddata.h"

#include "kdcrmdata/sugarcampaign.h"

#include <KLocalizedString>

using namespace Akonadi;

CampaignsPage::CampaignsPage(QWidget *parent)
    : Page(parent, QString(SugarCampaign::mimeType()), Campaign), mDataExtractor(new CampaignDataExtractor(this))
{
    setFilter(new FilterProxyModel(Campaign, this));
}

CampaignsPage::~CampaignsPage()
{
}

QString CampaignsPage::reportTitle() const
{
    return i18n("List of Campaigns");
}

void CampaignsPage::handleNewRows(int start, int end, bool emitChanges)
{
    //qDebug(); QElapsedTimer dt; dt.start();
    //QMap<QString, QString> campaignRefMap;
    QMap<QString, QString> assignedToRefMap;
    ItemsTreeModel *treeModel = itemsTreeModel();
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = treeModel->index(row, 0);
        const Item item = treeModel->data(index, EntityTreeModel::ItemRole).value<Item>();
        if (item.hasPayload<SugarCampaign>()) {
            const SugarCampaign campaign = item.payload<SugarCampaign>();
            //campaignRefMap.insert(campaign.id(), campaign.name());
            assignedToRefMap.insert(campaign.assignedUserId(), campaign.assignedUserName());
        }
    }
    //ReferencedData::instance(CampaignRef)->addMap(campaignRefMap, emitChanges);
    ReferencedData::instance(AssignedToRef)->addMap(assignedToRefMap, emitChanges);
    //qDebug() << "done," << dt.elapsed() << "ms";
}

ItemDataExtractor *CampaignsPage::itemDataExtractor() const
{
    return mDataExtractor;
}
