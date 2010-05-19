#ifndef SUGARACCOUNT_H
#define SUGARACCOUNT_H

#include <QtCore/QSharedDataPointer>


/**
  @short sugar account entry

  This class represents an account entry in sugarcrm.

  The data of this class is implicitly shared. You can pass this class by value.
 */
class SugarAccount
{

  public:

    /**
      Construct an empty SugarAccount entry.
     */
    SugarAccount();

    /**
      Destroys the SugarAccount entry.
     */
    ~SugarAccount();

    /**
      Copy constructor.
     */
    SugarAccount( const SugarAccount & );

    /**
      Assignment operator.

      @return a reference to @c this
    */
    SugarAccount &operator=( const SugarAccount & );

    /**
      Equality operator.

      @return @c true if @c this and the given SugarAccount are equal,
              otherwise @c false
    */
    bool operator==( const SugarAccount & ) const;

    /**
      Not-equal operator.

      @return @c true if @c this and the given SugarAccount are not equal,
              otherwise @c false
    */
    bool operator!=( const SugarAccount & ) const;

    /**
      Return, if the SugarAccount entry is empty.
     */
    bool isEmpty() const;

    /**
      Clears all entries of the SugarAccount.
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
      Set website.
     */
    void setWebsite( const QString &value );
    /**
      Return Website.
     */
    QString website() const;

    /**
      Set Tycker Symbol.
     */
    void setTyckerSymbol( const QString &value );
    /**
      Return Tycker Symbol.
     */
    QString tyckerSymbol() const;

    /**
      Set Member of.
     */
    void setMemberOf( const QString &value );
    /**
      Return Member of.
     */
    QString memberOf() const;

    /**
      Set Ownership.
     */
    void setOwnership( const QString &value );
    /**
      Return Ownership.
     */
    QString ownership() const;

    /**
      Set Industry.
     */
    void setIndustry( const QString &value );
    /**
      Return Industry.
     */
    QString industry() const;

    /**
      Set Type.
     */
    void setType( const QString &value );
    /**
      Return type.
     */
    QString type() const;

    /**
      Set Campaign.
     */
    void setCampaign( const QString &value );
    /**
      Return Campaign.
     */
    QString campaign() const;

    /**
      Set Assigned to.
     */
    void setAssignedTo( const QString &value );
    /**
      Return assigned to.
     */
    QString assignedTo() const;

    /**
      Set Office Phone.
     */
    void setOfficePhone( const QString &value );
    /**
      Return Office Phone.
     */
    QString officePhone() const;

    /**
      Set Fax Phone.
     */
    void setFax( const QString &value );
    /**
      Return Fax Phone.
     */
    QString fax() const;

    /**
      Set Other Phone.
     */
    void setOtherPhone( const QString &value );
    /**
      Return Other Phone.
     */
    QString otherPhone() const;

    /**
      Set Employees.
     */
    void setEmployees( const QString &value );
    /**
      Return Employees.
     */
    QString employees() const;

    /**
      Set Rating.
     */
    void setRating( const QString &value );
    /**
      Return Rating.
     */
    QString rating() const;

    /**
      Set SIC Code.
     */
    void setSicCode( const QString &value );
    /**
      Return SIC Code.
     */
    QString sicCode() const;

    /**
      Set Annual Revenue.
     */
    void setAnnualRevenue( const QString &value );
    /**
      Return AnnualRevenue.
     */
    QString annualRevenue() const;

    /**
      Set Billing Address.
     */
    void setBillingAddress( const QString &value );
    /**
      Return Billing Address.
     */
    QString billingAddress() const;

    /**
      Set City.
     */
    void setCity( const QString &value );
    /**
      Return City.
     */
    QString city() const;

    /**
      Set State.
     */
    void setState( const QString &value );
    /**
      Return State.
     */
    QString state() const;

    /**
      Set Postal Code.
     */
    void setPostalCode( const QString &value );
    /**
      Return Postal Code.
     */
    QString postalCode() const;

    /**
      Set Country.
     */
    void setCountry( const QString &value );
    /**
      Return Country.
     */
    QString country() const;

    /**
      Set Shipping Address.
     */
    void setShippingAddress( const QString &value );
    /**
      Return Shipping Address.
     */
    QString shippingAddress() const;

    /**
      Set Shipping City.
     */
    void setShippingCity( const QString &value );
    /**
      Return Shipping City.
     */
    QString shippingCity() const;

    /**
      Set Shipping State.
     */
    void setShippingState( const QString &value );
    /**
      Return Shipping State.
     */
    QString shippingState() const;

    /**
      Set Shipping Postal Code.
     */
    void setShippingPostalCode( const QString &value );
    /**
      Return Shipping Postal Code.
     */
    QString shippingPostalCode() const;

    /**
      Set Shipping Country.
     */
    void setShippingCountry( const QString &value );
    /**
      Return Shipping Country.
     */
    QString shippingCountry() const;

    /**
      Set Primary Email.
     */
    void setPrimaryEmail( const QString &value );
    /**
      Return Primary Email.
     */
    QString primaryEmail() const;

    /**
      Set Opted Out Email.
     */
    void setOptedOutEmail( const QString &value );
    /**
      Return Opted Out Email.
     */
    QString optedOutEmail() const;

    /**
      Set Invalid Email.
     */
    void setInvalidEmail( const QString &value );
    /**
      Return Invalid Email.
     */
    QString invalidEmail() const;

    /**
      Set Description.
     */
    void setDescription( const QString &value );
    /**
      Return Description.
     */
    QString description() const;

    /**
       Return the Mime type
     */
    static QString mimeType();

  private:
    class Private;
    QSharedDataPointer<Private> d;
};

#endif /* SUGARACCOUNT_H */

