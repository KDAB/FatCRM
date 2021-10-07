#include "sugarcontactwrapper.h"

SugarContactWrapper::SugarContactWrapper(const KContacts::Addressee &addressee)
    : m_addressee(addressee)
{
}

QString SugarContactWrapper::id() const
{
    return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-ContactId"));
}

QString SugarContactWrapper::salutation() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-Salutation")); }
QString SugarContactWrapper::accountId() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AccountId")); }
QString SugarContactWrapper::assistant() const { return m_addressee.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("X-AssistantsName")); }
QString SugarContactWrapper::phoneAssistant() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AssistantsPhone")); }
QString SugarContactWrapper::leadSource() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-LeadSourceName")); }
QString SugarContactWrapper::campaign() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-CampaignName")); }
QString SugarContactWrapper::campaignId() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-CampaignId")); }
QString SugarContactWrapper::assignedUserId() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AssignedUserId")); }
QString SugarContactWrapper::assignedUserName() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AssignedUserName")); }
QString SugarContactWrapper::reportsToId() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-ReportsToUserId")); }
QString SugarContactWrapper::reportsTo() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-ReportsToUserName")); }
QString SugarContactWrapper::doNotCall() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-DoNotCall")); }
QString SugarContactWrapper::invalidEmail() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-InvalidEmail")); }
QString SugarContactWrapper::modifiedByName() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-ModifiedByName")); }
QString SugarContactWrapper::dateCreated() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-DateCreated")); }
QString SugarContactWrapper::dateModified() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-DateModified")); }
QString SugarContactWrapper::dateEntered() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-DateCreated")); }
QString SugarContactWrapper::createdByName() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-CreatedByName")); }
QString SugarContactWrapper::modifiedUserId() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-ModifiedUserId")); }
QString SugarContactWrapper::opportunityRoleFields() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-OpportunityRoleFields")); }
QString SugarContactWrapper::cAcceptStatusFields() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-CacceptStatusFields")); }
QString SugarContactWrapper::mAcceptStatusFields() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-MacceptStatusFields")); }
QString SugarContactWrapper::deleted() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-Deleted")); }
QString SugarContactWrapper::createdBy() const { return m_addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-CreatedById")); }
