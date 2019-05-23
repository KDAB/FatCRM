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

#ifndef CAMPAIGNDETAILS_H
#define CAMPAIGNDETAILS_H

#include "details.h"

namespace Ui
{
class CampaignDetails;
}

class CampaignDataExtractor;

class CampaignDetails : public Details
{
    Q_OBJECT
public:
    explicit CampaignDetails(QWidget *parent = nullptr);

    ~CampaignDetails() override;

    ItemDataExtractor *itemDataExtractor() const override;

private Q_SLOTS:
    void slotSetStartDate();
    void slotSetEndDate();
    void slotClearStartDate();
    void slotClearEndDate();

private:
    void initialize();
    QMap<QString, QString> data(const Akonadi::Item &item) const override;
    void updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const override;
    void setDataInternal(const QMap<QString, QString> &) override;

    Ui::CampaignDetails *mUi;
    std::unique_ptr<CampaignDataExtractor> mDataExtractor;
};

#endif /* CAMPAIGNDETAILS_H */

