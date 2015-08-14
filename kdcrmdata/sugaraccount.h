/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef SUGARACCOUNT_H
#define SUGARACCOUNT_H

#include "kdcrmdata_export.h"

#include <QMetaType>
#include <QSharedDataPointer>

/**
  @short sugar account entry

  This class represents an account entry in sugarcrm.

  The data of this class is implicitly shared. You can pass this class by value.
 */
class KDCRMDATA_EXPORT SugarAccount
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
    SugarAccount(const SugarAccount &);

    /**
      Assignment operator.

      @return a reference to @c this
    */
    SugarAccount &operator=(const SugarAccount &);

    /**
      Decide whether two SugarAccount instance refer to the same account
      (for duplicate handling). Not all fields might have the same value
      (so this is not an operator==).

      @return @c true if @c this and the given SugarAccount should be a single account,
              otherwise @c false
    */
    bool isSameAccount(const SugarAccount &other) const;

    /**
      Returns a key representing this account uniquely
    */
    QString key() const;


    /**
      Returns the canonical name of the account.
      E.g. "KDAB, Inc." has a canonical name of "KDAB".
    */
    QString cleanAccountName() const;

private:
    /**
      Equality operator.

      @return @c true if @c this and the given SugarAccount are equal,
              otherwise @c false
    */
    bool operator==(const SugarAccount &) const;

    /**
      Not-equal operator.

      @return @c true if @c this and the given SugarAccount are not equal,
              otherwise @c false
    */
    bool operator!=(const SugarAccount &) const;
public:

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
    void setId(const QString &id);
    /**
      Return unique identifier.
     */
    QString id() const;

    /**
      Set name.
     */
    void setName(const QString &name);
    /**
      Return name.
     */
    QString name() const;

    /**
      Set Creation date.
     */
    void setDateEntered(const QString &name);
    /**
      Return Creation date.
     */
    QString dateEntered() const;

    /**
      Set Modification date.
     */
    void setDateModified(const QString &name);
    /**
      Return Modification date.
     */
    QString dateModified() const;

    /**
      Set Modified User id.
     */
    void setModifiedUserId(const QString &name);
    /**
      Return Modified User id.
     */
    QString modifiedUserId() const;

    /**
      Set Modified By name.
     */
    void setModifiedByName(const QString &name);
    /**
      Return Modified By name.
     */
    QString modifiedByName() const;

    /**
      Set Created By id.
     */
    void setCreatedBy(const QString &name);
    /**
      Return created by id.
     */
    QString createdBy() const;

    /**
      Set Created By Name.
     */
    void setCreatedByName(const QString &name);
    /**
      Return created by Name.
     */
    QString createdByName() const;

    /**
      Set Description.
    */
    void setDescription(const QString &value);
    /**
      Return Description.
     */
    QString description() const;

    /**
      Set Deleted.
    */
    void setDeleted(const QString &value);
    /**
      Return Deleted.
     */
    QString deleted() const;

    /**
      Set Assigned User Id.
    */
    void setAssignedUserId(const QString &value);
    /**
      Return Assigned User Id.
     */
    QString assignedUserId() const;

    /**
      Set Assigned User Name.
     */
    void setAssignedUserName(const QString &value);
    /**
      Return assigned User Name.
     */
    QString assignedUserName() const;

    /**
      Set Account type.
     */
    void setAccountType(const QString &value);
    /**
      Return Account type.
     */
    QString accountType() const;

    /**
      Set Industry.
     */
    void setIndustry(const QString &value);
    /**
      Return Industry.
     */
    QString industry() const;

    /**
      Set Annual Revenue.
     */
    void setAnnualRevenue(const QString &value);
    /**
      Return AnnualRevenue.
     */
    QString annualRevenue() const;

    /**
      Set Phone Fax.
     */
    void setPhoneFax(const QString &value);
    /**
      Return Phone Fax.
     */
    QString phoneFax() const;

    /**
      Set Billing Address Street.
     */
    void setBillingAddressStreet(const QString &value);
    /**
      Return Billing Address Street.
     */
    QString billingAddressStreet() const;

    /**
       Set Billing Address City.
     */
    void setBillingAddressCity(const QString &value);
    /**
      Return Billing Address City.
     */
    QString billingAddressCity() const;

    /**
      Set Billing Address State.
     */
    void setBillingAddressState(const QString &value);
    /**
      Return Billing Address State.
     */
    QString billingAddressState() const;

    /**
       Set Billing Address Postal Code.
     */
    void setBillingAddressPostalcode(const QString &value);
    /**
       Return Billing Address Postal Code.
     */
    QString billingAddressPostalcode() const;

    /**
      Set Billing Address Country.
     */
    void setBillingAddressCountry(const QString &value);
    /**
      Return Billing Address Country.
     */
    QString billingAddressCountry() const;

    /**
      Set Rating.
     */
    void setRating(const QString &value);
    /**
      Return Rating.
     */
    QString rating() const;

    /**
      Set Office Phone.
     */
    void setPhoneOffice(const QString &value);
    /**
      Return Office Phone.
     */
    QString phoneOffice() const;

    /**
      Set Other Phone.
     */
    void setPhoneAlternate(const QString &value);
    /**
      Return Other Phone.
     */
    QString phoneAlternate() const;

    /**
      Set website.
     */
    void setWebsite(const QString &value);
    /**
      Return Website.
     */
    QString website() const;

    /**
      Set Ownership.
     */
    void setOwnership(const QString &value);
    /**
      Return Ownership.
     */
    QString ownership() const;

    /**
      Set Employees.
     */
    void setEmployees(const QString &value);
    /**
      Return Employees.
     */
    QString employees() const;

    /**
      Set Ticker Symbol.
     */
    void setTickerSymbol(const QString &value);
    /**
      Return Ticker Symbol.
     */
    QString tickerSymbol() const;

    /**
      Set Shipping Address street.
     */
    void setShippingAddressStreet(const QString &value);
    /**
      Return Shipping Address street.
     */
    QString shippingAddressStreet() const;

    /**
      Set Shipping Address City.
     */
    void setShippingAddressCity(const QString &value);
    /**
      Return Shipping Address City.
     */
    QString shippingAddressCity() const;

    /**
      Set Shipping Address State.
     */
    void setShippingAddressState(const QString &value);
    /**
      Return Shipping Address State.
     */
    QString shippingAddressState() const;

    /**
      Set Shipping Address Postal Code.
     */
    void setShippingAddressPostalcode(const QString &value);
    /**
      Return Shipping Address Postal Code.
     */
    QString shippingAddressPostalcode() const;

    /**
      Set Shipping Address Country.
     */
    void setShippingAddressCountry(const QString &value);
    /**
      Return Shipping Address Country.
     */
    QString shippingAddressCountry() const;

    /**
      Set Primary Email.
     */
    void setEmail1(const QString &value);
    /**
      Return Primary Email.
     */
    QString email1() const;

    /**
      Set Parent Id .
     */
    void setParentId(const QString &value);
    /**
      Return Parent Id.
     */
    QString parentId() const;

    /**
      Set Parent Name .
     */
    void setParentName(const QString &value);
    /**
      Return Parent Name.
     */
    QString parentName() const;

    /**
      Set SIC Code.
     */
    void setSicCode(const QString &value);
    /**
      Return SIC Code.
     */
    QString sicCode() const;

    /**
      Set Campaign Id.
     */
    void setCampaignId(const QString &value);
    /**
      Return Campaign Id.
     */
    QString campaignId() const;

    /**
      Set Campaign Name.
     */
    void setCampaignName(const QString &value);
    /**
      Return Campaign Name.
     */
    QString campaignName() const;

    // Unknown fields (missing in this code) and custom fields (added by Sugar configuration)
    void setCustomField(const QString &name, const QString &value);
    QMap<QString, QString> customFields() const;

    /**
      Convenience: Set the data for this account
     */
    void setData(const QMap<QString, QString> &data);

    /**
      Convenience: Retrieve the data for this account
     */
    QMap<QString, QString> data() const;

    /**
       Return the Mime type
     */
    static QString mimeType();

    typedef QString(SugarAccount::*valueGetter)() const;
    typedef void (SugarAccount::*valueSetter)(const QString &);

    class AccountAccessorPair
    {
    public:
        AccountAccessorPair(valueGetter get, valueSetter set, const QString &name)
            : getter(get), setter(set), diffName(name)
        {}

    public:
        valueGetter getter;
        valueSetter setter;
        QString diffName;
    };

    typedef QHash<QString, AccountAccessorPair> AccessorHash;

    static AccessorHash accessorHash();

private:
    class Private;
    QSharedDataPointer<Private> d;
};

Q_DECLARE_METATYPE(SugarAccount)

#endif /* SUGARACCOUNT_H */
