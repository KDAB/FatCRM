/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sugarlead.h"
#include "kdcrmfields.h"

#include <KLocalizedString>

#include <QHash>
#include <QMap>
#include <QSharedData>
#include <QString>

class SugarLead::Private : public QSharedData
{
public:
    Private()
        : mEmpty(true)
    {
    }

    bool mEmpty;

    QString mId;
    QString mDateEntered;
    QString mDateModified;
    QString mModifiedUserId;
    QString mModifiedByName;
    QString mCreatedBy;
    QString mCreatedByName;
    QString mDescription;
    QString mDeleted;
    QString mAssignedUserId;
    QString mAssignedUserName;
    QString mSalutation;
    QString mFirstName;
    QString mLastName;
    QString mTitle;
    QString mDepartment;
    QString mDoNotCall;
    QString mPhoneHome;
    QString mPhoneMobile;
    QString mPhoneWork;
    QString mPhoneOther;
    QString mPhoneFax;
    QString mEmail1;
    QString mEmail2;
    QString mPrimaryAddressStreet;
    QString mPrimaryAddressCity;
    QString mPrimaryAddressState;
    QString mPrimaryAddressPostalcode;
    QString mPrimaryAddressCountry;
    QString mAltAddressStreet;
    QString mAltAddressCity;
    QString mAltAddressState;
    QString mAltAddressPostalcode;
    QString mAltAddressCountry;
    QString mAssistant;
    QString mAssistantPhone;
    QString mConverted;
    QString mReferedBy;
    QString mLeadSource;
    QString mLeadSourceDescription;
    QString mStatus;
    QString mStatusDescription;
    QString mReportsToId;
    QString mReportToName;
    QString mAccountName;
    QString mAccountDescription;
    QString mContactId;
    QString mAccountId;
    QString mOpportunityId;
    QString mOpportunityName;
    QString mOpportunityAmount;
    QString mCampaignId;
    QString mCampaignName;
    QString mCAcceptStatusFields;
    QString mMAcceptStatusFields;
    QString mBirthdate;
    QString mPortalName;
    QString mPortalApp;
};

SugarLead::SugarLead()
    : d(new Private)
{
}

SugarLead::SugarLead(const SugarLead &other)
    : d(other.d)
{
}

SugarLead::~SugarLead()
{
}

SugarLead &SugarLead::operator=(const SugarLead &other)
{
    if (this != &other) {
        d = other.d;
    }

    return *this;
}

bool SugarLead::isEmpty() const
{
    return d->mEmpty;
}

void SugarLead::clear()
{
    *this = SugarLead();
}

void SugarLead::setId(const QString &id)
{
    d->mEmpty = false;
    d->mId = id;
}

QString SugarLead::id() const
{
    return d->mId;
}

void SugarLead::setDateEntered(const QString &value)
{
    d->mEmpty = false;
    d->mDateEntered = value;
}

QString SugarLead::dateEntered() const
{
    return d->mDateEntered;
}

void SugarLead::setDateModified(const QString &value)
{
    d->mEmpty = false;
    d->mDateModified = value;
}

QString SugarLead::dateModified() const
{
    return d->mDateModified;
}

void SugarLead::setModifiedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedUserId = value;
}

QString SugarLead::modifiedUserId() const
{
    return d->mModifiedUserId;
}

void SugarLead::setModifiedByName(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedByName = value;
}

QString SugarLead::modifiedByName() const
{
    return d->mModifiedByName;
}

void SugarLead::setCreatedBy(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedBy = value;
}

QString SugarLead::createdBy() const
{
    return d->mCreatedBy;
}

void SugarLead::setCreatedByName(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedByName = value;
}

QString SugarLead::createdByName() const
{
    return d->mCreatedByName;
}

void SugarLead::setDescription(const QString &value)
{
    d->mEmpty = false;
    d->mDescription = value;
}

QString SugarLead::description() const
{
    return d->mDescription;
}

void SugarLead::setDeleted(const QString &value)
{
    d->mEmpty = false;
    d->mDeleted = value;
}

QString SugarLead::deleted() const
{
    return d->mDeleted;
}

void SugarLead::setAssignedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserId = value;
}

QString SugarLead::assignedUserId() const
{
    return d->mAssignedUserId;
}

void SugarLead::setAssignedUserName(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserName = value;
}

QString SugarLead::assignedUserName() const
{
    return d->mAssignedUserName;
}

void SugarLead::setSalutation(const QString &value)
{
    d->mEmpty = false;
    d->mSalutation = value;
}

QString SugarLead::salutation() const
{
    return d->mSalutation;
}

void SugarLead::setFirstName(const QString &value)
{
    d->mEmpty = false;
    d->mFirstName = value;
}

QString SugarLead::firstName() const
{
    return d->mFirstName;
}

void SugarLead::setLastName(const QString &value)
{
    d->mEmpty = false;
    d->mLastName = value;
}

QString SugarLead::lastName() const
{
    return d->mLastName;
}

void SugarLead::setTitle(const QString &value)
{
    d->mEmpty = false;
    d->mTitle = value;
}

QString SugarLead::title() const
{
    return d->mTitle;
}

void SugarLead::setDepartment(const QString &value)
{
    d->mEmpty = false;
    d->mDepartment = value;
}

QString SugarLead::department() const
{
    return d->mDepartment;
}

void SugarLead::setDoNotCall(const QString &value)
{
    d->mEmpty = false;
    d->mDoNotCall = value;
}

QString SugarLead::doNotCall() const
{
    return d->mDoNotCall;
}

void SugarLead::setPhoneHome(const QString &value)
{
    d->mEmpty = false;
    d->mPhoneHome = value;
}

QString SugarLead::phoneHome() const
{
    return d->mPhoneHome;
}

void SugarLead::setPhoneMobile(const QString &value)
{
    d->mEmpty = false;
    d->mPhoneMobile = value;
}

QString SugarLead::phoneMobile() const
{
    return d->mPhoneMobile;
}

void SugarLead::setPhoneWork(const QString &value)
{
    d->mEmpty = false;
    d->mPhoneWork = value;
}

QString SugarLead::phoneWork() const
{
    return d->mPhoneWork;
}

void SugarLead::setPhoneOther(const QString &value)
{
    d->mEmpty = false;
    d->mPhoneOther = value;
}

QString SugarLead::phoneOther() const
{
    return d->mPhoneOther;
}

void SugarLead::setPhoneFax(const QString &value)
{
    d->mEmpty = false;
    d->mPhoneFax = value;
}

QString SugarLead::phoneFax() const
{
    return d->mPhoneFax;
}

void SugarLead::setEmail1(const QString &value)
{
    d->mEmpty = false;
    d->mEmail1 = value;
}

QString SugarLead::email1() const
{
    return d->mEmail1;
}

void SugarLead::setEmail2(const QString &value)
{
    d->mEmpty = false;
    d->mEmail2 = value;
}

QString SugarLead::email2() const
{
    return d->mEmail2;
}

void SugarLead::setPrimaryAddressStreet(const QString &value)
{
    d->mEmpty = false;
    d->mPrimaryAddressStreet = value;
}

QString SugarLead::primaryAddressStreet() const
{
    return d->mPrimaryAddressStreet;
}

void SugarLead::setPrimaryAddressCity(const QString &value)
{
    d->mEmpty = false;
    d->mPrimaryAddressCity = value;
}

QString SugarLead::primaryAddressCity() const
{
    return d->mPrimaryAddressCity;
}

void SugarLead::setPrimaryAddressState(const QString &value)
{
    d->mEmpty = false;
    d->mPrimaryAddressState = value;
}

QString SugarLead::primaryAddressState() const
{
    return d->mPrimaryAddressState;
}

void SugarLead::setPrimaryAddressPostalcode(const QString &value)
{
    d->mEmpty = false;
    d->mPrimaryAddressPostalcode = value;
}

QString SugarLead::primaryAddressPostalcode() const
{
    return d->mPrimaryAddressPostalcode;
}

void SugarLead::setPrimaryAddressCountry(const QString &value)
{
    d->mEmpty = false;
    d->mPrimaryAddressCountry = value;
}

QString SugarLead::primaryAddressCountry() const
{
    return d->mPrimaryAddressCountry;
}

void SugarLead::setAltAddressStreet(const QString &value)
{
    d->mEmpty = false;
    d->mAltAddressStreet = value;
}

QString SugarLead::altAddressStreet() const
{
    return d->mAltAddressStreet;
}

void SugarLead::setAltAddressCity(const QString &value)
{
    d->mEmpty = false;
    d->mAltAddressCity = value;
}

QString SugarLead::altAddressCity() const
{
    return d->mAltAddressCity;
}

void SugarLead::setAltAddressState(const QString &value)
{
    d->mEmpty = false;
    d->mAltAddressState = value;
}

QString SugarLead::altAddressState() const
{
    return d->mAltAddressState;
}

void SugarLead::setAltAddressPostalcode(const QString &value)
{
    d->mEmpty = false;
    d->mAltAddressPostalcode = value;
}

QString SugarLead::altAddressPostalcode() const
{
    return d->mAltAddressPostalcode;
}

void SugarLead::setAltAddressCountry(const QString &value)
{
    d->mEmpty = false;
    d->mAltAddressCountry = value;
}

QString SugarLead::altAddressCountry() const
{
    return d->mAltAddressCountry;
}

void SugarLead::setAssistant(const QString &value)
{
    d->mEmpty = false;
    d->mAssistant = value;
}

QString SugarLead::assistant() const
{
    return d->mAssistant;
}

void SugarLead::setAssistantPhone(const QString &value)
{
    d->mEmpty = false;
    d->mAssistantPhone = value;
}

QString SugarLead::assistantPhone() const
{
    return d->mAssistantPhone;
}

void SugarLead::setConverted(const QString &value)
{
    d->mEmpty = false;
    d->mConverted = value;
}

QString SugarLead::converted() const
{
    return d->mConverted;
}

void SugarLead::setReferedBy(const QString &value)
{
    d->mEmpty = false;
    d->mReferedBy = value;
}

QString SugarLead::referedBy() const
{
    return d->mReferedBy;
}

void SugarLead::setLeadSource(const QString &value)
{
    d->mEmpty = false;
    d->mLeadSource = value;
}

QString SugarLead::leadSource() const
{
    return d->mLeadSource;
}

void SugarLead::setLeadSourceDescription(const QString &value)
{
    d->mEmpty = false;
    d->mLeadSourceDescription = value;
}

QString SugarLead::leadSourceDescription() const
{
    return d->mLeadSourceDescription;
}

void SugarLead::setStatus(const QString &value)
{
    d->mEmpty = false;
    d->mStatus = value;
}

QString SugarLead::status() const
{
    return d->mStatus;
}

void SugarLead::setStatusDescription(const QString &value)
{
    d->mEmpty = false;
    d->mStatusDescription = value;
}

QString SugarLead::statusDescription() const
{
    return d->mStatusDescription;
}

void SugarLead::setReportsToId(const QString &value)
{
    d->mEmpty = false;
    d->mReportsToId = value;
}

QString SugarLead::reportsToId() const
{
    return d->mReportsToId;
}

void SugarLead::setReportToName(const QString &value)
{
    d->mEmpty = false;
    d->mReportToName = value;
}

QString SugarLead::reportToName() const
{
    return d->mReportToName;
}

void SugarLead::setAccountName(const QString &value)
{
    d->mEmpty = false;
    d->mAccountName = value;
}

QString SugarLead::accountName() const
{
    return d->mAccountName;
}

void SugarLead::setAccountDescription(const QString &value)
{
    d->mEmpty = false;
    d->mAccountDescription = value;
}

QString SugarLead::accountDescription() const
{
    return d->mAccountDescription;
}

void SugarLead::setContactId(const QString &value)
{
    d->mEmpty = false;
    d->mContactId = value;
}

QString SugarLead::contactId() const
{
    return d->mContactId;
}

void SugarLead::setAccountId(const QString &value)
{
    d->mEmpty = false;
    d->mAccountId = value;
}

QString SugarLead::accountId() const
{
    return d->mAccountId;
}

void SugarLead::setOpportunityId(const QString &value)
{
    d->mEmpty = false;
    d->mOpportunityId = value;
}

QString SugarLead::opportunityId() const
{
    return d->mOpportunityId;
}

void SugarLead::setOpportunityName(const QString &value)
{
    d->mEmpty = false;
    d->mOpportunityName = value;
}

QString SugarLead::opportunityName() const
{
    return d->mOpportunityName;
}

void SugarLead::setOpportunityAmount(const QString &value)
{
    d->mEmpty = false;
    d->mOpportunityAmount = value;
}

QString SugarLead::opportunityAmount() const
{
    return d->mOpportunityAmount;
}

void SugarLead::setCampaignId(const QString &value)
{
    d->mEmpty = false;
    d->mCampaignId = value;
}

QString SugarLead::campaignId() const
{
    return d->mCampaignId;
}

void SugarLead::setCampaignName(const QString &value)
{
    d->mEmpty = false;
    d->mCampaignName = value;
}

QString SugarLead::campaignName() const
{
    return d->mCampaignName;
}

void SugarLead::setCAcceptStatusFields(const QString &value)
{
    d->mEmpty = false;
    d->mCAcceptStatusFields = value;
}

QString SugarLead::cAcceptStatusFields() const
{
    return d->mCAcceptStatusFields;
}

void SugarLead::setMAcceptStatusFields(const QString &value)
{
    d->mEmpty = false;
    d->mMAcceptStatusFields = value;
}

QString SugarLead::mAcceptStatusFields() const
{
    return d->mMAcceptStatusFields;
}

void SugarLead::setBirthdate(const QString &value)
{
    d->mEmpty = false;
    d->mBirthdate = value;
}

QString SugarLead::birthdate() const
{
    return d->mBirthdate;
}

void SugarLead::setPortalName(const QString &value)
{
    d->mEmpty = false;
    d->mPortalName = value;
}

QString SugarLead::portalName() const
{
    return d->mPortalName;
}

void SugarLead::setPortalApp(const QString &value)
{
    d->mEmpty = false;
    d->mPortalApp = value;
}

QString SugarLead::portalApp() const
{
    return d->mPortalApp;
}

void SugarLead::setData(const QMap<QString, QString>& data)
{
    d->mEmpty = false;

    const SugarLead::AccessorHash accessors = SugarLead::accessorHash();
    QMap<QString, QString>::const_iterator it = data.constBegin();
    for ( ; it != data.constEnd() ; ++it) {
        const SugarLead::AccessorHash::const_iterator accessIt = accessors.constFind(it.key());
        if (accessIt != accessors.constEnd()) {
            (this->*(accessIt.value().setter))(it.value());
        } else {
            //TODO: add custom field support
            // d->mCustomFields.insert(it.key(), it.value());
        }
    }
}

QMap<QString, QString> SugarLead::data()
{
    QMap<QString, QString> data;

    const SugarLead::AccessorHash accessors = SugarLead::accessorHash();
    SugarLead::AccessorHash::const_iterator it    = accessors.constBegin();
    SugarLead::AccessorHash::const_iterator endIt = accessors.constEnd();
    for (; it != endIt; ++it) {
        const SugarLead::valueGetter getter = (*it).getter;
        data.insert(it.key(), (this->*getter)());
    }

/*TODO: add custom field support
    // plus custom fields
    QMap<QString, QString>::const_iterator cit = d->mCustomFields.constBegin();
    const QMap<QString, QString>::const_iterator end = d->mCustomFields.constEnd();
    for ( ; cit != end ; ++cit ) {
        data.insert(cit.key(), cit.value());
    }
*/

    return data;
}

QString SugarLead::mimeType()
{
    return QStringLiteral("application/x-vnd.kdab.crm.lead");
}

Q_GLOBAL_STATIC(SugarLead::AccessorHash, s_accessors)

SugarLead::AccessorHash SugarLead::accessorHash()
{
    AccessorHash &accessors = *s_accessors();
    if (accessors.isEmpty()) {
        accessors.insert(KDCRMFields::id(),
                         LeadAccessorPair(&SugarLead::id, &SugarLead::setId, QString()));
        accessors.insert(KDCRMFields::dateEntered(),
                         LeadAccessorPair(&SugarLead::dateEntered, &SugarLead::setDateEntered, QString()));
        accessors.insert(KDCRMFields::dateModified(),
                         LeadAccessorPair(&SugarLead::dateModified, &SugarLead::setDateModified, QString()));
        accessors.insert(KDCRMFields::modifiedUserId(),
                         LeadAccessorPair(&SugarLead::modifiedUserId, &SugarLead::setModifiedUserId, QString()));
        accessors.insert(KDCRMFields::modifiedByName(),
                         LeadAccessorPair(&SugarLead::modifiedByName, &SugarLead::setModifiedByName, QString()));
        accessors.insert(KDCRMFields::createdBy(),
                         LeadAccessorPair(&SugarLead::createdBy, &SugarLead::setCreatedBy, QString()));
        accessors.insert(KDCRMFields::createdByName(),
                         LeadAccessorPair(&SugarLead::createdByName, &SugarLead::setCreatedByName, QString()));
        accessors.insert(KDCRMFields::description(),
                         LeadAccessorPair(&SugarLead::description, &SugarLead::setDescription,
                                          i18nc("@item:intable", "Description")));
        accessors.insert(KDCRMFields::deleted(),
                         LeadAccessorPair(&SugarLead::deleted, &SugarLead::setDeleted, QString()));
        accessors.insert(KDCRMFields::assignedUserId(),
                         LeadAccessorPair(&SugarLead::assignedUserId, &SugarLead::setAssignedUserId, QString()));
        accessors.insert(KDCRMFields::assignedUserName(),
                         LeadAccessorPair(&SugarLead::assignedUserName, &SugarLead::setAssignedUserName,
                                          i18nc("@item:intable", "Assigned To")));
        accessors.insert(KDCRMFields::salutation(),
                         LeadAccessorPair(&SugarLead::salutation, &SugarLead::setSalutation,
                                          i18nc("@item:intable", "Salutation")));
        accessors.insert(KDCRMFields::firstName(),
                         LeadAccessorPair(&SugarLead::firstName, &SugarLead::setFirstName,
                                          i18nc("@item:intable", "First Name")));
        accessors.insert(KDCRMFields::lastName(),
                         LeadAccessorPair(&SugarLead::lastName, &SugarLead::setLastName,
                                          i18nc("@item:intable", "Last Name")));
        accessors.insert(KDCRMFields::title(),
                         LeadAccessorPair(&SugarLead::title, &SugarLead::setTitle,
                                          i18nc("@item:intable job title", "Title")));
        accessors.insert(KDCRMFields::department(),
                         LeadAccessorPair(&SugarLead::department, &SugarLead::setDepartment,
                                          i18nc("@item:intable", "Department")));
        accessors.insert(KDCRMFields::doNotCall(),
                         LeadAccessorPair(&SugarLead::doNotCall, &SugarLead::setDoNotCall, QString()));
        accessors.insert(KDCRMFields::phoneHome(),
                         LeadAccessorPair(&SugarLead::phoneHome, &SugarLead::setPhoneHome,
                                          i18nc("@item:intable", "Phone (Home)")));
        accessors.insert(KDCRMFields::phoneMobile(),
                         LeadAccessorPair(&SugarLead::phoneMobile, &SugarLead::setPhoneMobile,
                                          i18nc("@item:intable", "Phone (Mobile)")));
        accessors.insert(KDCRMFields::phoneWork(),
                         LeadAccessorPair(&SugarLead::phoneWork, &SugarLead::setPhoneWork,
                                          i18nc("@item:intable", "Phone (Office)")));
        accessors.insert(KDCRMFields::phoneOther(),
                         LeadAccessorPair(&SugarLead::phoneOther, &SugarLead::setPhoneOther,
                                          i18nc("@item:intable", "Phone (Other)")));
        accessors.insert(KDCRMFields::phoneFax(),
                         LeadAccessorPair(&SugarLead::phoneFax, &SugarLead::setPhoneFax,
                                          i18nc("@item:intable", "Fax")));
        accessors.insert(KDCRMFields::email1(),
                         LeadAccessorPair(&SugarLead::email1, &SugarLead::setEmail1,
                                          i18nc("@item:intable", "Primary Email")));
        accessors.insert(KDCRMFields::email2(),
                         LeadAccessorPair(&SugarLead::email2, &SugarLead::setEmail2,
                                          i18nc("@item:intable", "Other Email")));
        accessors.insert(KDCRMFields::primaryAddressStreet(),
                         LeadAccessorPair(&SugarLead::primaryAddressStreet, &SugarLead::setPrimaryAddressStreet, QString()));
        accessors.insert(KDCRMFields::primaryAddressCity(),
                         LeadAccessorPair(&SugarLead::primaryAddressCity, &SugarLead::setPrimaryAddressCity, QString()));
        accessors.insert(KDCRMFields::primaryAddressState(),
                         LeadAccessorPair(&SugarLead::primaryAddressState, &SugarLead::setPrimaryAddressState, QString()));
        accessors.insert(KDCRMFields::primaryAddressPostalcode(),
                         LeadAccessorPair(&SugarLead::primaryAddressPostalcode, &SugarLead::setPrimaryAddressPostalcode, QString()));
        accessors.insert(KDCRMFields::primaryAddressCountry(),
                         LeadAccessorPair(&SugarLead::primaryAddressCountry, &SugarLead::setPrimaryAddressCountry, QString()));
        accessors.insert(KDCRMFields::altAddressStreet(),
                         LeadAccessorPair(&SugarLead::altAddressStreet, &SugarLead::setAltAddressStreet, QString()));
        accessors.insert(KDCRMFields::altAddressCity(),
                         LeadAccessorPair(&SugarLead::altAddressCity, &SugarLead::setAltAddressCity, QString()));
        accessors.insert(KDCRMFields::altAddressState(),
                         LeadAccessorPair(&SugarLead::altAddressState, &SugarLead::setAltAddressState, QString()));
        accessors.insert(KDCRMFields::altAddressPostalcode(),
                         LeadAccessorPair(&SugarLead::altAddressPostalcode, &SugarLead::setAltAddressPostalcode, QString()));
        accessors.insert(KDCRMFields::altAddressCountry(),
                         LeadAccessorPair(&SugarLead::altAddressCountry, &SugarLead::setAltAddressCountry, QString()));
        accessors.insert(KDCRMFields::assistant(),
                         LeadAccessorPair(&SugarLead::assistant, &SugarLead::setAssistant,
                                          i18nc("@item:intable", "Assistant")));
        accessors.insert(KDCRMFields::phoneAssistant(),
                         LeadAccessorPair(&SugarLead::assistantPhone, &SugarLead::setAssistantPhone,
                                          i18nc("@item:intable", "Assistant Phone")));
        accessors.insert(KDCRMFields::converted(),
                         LeadAccessorPair(&SugarLead::converted, &SugarLead::setConverted, QString()));
        accessors.insert(KDCRMFields::referedBy(),
                         LeadAccessorPair(&SugarLead::referedBy, &SugarLead::setReferedBy,
                                          i18nc("@item:intable", "Referred By")));
        accessors.insert(KDCRMFields::leadSource(),
                         LeadAccessorPair(&SugarLead::leadSource, &SugarLead::setLeadSource,
                                          i18nc("@item:intable", "Lead Source")));
        accessors.insert(KDCRMFields::leadSourceDescription(),
                         LeadAccessorPair(&SugarLead::leadSourceDescription, &SugarLead::setLeadSourceDescription,
                                          i18nc("@item:intable", "Lead Source Description")));
        accessors.insert(KDCRMFields::status(),
                         LeadAccessorPair(&SugarLead::status, &SugarLead::setStatus,
                                          i18nc("@item:intable", "Status")));
        accessors.insert(KDCRMFields::statusDescription(),
                         LeadAccessorPair(&SugarLead::statusDescription, &SugarLead::setStatusDescription,
                                          i18nc("@item:intable", "Status Description")));
        accessors.insert(KDCRMFields::reportsToId(),
                         LeadAccessorPair(&SugarLead::reportsToId, &SugarLead::setReportsToId, QString()));
        accessors.insert(KDCRMFields::reportsTo(),
                         LeadAccessorPair(&SugarLead::reportToName, &SugarLead::setReportToName,
                                          i18nc("@item:intable", "Reports To")));
        accessors.insert(KDCRMFields::accountName(),
                         LeadAccessorPair(&SugarLead::accountName, &SugarLead::setAccountName,
                                          i18nc("@item:intable", "Account")));
        accessors.insert(KDCRMFields::accountDescription(),
                         LeadAccessorPair(&SugarLead::accountDescription, &SugarLead::setAccountDescription,
                                          i18nc("@item:intable", "Account Description")));
        accessors.insert(KDCRMFields::contactId(),
                         LeadAccessorPair(&SugarLead::contactId, &SugarLead::setContactId, QString()));
        accessors.insert(KDCRMFields::accountId(),
                         LeadAccessorPair(&SugarLead::accountId, &SugarLead::setAccountId, QString()));
        accessors.insert(KDCRMFields::opportunityId(),
                         LeadAccessorPair(&SugarLead::opportunityId, &SugarLead::setOpportunityId, QString()));
        accessors.insert(KDCRMFields::opportunityName(),
                         LeadAccessorPair(&SugarLead::opportunityName, &SugarLead::setOpportunityName,
                                          i18nc("@item:intable", "Opportunity")));
        accessors.insert(KDCRMFields::opportunityAmount(),
                         LeadAccessorPair(&SugarLead::opportunityAmount, &SugarLead::setOpportunityAmount,
                                          i18nc("@item:intable", "Opportunity Amount")));
        accessors.insert(KDCRMFields::campaignId(),
                         LeadAccessorPair(&SugarLead::campaignId, &SugarLead::setCampaignId, QString()));
        accessors.insert(KDCRMFields::campaignName(),
                         LeadAccessorPair(&SugarLead::campaignName, &SugarLead::setCampaignName,
                                          i18nc("@item:intable", "Campaign")));
        accessors.insert(KDCRMFields::cAcceptStatusFields(),
                         LeadAccessorPair(&SugarLead::cAcceptStatusFields, &SugarLead::setCAcceptStatusFields, QString()));
        accessors.insert(KDCRMFields::mAcceptStatusFields(),
                         LeadAccessorPair(&SugarLead::mAcceptStatusFields, &SugarLead::setMAcceptStatusFields, QString()));
        accessors.insert(KDCRMFields::birthdate(),
                         LeadAccessorPair(&SugarLead::birthdate, &SugarLead::setBirthdate,
                                          i18nc("@item:intable", "Birthdate")));
        accessors.insert(KDCRMFields::portalName(),
                         LeadAccessorPair(&SugarLead::portalName, &SugarLead::setPortalName,
                                          i18nc("@item:intable", "Portal")));
        accessors.insert(KDCRMFields::portalApp(),
                         LeadAccessorPair(&SugarLead::portalApp, &SugarLead::setPortalApp,
                                          i18nc("@item:intable", "Portal Application")));
    }

    return accessors;
}
