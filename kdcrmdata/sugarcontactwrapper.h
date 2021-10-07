#ifndef SUGARCONTACTWRAPPER_H
#define SUGARCONTACTWRAPPER_H

#include "kdcrmdata_export.h"
#include <KContacts/Addressee>

class KDCRMDATA_EXPORT SugarContactWrapper
{
public:
    explicit SugarContactWrapper(const KContacts::Addressee &addressee);

    QString id() const;
    QString accountId() const;

    QString salutation() const;
    QString assistant() const;
    QString phoneAssistant() const;
    QString leadSource() const;
    QString campaign() const;
    QString campaignId() const;
    QString assignedUserId() const;
    QString assignedUserName() const;
    QString reportsToId() const;
    QString reportsTo() const;
    QString doNotCall() const;
    QString invalidEmail() const;
    QString modifiedByName() const;
    QString dateCreated() const;
    QString dateModified() const;
    QString dateEntered() const;
    QString createdByName() const;
    QString modifiedUserId() const;
    QString opportunityRoleFields() const;
    QString cAcceptStatusFields() const;
    QString mAcceptStatusFields() const;
    QString deleted() const;
    QString createdBy() const;

private:
    KContacts::Addressee m_addressee;
};

#endif // SUGARCONTACTWRAPPER_H
