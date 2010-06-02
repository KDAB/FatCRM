#ifndef SUGARLEAD_H
#define SUGARLEAD_H

#include <QtCore/QSharedDataPointer>

#include "kdcrmdata_export.h"

/**
 Short: sugar lead entry

  This class represents an lead entry in sugarcrm.

  The data of this class is implicitly shared. You can pass this class by value.
 */
class KDCRMDATA_EXPORT SugarLead
{

  public:

    /**
      Construct an empty SugarLead entry.
     */
    SugarLead();

    /**
      Destroys the SugarLead entry.
     */
    ~SugarLead();

    /**
      Copy constructor.
     */
    SugarLead( const SugarLead & );

    /**
      Assignment operator.

      return a reference to this
    */
    SugarLead &operator=( const SugarLead & );

    /**
      Equality operator.

      return true if this and the given SugarLead are equal,
              otherwise false
    */
    bool operator==( const SugarLead & ) const;

    /**
      Not-equal operator.

      return true if  this and the given SugarLead are not equal,
              otherwise false
    */
    bool operator!=( const SugarLead & ) const;

    /**
      Return, if the SugarLead entry is empty.
     */
    bool isEmpty() const;

    /**
      Clears all entries of the SugarLead.
    */
    void clear();

    /**
      Set unique identifier.
     */
    void setId( const QString &id );
    /**
      Return unique identifier.
     */
    QString id() const;

    /**
      Set Creation date.
     */
    void setDateEntered( const QString &name );
    /**
      Return Creation date.
     */
    QString dateEntered() const;

    /**
      Set Modification date.
     */
    void setDateModified( const QString &name );
    /**
      Return Modification date.
     */
    QString dateModified() const;

    /**
      Set Modified User id.
     */
    void setModifiedUserId( const QString &name );
    /**
      Return Modified User id.
     */
    QString modifiedUserId() const;

    /**
      Set Modified By name.
     */
    void setModifiedByName( const QString &name );
    /**
      Return Modified By name.
     */
    QString modifiedByName() const;

    /**
      Set Created By id.
     */
    void setCreatedBy( const QString &name );
    /**
      Return created by id.
     */
    QString createdBy() const;

    /**
      Set Created By Name.
     */
    void setCreatedByName( const QString &name );
    /**
      Return created by Name.
     */
    QString createdByName() const;

    /**
      Set Description.
    */
    void setDescription( const QString &value );
    /**
      Return Description.
     */
    QString description() const;

    /**
      Set Deleted.
    */
    void setDeleted( const QString &value );
    /**
      Return Deleted.
     */
    QString deleted() const;

    /**
      Set Assigned User Id.
    */
    void setAssignedUserId( const QString &value );
    /**
      Return Assigned User Id.
     */
    QString assignedUserId() const;

    /**
      Set Assigned User Name.
     */
    void setAssignedUserName( const QString &value );
    /**
      Return assigned User Name.
     */
    QString assignedUserName() const;

    /**
      Set the Salutation type (Mr Ms etc...).
     */
    void setSalutation( const QString &value );
    /**
      Return salutation.
     */
    QString salutation() const;

    /**
      Set the first name.
     */
    void setFirstName( const QString &value );
    /**
      Return the first name.
     */
    QString firstName() const;

    /**
      Set the last name.
     */
    void setLastName( const QString &value );
    /**
      Return the last name.
     */
    QString lastName() const;

    /**
      Set Job title.
     */
    void setTitle( const QString &value );
    /**
      Return job title.
     */
    QString title() const;

    /**
      Set Job Department.
     */
    void setDepartment( const QString &value );
    /**
      Return Job department.
     */
    QString department() const;

    /**
       Set Do not call.
     */
    void setDoNotCall( const QString &value );
    /**
      Return Do not call.
     */
    QString doNotCall() const;

    /**
      Set Phone Home.
     */
    void setPhoneHome( const QString &value );
    /**
      Return Phone Home.
     */
    QString phoneHome() const;

    /**
       Set Phone Mobile.
     */
    void setPhoneMobile( const QString &value );
    /**
       Return Phone Mobile.
     */
    QString phoneMobile() const;

    /**
      Set Phone Work.
     */
    void setPhoneWork( const QString &value );
    /**
      Return Phone Work.
     */
    QString phoneWork() const;

    /**
      Set Phone Other.
     */
    void setPhoneOther( const QString &value );
    /**
      Return Phone Other.
     */
    QString phoneOther() const;

    /**
      Set Phone Fax.
     */
    void setPhoneFax( const QString &value );
    /**
      Return Phone Fax.
     */
    QString phoneFax() const;

    /**
      Set Email1.
     */
    void setEmail1( const QString &value );
    /**
      Return Email1.
     */
    QString email1() const;

    /**
      Set Email2.
     */
    void setEmail2( const QString &value );
    /**
      Return Email2.
     */
    QString email2() const;

    /**
      Set Primary Address Street.
     */
    void setPrimaryAddressStreet( const QString &value );
    /**
      Return Primary Address Street.
     */
    QString primaryAddressStreet() const;

    /**
      Set Primary Address City.
     */
    void setPrimaryAddressCity( const QString &value );
    /**
      Return Primary Address City.
     */
    QString primaryAddressCity() const;

    /**
      Set Primary Address State.
     */
    void setPrimaryAddressState( const QString &value );
    /**
      Return Primary Address State.
     */
    QString primaryAddressState() const;

    /**
      Set Primary Address Postalcode.
     */
    void setPrimaryAddressPostalcode( const QString &value );
    /**
      Return Primary Address Postal code.
     */
    QString primaryAddressPostalcode() const;

    /**
      Set Primary Address Country.
     */
    void setPrimaryAddressCountry( const QString &value );
    /**
      Return Primary Address Country.
     */
    QString primaryAddressCountry() const;

    /**
      Set Alt Address Street.
     */
    void setAltAddressStreet( const QString &value );
    /**
      Return Alt Address Street.
     */
    QString altAddressStreet() const;

    /**
      Set Alt Address City.
     */
    void setAltAddressCity( const QString &value );
    /**
      Return Alt Address City.
     */
    QString altAddressCity() const;

    /**
      Set Alt Address State.
     */
    void setAltAddressState( const QString &value );
    /**
      Return Alt Address State.
     */
    QString altAddressState() const;

    /**
      Set Alt Address Postalcode.
     */
    void setAltAddressPostalcode( const QString &value );
    /**
      Return Alt Address Postal code.
     */
    QString altAddressPostalcode() const;

    /**
      Set Alt Address Country.
     */
    void setAltAddressCountry( const QString &value );
    /**
      Return Alt Address Country.
     */
    QString altAddressCountry() const;

    /**
      Set Assistant name.
     */
    void setAssistant( const QString &value );
    /**
      Return Assistant name.
     */
    QString assistant() const;

    /**
      Set Assistant Phone.
     */
    void setAssistantPhone( const QString &value );
    /**
      Return Assistant Phone.
     */
    QString assistantPhone() const;

    /**
      Set Converted.
     */
    void setConverted( const QString &value );
    /**
      Return Converted.
     */
    QString converted() const;

    /**
      Set Referred by.
     */
    void setReferedBy( const QString &value );
    /**
      Return referred by.
     */
    QString referedBy() const;

    /**
      Set Lead Source.
     */
    void setLeadSource( const QString &value );
    /**
      Return Lead Source.
     */
    QString leadSource() const;

    /**
      Set Lead Source Description.
     */
    void setLeadSourceDescription( const QString &value );
    /**
      Return Lead Source Description.
     */
    QString leadSourceDescription() const;

    /**
      Set Status.
     */
    void setStatus( const QString &value );
    /**
      Return Status.
     */
    QString status() const;

    /**
      Set Status Description.
     */
    void setStatusDescription( const QString &value );
    /**
      Return Status Description.
     */
    QString statusDescription() const;

    /**
      Set reports to Id.
     */
    void setReportsToId( const QString &value );
    /**
      Return reports to Id.
     */
    QString reportsToId() const;

    /**
      Set report to name.
     */
    void setReportToName( const QString &value );
    /**
      Return report to name.
     */
    QString reportToName() const;

    /**
      Set account name.
     */
    void setAccountName( const QString &value );
    /**
      Return account name.
     */
    QString accountName() const;

    /**
      Set Account Description.
     */
    void setAccountDescription( const QString &value );
    /**
      Return Account Description.
     */
    QString accountDescription() const;

    /**
      Set Contact Id.
     */
    void setContactId( const QString &value );
    /**
      Return Contact Id.
     */
    QString contactId() const;

    /**
      Set Account Id.
     */
    void setAccountId( const QString &value );
    /**
      Return Account Id.
     */
    QString accountId() const;

    /**
      Set Opportunity Id.
     */
    void setOpportunityId( const QString &value );
    /**
      Return Opportunity Id.
     */
    QString opportunityId() const;

    /**
      Set Opportunity Name.
     */
    void setOpportunityName( const QString &value );
    /**
      Return Opportunity Name.
     */
    QString opportunityName() const;

    /**
      Set Opportunity Amount.
     */
    void setOpportunityAmount( const QString &value );
    /**
      Return Opportunity Amount.
     */
    QString opportunityAmount() const;

    /**
      Set Campaign Id.
     */
    void setCampaignId( const QString &value );
    /**
      Return Campaing Id.
     */
    QString campaignId() const;

    /**
      Set Campaign Name.
     */
    void setCampaignName( const QString &value );
    /**
      Return Campaing Name.
     */
    QString campaignName() const;

    /**
      Set C Accept Status Fields.
     */
    void setCAcceptStatusFields( const QString &value );
    /**
      Return C Accept Status Fields.
     */
    QString cAcceptStatusFields() const;

     /**
      Set M Accept Status Fields.
     */
    void setMAcceptStatusFields( const QString &value );
    /**
      Return M Accept Status Fields.
     */
    QString mAcceptStatusFields() const;

     /**
      Set Birth Date.
     */
    void setBirthdate( const QString &value );
    /**
      Return Birth Date.
     */
    QString birthdate() const;

    /**
      Set Portal Name.
     */
    void setPortalName( const QString &value );
    /**
      Return Portal Name.
     */
    QString portalName() const;

    /**
      Set Portal App.
     */
    void setPortalApp( const QString &value );
    /**
      Return Portal App.
     */
    QString portalApp() const;

    /**
       Return the Mime type
     */
    static QString mimeType();

  private:
    class Private;
    QSharedDataPointer<Private> d;
};


#endif /* SUGARLEAD_H */

