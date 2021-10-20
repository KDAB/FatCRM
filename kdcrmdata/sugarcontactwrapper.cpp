/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>

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

#include "sugarcontactwrapper.h"

SugarContactWrapper::SugarContactWrapper(const KContacts::Addressee &addressee)
    : m_addressee(const_cast<KContacts::Addressee &>(addressee))
{
}

QString SugarContactWrapper::id() const
{
    return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-ContactId"));
}

void SugarContactWrapper::setId(const QString &value)
{
    m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-ContactId"), value);
}

QString SugarContactWrapper::fullName() const
{
    return m_addressee.givenName() + ' ' + m_addressee.familyName();
}

// you get the idea....

QString SugarContactWrapper::accountId()             const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AccountId")); }
QString SugarContactWrapper::assignedUserId()        const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AssignedUserId")); }
QString SugarContactWrapper::assignedUserName()      const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AssignedUserName")); }
QString SugarContactWrapper::assistant()             const { return m_addressee.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("X-AssistantsName")); }
QString SugarContactWrapper::cAcceptStatusFields()   const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-CacceptStatusFields")); }
QString SugarContactWrapper::campaign()              const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-CampaignName")); }
QString SugarContactWrapper::campaignId()            const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-CampaignId")); }
QString SugarContactWrapper::createdBy()             const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-CreatedById")); }
QString SugarContactWrapper::createdByName()         const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-CreatedByName")); }
QString SugarContactWrapper::dateCreated()           const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-DateCreated")); }
QString SugarContactWrapper::dateEntered()           const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-DateCreated")); }
QString SugarContactWrapper::dateModified()          const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-DateModified")); }
QString SugarContactWrapper::deleted()               const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-Deleted")); }
QString SugarContactWrapper::doNotCall()             const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-DoNotCall")); }
QString SugarContactWrapper::invalidEmail()          const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-InvalidEmail")); }
QString SugarContactWrapper::leadSource()            const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-LeadSourceName")); }
QString SugarContactWrapper::mAcceptStatusFields()   const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-MacceptStatusFields")); }
QString SugarContactWrapper::modifiedByName()        const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-ModifiedByName")); }
QString SugarContactWrapper::modifiedUserId()        const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-ModifiedUserId")); }
QString SugarContactWrapper::opportunityRoleFields() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-OpportunityRoleFields")); }
QString SugarContactWrapper::phoneAssistant()        const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AssistantsPhone")); }
QString SugarContactWrapper::reportsTo()             const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-ReportsToUserName")); }
QString SugarContactWrapper::reportsToId()           const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-ReportsToUserId")); }
QString SugarContactWrapper::salutation()            const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-Salutation")); }

void SugarContactWrapper::setAccountId(const QString &value)             { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-AccountId"), value); }
void SugarContactWrapper::setAssignedUserId(const QString &value)        { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-AssignedUserId"), value); }
void SugarContactWrapper::setAssignedUserName(const QString &value)      { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-AssignedUserName"), value); }
void SugarContactWrapper::setAssistant(const QString &value)             { m_addressee.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("X-AssistantsName"), value); }
void SugarContactWrapper::setCAcceptStatusFields(const QString &value)   { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-CacceptStatusFields"), value); }
void SugarContactWrapper::setCampaign(const QString &value)              { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-CampaignName"), value); }
void SugarContactWrapper::setCampaignId(const QString &value)            { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-CampaignId"), value); }
void SugarContactWrapper::setCreatedBy(const QString &value)             { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-CreatedById"), value); }
void SugarContactWrapper::setCreatedByName(const QString &value)         { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-CreatedByName"), value); }
void SugarContactWrapper::setDateCreated(const QString &value)           { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-DateCreated"), value); }
void SugarContactWrapper::setDateEntered(const QString &value)           { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-DateCreated"), value); }
void SugarContactWrapper::setDateModified(const QString &value)          { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-DateModified"), value); }
void SugarContactWrapper::setDeleted(const QString &value)               { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-Deleted"), value); }
void SugarContactWrapper::setDoNotCall(const QString &value)             { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-DoNotCall"), value); }
void SugarContactWrapper::setInvalidEmail(const QString &value)          { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-InvalidEmail"), value); }
void SugarContactWrapper::setLeadSource(const QString &value)            { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-LeadSourceName"), value); }
void SugarContactWrapper::setMAcceptStatusFields(const QString &value)   { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-MacceptStatusFields"), value); }
void SugarContactWrapper::setModifiedByName(const QString &value)        { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-ModifiedByName"), value); }
void SugarContactWrapper::setModifiedUserId(const QString &value)        { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-ModifiedUserId"), value); }
void SugarContactWrapper::setOpportunityRoleFields(const QString &value) { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-OpportunityRoleFields"), value); }
void SugarContactWrapper::setPhoneAssistant(const QString &value)        { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-AssistantsPhone"), value); }
void SugarContactWrapper::setReportsTo(const QString &value)             { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-ReportsToUserName"), value); }
void SugarContactWrapper::setReportsToId(const QString &value)           { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-ReportsToUserId"), value); }
void SugarContactWrapper::setSalutation(const QString &value)            { m_addressee.insertCustom(QStringLiteral("FATCRM"), QStringLiteral("X-Salutation"), value); }
