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

#include "campaigndetails.h"

#include "editcalendarbutton.h"
#include "ui_campaigndetails.h"
#include "referenceddatamodel.h"

#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/kdcrmfields.h"
#include "kdcrmdata/sugarcampaign.h"

CampaignDetails::CampaignDetails(QWidget *parent)
    : Details(Campaign, parent), mUi(new Ui::CampaignDetails)

{
    mUi->setupUi(this);
    initialize();
}

CampaignDetails::~CampaignDetails()
{
    delete mUi;
}

void CampaignDetails::initialize()
{
    connect(mUi->clearStartDateButton, SIGNAL(clicked()), this, SLOT(slotClearStartDate()));
    connect(mUi->startDateCalendarButton->calendarWidget(), SIGNAL(clicked(QDate)),
            this, SLOT(slotSetStartDate()));
    connect(mUi->clearEndDateButton, SIGNAL(clicked()), this, SLOT(slotClearEndDate()));
    connect(mUi->endDateCalendarButton->calendarWidget(), SIGNAL(clicked(QDate)),
            this, SLOT(slotSetEndDate()));

    ReferencedDataModel::setModelForCombo(mUi->assigned_user_id, AssignedToRef);
}

QUrl CampaignDetails::itemUrl() const
{
    return QUrl();
}

void CampaignDetails::slotSetStartDate()
{
    // TODO FIXME: use QDateEdit
    mUi->startDate->setText(KDCRMUtils::dateToString(mUi->startDateCalendarButton->calendarWidget()->selectedDate()));
    mUi->startDateCalendarButton->calendarWidget()->setSelectedDate(QDate::currentDate());
    mUi->startDateCalendarButton->calendarDialog()->close();
}

void CampaignDetails::slotSetEndDate()
{
    mUi->endDate->setText(KDCRMUtils::dateToString(mUi->endDateCalendarButton->calendarWidget()->selectedDate()));
    mUi->endDateCalendarButton->calendarWidget()->setSelectedDate(QDate::currentDate());
    mUi->endDateCalendarButton->calendarDialog()->close();
}

void CampaignDetails::slotClearStartDate()
{
    mUi->startDate->clear();
}

void CampaignDetails::slotClearEndDate()
{
    mUi->endDate->clear();
}

QMap<QString, QString> CampaignDetails::data(const Akonadi::Item &item) const
{
    Q_ASSERT(item.hasPayload<SugarCampaign>());
    SugarCampaign campaign = item.payload<SugarCampaign>();
    return campaign.data();
}

void CampaignDetails::updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const
{
    SugarCampaign campaign;
    if (item.hasPayload<SugarCampaign>()) {
        campaign = item.payload<SugarCampaign>();
    }
    campaign.setData(data);

    item.setMimeType(SugarCampaign::mimeType());
    item.setPayload<SugarCampaign>(campaign);
}

void CampaignDetails::setDataInternal(const QMap<QString, QString> &) const
{
    fillComboBox(mUi->status, KDCRMFields::status());
    fillComboBox(mUi->campaign_type, KDCRMFields::campaignType());
}
