#ifndef SUGAROPPORTUNITY_H
#define SUGAROPPORTUNITY_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedDataPointer>

#include "kdcrmdata_export.h"

/**
  @short sugar opportunity entry

  This class represents an opportunity entry in sugarcrm.

  The data of this class is implicitly shared. You can pass this class by value.
 */
class KDCRMDATA_EXPORT SugarOpportunity
{

  public:

    /**
      Construct an empty SugarOpportunity entry.
     */
    SugarOpportunity();

    /**
      Destroys the SugarOpportunity entry.
     */
    ~SugarOpportunity();

    /**
      Copy constructor.
     */
    SugarOpportunity( const SugarOpportunity & );

    /**
      Assignment operator.

      return a reference to this
    */
    SugarOpportunity &operator=( const SugarOpportunity & );

    /**
      Equality operator.

      return true if this and the given SugarOpportunity are equal,
              otherwise false
    */
    bool operator==( const SugarOpportunity & ) const;

    /**
      Not-equal operator.

      return true if this and the given SugarOpportunity are not equal,
              otherwise false
    */
    bool operator!=( const SugarOpportunity & ) const;

    /**
      Return, if the SugarOpportunity entry is empty.
     */
    bool isEmpty() const;

    /**
      Clears all entries of the SugarOpportunity.
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
      Set name.
     */
    void setName( const QString &name );
    /**
      Return name.
     */
    QString name() const;

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
      Set Opportunity type.
     */
    void setOpportunityType( const QString &value );
    /**
      Return Opportunity type.
     */
    QString opportunityType() const;

    /**
      Set the Account Name.
     */
    void setAccountName( const QString &value );
    /**
      Return the Account Name.
    */
    QString accountName() const;

    /**
      Set the Account Id.
     */
    void setAccountId( const QString &value );
    /**
      Return the Account Name.
    */
    QString accountId() const;

    /**
      Set Campaign Id.
     */
    void setCampaignId( const QString &value );
    /**
      Return Campaign Id.
     */
    QString campaignId() const;

    /**
      Set Campaign Name.
     */
    void setCampaignName( const QString &value );
    /**
      Return Campaign Name.
     */
    QString campaignName() const;

    /**
      Set Lead Source.
     */
    void setLeadSource( const QString &value );
    /**
      Return Lead Source.
     */
    QString leadSource() const;

    /**
      Set Amount.
     */
    void setAmount( const QString &value );
    /**
      Return Amount.
     */
    QString amount() const;

    /**
      Set Amount US dollar.
     */
    void setAmountUsDollar( const QString &value );
    /**
      Return Amount US dollar.
     */
    QString amountUsDollar() const;

    /**
       Set the currency Id.
     */
    void setCurrencyId( const QString &value );
    /**
      Return the currency Id.
     */
    QString currencyId() const;

    /**
      Set the Currency Name.
     */
    void setCurrencyName( const QString &value );
    /**
      Return the Currency Name.
     */
    QString currencyName() const;

    /**
       Set the Currency Symbol.
     */
    void setCurrencySymbol( const QString &value );
    /**
       Return the Currency Symbol.
     */
    QString currencySymbol() const;

    /**
      Set the Closed Date.
     */
    void setDateClosed( const QString &value );
    /**
      Return the Closed Date.
     */
    QString dateClosed() const;

    /**
      Set the Next Step.
     */
    void setNextStep( const QString &value );
    /**
      Return the Next Step.
     */
    QString nextStep() const;

    /**
      Set the Sales Stage.
     */
    void setSalesStage( const QString &value );
    /**
      Return the Sales Stage.
     */
    QString salesStage() const;

    /**
      Set the Probability.
     */
    void setProbability( const QString &value );
    /**
      Return the Probability.
     */
    QString probability() const;

    /**
      Convenience: Set the data for this opportunity
     */
    void setData( QMap<QString, QString> data );

    /**
      Convenience: Retrieve the data for this opportunity
     */
    QMap<QString, QString> data();


    /**
       Return the Mime type
     */
    static QString mimeType();

  private:
    class Private;
    QSharedDataPointer<Private> d;
};

Q_DECLARE_METATYPE( SugarOpportunity )

#endif /* SUGAROPPORTUNITY_H */

