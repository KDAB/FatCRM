/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2021-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef SUGARCONTACTWRAPPER_H
#define SUGARCONTACTWRAPPER_H

#include "kdcrmdata_export.h"
#include <KContacts/Addressee>

class KDCRMDATA_EXPORT SugarContactWrapper
{
public:
    explicit SugarContactWrapper(const KContacts::Addressee &addressee);

    QString id() const;
    void setId(const QString &value);

    QString fullName() const;

    QString accountId() const;
    QString assignedUserId() const;
    QString assignedUserName() const;
    QString assistant() const;
    QString cAcceptStatusFields() const;
    QString campaign() const;
    QString campaignId() const;
    QString createdBy() const;
    QString createdByName() const;
    QString dateCreated() const;
    QString dateEntered() const;
    QString dateModified() const;
    QString deleted() const;
    QString doNotCall() const;
    QString invalidEmail() const;
    QString leadSource() const;
    QString mAcceptStatusFields() const;
    QString modifiedByName() const;
    QString modifiedUserId() const;
    QString opportunityRoleFields() const;
    QString phoneAssistant() const;
    QString reportsTo() const;
    QString reportsToId() const;
    QString salutation() const;

    void setAccountId(const QString &value);
    void setAssignedUserId(const QString &value);
    void setAssignedUserName(const QString &value);
    void setAssistant(const QString &value);
    void setCAcceptStatusFields(const QString &value);
    void setCampaign(const QString &value);
    void setCampaignId(const QString &value);
    void setCreatedBy(const QString &value);
    void setCreatedByName(const QString &value);
    void setDateCreated(const QString &value);
    void setDateEntered(const QString &value);
    void setDateModified(const QString &value);
    void setDeleted(const QString &value);
    void setDoNotCall(const QString &value);
    void setInvalidEmail(const QString &value);
    void setLeadSource(const QString &value);
    void setMAcceptStatusFields(const QString &value);
    void setModifiedByName(const QString &value);
    void setModifiedUserId(const QString &value);
    void setOpportunityRoleFields(const QString &value);
    void setPhoneAssistant(const QString &value);
    void setReportsTo(const QString &value);
    void setReportsToId(const QString &value);
    void setSalutation(const QString &value);

private:
    KContacts::Addressee &m_addressee;
};

#endif // SUGARCONTACTWRAPPER_H
