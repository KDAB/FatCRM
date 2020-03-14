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

#include "leaddetails.h"

#include "editcalendarbutton.h"
#include "leaddataextractor.h"
#include "ui_leaddetails.h"
#include "referenceddatamodel.h"
#include "kdcrmutils.h"

#include "kdcrmdata/sugarlead.h"
#include "kdcrmdata/kdcrmfields.h"

#include <QDialog>

LeadDetails::LeadDetails(QWidget *parent)
    : Details(DetailsType::Lead, parent), mUi(new Ui::LeadDetails), mDataExtractor(new LeadDataExtractor)

{
    mUi->setupUi(this);

    mUi->status->setObjectName(KDCRMFields::status());
    mUi->statusDescription->setObjectName(KDCRMFields::statusDescription());
    mUi->opportunityAmount->setObjectName(KDCRMFields::opportunityAmount());
    mUi->phone_work->setObjectName(KDCRMFields::phoneWork());
    mUi->phone_mobile->setObjectName(KDCRMFields::phoneMobile());
    mUi->phone_home->setObjectName(KDCRMFields::phoneHome());
    mUi->phone_other->setObjectName(KDCRMFields::phoneOther());
    mUi->phone_fax->setObjectName(KDCRMFields::phoneFax());
    mUi->do_not_call->setObjectName(KDCRMFields::doNotCall());
    mUi->email1->setObjectName(KDCRMFields::email1());
    mUi->email2->setObjectName(KDCRMFields::email2());
    mUi->primaryAddressStreet->setObjectName(KDCRMFields::primaryAddressStreet());
    mUi->primaryAddressCity->setObjectName(KDCRMFields::primaryAddressCity());
    mUi->primaryAddressState->setObjectName(KDCRMFields::primaryAddressState());
    mUi->primaryAddressPostalcode->setObjectName(KDCRMFields::primaryAddressPostalcode());
    mUi->primaryAddressCountry->setObjectName(KDCRMFields::primaryAddressCountry());
    mUi->altAddressStreet->setObjectName(KDCRMFields::altAddressStreet());
    mUi->altAddressCity->setObjectName(KDCRMFields::altAddressCity());
    mUi->altAddressState->setObjectName(KDCRMFields::altAddressState());
    mUi->altAddressPostalcode->setObjectName(KDCRMFields::altAddressPostalcode());
    mUi->altAddressCountry->setObjectName(KDCRMFields::altAddressCountry());
    mUi->lead_source->setObjectName(KDCRMFields::leadSource());
    mUi->leadSourceDescription->setObjectName(KDCRMFields::leadSourceDescription());
    mUi->campaign_id->setObjectName(KDCRMFields::campaignId());
    mUi->referredBy->setObjectName(KDCRMFields::referedBy());
    mUi->salutation->setObjectName(KDCRMFields::salutation());
    mUi->first_name->setObjectName(KDCRMFields::firstName());
    mUi->last_name->setObjectName(KDCRMFields::lastName());
    mUi->birthdate->setObjectName(KDCRMFields::birthdate());
    mUi->account_id->setObjectName(KDCRMFields::accountId());
    mUi->title->setObjectName(KDCRMFields::title());
    mUi->department->setObjectName(KDCRMFields::department());
    mUi->assigned_user_id->setObjectName(KDCRMFields::assignedUserId());

    initialize();
}

LeadDetails::~LeadDetails()
{
    delete mUi;
}

ItemDataExtractor *LeadDetails::itemDataExtractor() const
{
    return mDataExtractor.get();
}

void LeadDetails::initialize()
{
    //ReferencedDataModel::setModelForCombo(mUi->campaign_id, CampaignRef);
    // TODO FIXME: leads can refer to account names which are do not match sugar accounts
    ReferencedDataModel::setModelForCombo(mUi->account_id, AccountRef);
    ReferencedDataModel::setModelForCombo(mUi->assigned_user_id, AssignedToRef);

    connect(mUi->clearDateButton, SIGNAL(clicked()), this, SLOT(slotClearDate()));
    connect(mUi->calendarButton->calendarWidget(), SIGNAL(clicked(QDate)),
            this, SLOT(slotSetBirthDate()));
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

QMap<QString, QString> LeadDetails::data(const Akonadi::Item &item) const
{
    Q_ASSERT(item.hasPayload<SugarLead>());
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

void LeadDetails::setDataInternal(const QMap<QString, QString> &data)
{
    Q_UNUSED(data);
    fillComboBox(mUi->salutation, KDCRMFields::salutation());
    fillComboBox(mUi->lead_source, KDCRMFields::leadSource());
    fillComboBox(mUi->status, KDCRMFields::status());
}
