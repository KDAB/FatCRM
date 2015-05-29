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

#include "leaddetails.h"

#include "editcalendarbutton.h"
#include "ui_leaddetails.h"
#include "referenceddatamodel.h"
#include "kdcrmutils.h"

#include <kdcrmdata/sugarlead.h>

LeadDetails::LeadDetails(QWidget *parent)
    : Details(Lead, parent), mUi(new Ui::LeadDetails)

{
    mUi->setupUi(this);
    initialize();
}

LeadDetails::~LeadDetails()
{
    delete mUi;
}

void LeadDetails::initialize()
{
    mUi->lead_source->addItems(sourceItems());
    ReferencedDataModel::setModelForCombo(mUi->campaign_name, CampaignRef);
    mUi->salutation->addItems(salutationItems());
    // TODO FIXME: leads can refer to account names which are do not match sugar accounts
    ReferencedDataModel::setModelForCombo(mUi->account_name, AccountRef);
    ReferencedDataModel::setModelForCombo(mUi->assigned_user_name, AssignedToRef);

    connect(mUi->clearDateButton, SIGNAL(clicked()), this, SLOT(slotClearDate()));
    connect(mUi->calendarButton->calendarWidget(), SIGNAL(clicked(QDate)),
            this, SLOT(slotSetBirthDate()));

    mUi->status->addItems(statusItems());
}

void LeadDetails::slotSetBirthDate()
{
    mUi->birthdate->setText(KDCRMUtils::dateToString(mUi->calendarButton->calendarWidget()->selectedDate()));
    mUi->calendarButton->calendarWidget()->setSelectedDate(QDate::currentDate());
    mUi->calendarButton->calendarDialog()->close();
}

void LeadDetails::slotClearDate()
{
    mUi->birthdate->clear();
}

QStringList LeadDetails::statusItems() const
{
    QStringList status;
    status << QString("") << QString("New")
           << QString("Assigned") << QString("In Process")
           << QString("Converted") << QString("Recycled")
           << QString("Dead");
    return status;
}

QMap<QString, QString> LeadDetails::data(const Akonadi::Item &item) const
{
    SugarLead lead = item.payload<SugarLead>();
    return lead.data();
}

void LeadDetails::updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const
{
    SugarLead lead;
    if (item.hasPayload<SugarLead>()) {
        lead = item.payload<SugarLead>();
    }
    lead.setData(data);

    item.setMimeType(SugarLead::mimeType());
    item.setPayload<SugarLead>(lead);
}
