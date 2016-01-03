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

#include "sugarleadio.h"
#include "sugarlead.h"
#include "kdcrmfields.h"

#include <KLocalizedString>
#include <QIODevice>
#include <QXmlStreamWriter>

SugarLeadIO::SugarLeadIO()
{
}

bool SugarLeadIO::readSugarLead(QIODevice *device, SugarLead &lead)
{
    if (device == 0 || !device->isReadable()) {
        return false;
    }

    lead = SugarLead();
    xml.setDevice(device);
    if (xml.readNextStartElement()) {
        if (xml.name() == "sugarLead"
                && xml.attributes().value("version") == "1.0") {
            readLead(lead);
        } else {
            xml.raiseError(i18n("It is not a sugarLead version 1.0 data."));
        }

    }
    return !xml.error();
}

QString SugarLeadIO::errorString() const
{
    return i18n("%1\nLine %2, column %3",
           xml.errorString(),
           xml.lineNumber(),
           xml.columnNumber());
}

void SugarLeadIO::readLead(SugarLead &lead)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "sugarLead");

    while (xml.readNextStartElement()) {
        if (xml.name() == "id") {
            lead.setId(xml.readElementText());
        } else if (xml.name() == "date_entered") {
            lead.setDateEntered(xml.readElementText());
        } else if (xml.name() == "date_modified") {
            lead.setDateModified(xml.readElementText());
        } else if (xml.name() == "modified_user_id") {
            lead.setModifiedUserId(xml.readElementText());
        } else if (xml.name() == "modified_by_name") {
            lead.setModifiedByName(xml.readElementText());
        } else if (xml.name() == "created_by") {
            lead.setCreatedBy(xml.readElementText());
        } else if (xml.name() == "created_by_name") {
            lead.setCreatedByName(xml.readElementText());
        } else if (xml.name() == KDCRMFields::description()) {
            lead.setDescription(xml.readElementText());
        } else if (xml.name() == KDCRMFields::deleted()) {
            lead.setDeleted(xml.readElementText());
        } else if (xml.name() == KDCRMFields::assignedUserId()) {
            lead.setAssignedUserId(xml.readElementText());
        } else if (xml.name() == KDCRMFields::assignedUserName()) {
            lead.setAssignedUserName(xml.readElementText());
        } else if (xml.name() == KDCRMFields::salutation()) {
            lead.setSalutation(xml.readElementText());
        } else if (xml.name() == "first_name") {
            lead.setFirstName(xml.readElementText());
        } else if (xml.name() == "last_name") {
            lead.setLastName(xml.readElementText());
        } else if (xml.name() == KDCRMFields::title()) {
            lead.setTitle(xml.readElementText());
        } else if (xml.name() == KDCRMFields::department()) {
            lead.setDepartment(xml.readElementText());
        } else if (xml.name() == "do_not_call") {
            lead.setDoNotCall(xml.readElementText());
        } else if (xml.name() == "phone_home") {
            lead.setPhoneHome(xml.readElementText());
        } else if (xml.name() == "phone_mobile") {
            lead.setPhoneMobile(xml.readElementText());
        } else if (xml.name() == "phone_work") {
            lead.setPhoneWork(xml.readElementText());
        } else if (xml.name() == "phone_other") {
            lead.setPhoneOther(xml.readElementText());
        } else if (xml.name() == "phone_fax") {
            lead.setPhoneFax(xml.readElementText());
        } else if (xml.name() == KDCRMFields::email1()) {
            lead.setEmail1(xml.readElementText());
        } else if (xml.name() == KDCRMFields::email2()) {
            lead.setEmail2(xml.readElementText());
        } else if (xml.name() == KDCRMFields::primaryAddressStreet()) {
            lead.setPrimaryAddressStreet(xml.readElementText());
        } else if (xml.name() == KDCRMFields::primaryAddressCity()) {
            lead.setPrimaryAddressCity(xml.readElementText());
        } else if (xml.name() == KDCRMFields::primaryAddressState()) {
            lead.setPrimaryAddressState(xml.readElementText());
        } else if (xml.name() == KDCRMFields::primaryAddressPostalcode()) {
            lead.setPrimaryAddressPostalcode(xml.readElementText());
        } else if (xml.name() == KDCRMFields::primaryAddressCountry()) {
            lead.setPrimaryAddressCountry(xml.readElementText());
        } else if (xml.name() == KDCRMFields::altAddressStreet()) {
            lead.setAltAddressStreet(xml.readElementText());
        } else if (xml.name() == KDCRMFields::altAddressCity()) {
            lead.setAltAddressCity(xml.readElementText());
        } else if (xml.name() == KDCRMFields::altAddressState()) {
            lead.setAltAddressState(xml.readElementText());
        } else if (xml.name() == KDCRMFields::altAddressPostalcode()) {
            lead.setAltAddressPostalcode(xml.readElementText());
        } else if (xml.name() == KDCRMFields::altAddressCountry()) {
            lead.setAltAddressCountry(xml.readElementText());
        } else if (xml.name() == KDCRMFields::assistant()) {
            lead.setAssistant(xml.readElementText());
        } else if (xml.name() == "assistant_phone") {
            lead.setAssistantPhone(xml.readElementText());
        } else if (xml.name() == "converted") {
            lead.setConverted(xml.readElementText());
        } else if (xml.name() == "refered_by") {
            lead.setReferedBy(xml.readElementText());
        } else if (xml.name() == "lead_source") {
            lead.setLeadSource(xml.readElementText());
        } else if (xml.name() == "lead_source_description") {
            lead.setLeadSourceDescription(xml.readElementText());
        } else if (xml.name() == KDCRMFields::status()) {
            lead.setStatus(xml.readElementText());
        } else if (xml.name() == "status_description") {
            lead.setStatusDescription(xml.readElementText());
        } else if (xml.name() == KDCRMFields::reportsToId()) {
            lead.setReportsToId(xml.readElementText());
        } else if (xml.name() == KDCRMFields::reportsTo()) {
            lead.setReportToName(xml.readElementText());
        } else if (xml.name() == "account_name") {
            lead.setAccountName(xml.readElementText());
        } else if (xml.name() == "account_description") {
            lead.setAccountDescription(xml.readElementText());
        } else if (xml.name() == "contact_id") {
            lead.setContactId(xml.readElementText());
        } else if (xml.name() == "account_id") {
            lead.setAccountId(xml.readElementText());
        } else if (xml.name() == "opportunity_id") {
            lead.setOpportunityId(xml.readElementText());
        } else if (xml.name() == "opportunity_name") {
            lead.setOpportunityName(xml.readElementText());
        } else if (xml.name() == "opportunity_amount") {
            lead.setOpportunityAmount(xml.readElementText());
        } else if (xml.name() == "campaign_id") {
            lead.setCampaignId(xml.readElementText());
        } else if (xml.name() == "campaign_name") {
            lead.setCampaignName(xml.readElementText());
        } else if (xml.name() == "c_accept_status_fields") {
            lead.setCAcceptStatusFields(xml.readElementText());
        } else if (xml.name() == "m_accept_status_fields") {
            lead.setMAcceptStatusFields(xml.readElementText());
        } else if (xml.name() == KDCRMFields::birthdate()) {
            lead.setBirthdate(xml.readElementText());
        } else if (xml.name() == "portal_name") {
            lead.setPortalName(xml.readElementText());
        } else if (xml.name() == "portal_app") {
            lead.setPortalApp(xml.readElementText());
        } else {
            xml.skipCurrentElement();
        }
    }
}

bool SugarLeadIO::writeSugarLead(const SugarLead &lead, QIODevice *device)
{
    if (device == 0 || !device->isWritable()) {
        return false;
    }

    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeDTD("<!DOCTYPE sugarLead>");
    writer.writeStartElement("sugarLead");
    writer.writeAttribute("version", "1.0");
    writer.writeTextElement(QString("id"), lead.id());
    writer.writeTextElement(QString("date_entered"), lead.dateEntered());
    writer.writeTextElement(QString("date_modified"), lead.dateModified());
    writer.writeTextElement(QString("modified_user_id"), lead.modifiedUserId());
    writer.writeTextElement(QString("modified_by_name"), lead.modifiedByName());
    writer.writeTextElement(QString("created_by"), lead.createdBy());
    writer.writeTextElement(QString("created_by_name"), lead.createdByName());
    writer.writeTextElement(QString(KDCRMFields::description()), lead.description());
    writer.writeTextElement(QString(KDCRMFields::deleted()), lead.deleted());
    writer.writeTextElement(QString("assigned_user_id"), lead.assignedUserId());
    writer.writeTextElement(KDCRMFields::assignedUserName(), lead.assignedUserName());
    writer.writeTextElement(QString(KDCRMFields::salutation()), lead.salutation());
    writer.writeTextElement(QString("first_name"), lead.firstName());
    writer.writeTextElement(QString("last_name"), lead.lastName());
    writer.writeTextElement(QString(KDCRMFields::title()), lead.title());
    writer.writeTextElement(QString(KDCRMFields::department()), lead.department());
    writer.writeTextElement(QString("do_not_call"), lead.doNotCall());
    writer.writeTextElement(QString("phone_home"), lead.phoneHome());
    writer.writeTextElement(QString("phone_mobile"), lead.phoneMobile());
    writer.writeTextElement(QString("phone_work"), lead.phoneWork());
    writer.writeTextElement(QString("phone_other"), lead.phoneOther());
    writer.writeTextElement(QString("phone_fax"), lead.phoneFax());
    writer.writeTextElement(QString(KDCRMFields::email1()), lead.email1());
    writer.writeTextElement(QString(KDCRMFields::email2()), lead.email2());
    writer.writeTextElement(KDCRMFields::primaryAddressStreet(), lead.primaryAddressStreet());
    writer.writeTextElement(KDCRMFields::primaryAddressCity(), lead.primaryAddressCity());
    writer.writeTextElement(KDCRMFields::primaryAddressState(), lead.primaryAddressState());
    writer.writeTextElement(KDCRMFields::primaryAddressPostalcode(), lead.primaryAddressPostalcode());
    writer.writeTextElement(KDCRMFields::primaryAddressCountry(), lead.primaryAddressCountry());
    writer.writeTextElement(KDCRMFields::altAddressStreet(), lead.altAddressStreet());
    writer.writeTextElement(KDCRMFields::altAddressCity(), lead.altAddressCity());
    writer.writeTextElement(KDCRMFields::altAddressState(), lead.altAddressState());
    writer.writeTextElement(KDCRMFields::altAddressPostalcode(), lead.altAddressPostalcode());
    writer.writeTextElement(KDCRMFields::altAddressCountry(), lead.altAddressCountry());
    writer.writeTextElement(QString(KDCRMFields::assistant()), lead.assistant());
    writer.writeTextElement(QString("assistant_phone"), lead.assistantPhone());
    writer.writeTextElement(QString("converted"), lead.converted());
    writer.writeTextElement(QString("refered_by"), lead.referedBy());
    writer.writeTextElement(QString("lead_source"), lead.leadSource());
    writer.writeTextElement(QString("lead_source_description"), lead.leadSourceDescription());
    writer.writeTextElement(KDCRMFields::status(), lead.status());
    writer.writeTextElement(QString("status_description"), lead.statusDescription());
    writer.writeTextElement(KDCRMFields::reportsToId(), lead.reportsToId());
    writer.writeTextElement(KDCRMFields::reportsTo(), lead.reportToName());
    writer.writeTextElement(QString("account_name"), lead.accountName());
    writer.writeTextElement(QString("account_description"), lead.accountDescription());
    writer.writeTextElement(QString("contact_id"), lead.contactId());
    writer.writeTextElement(QString("account_id"), lead.accountId());
    writer.writeTextElement(QString("opportunity_id"), lead.opportunityId());
    writer.writeTextElement(QString("opportunity_name"), lead.opportunityName());
    writer.writeTextElement(QString("opportunity_amount"), lead.opportunityAmount());
    writer.writeTextElement(QString("campaign_id"), lead.campaignId());
    writer.writeTextElement(QString("campaign_name"), lead.campaignName());
    writer.writeTextElement(QString("c_accept_status_fields"), lead.cAcceptStatusFields());
    writer.writeTextElement(QString("m_accept_status_fields"), lead.mAcceptStatusFields());
    writer.writeTextElement(QString(KDCRMFields::birthdate()), lead.birthdate());
    writer.writeTextElement(QString("portal_name"), lead.portalName());
    writer.writeTextElement(QString("portal_app"), lead.portalApp());

    writer.writeEndDocument();

    return true;
}
