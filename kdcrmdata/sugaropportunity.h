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

#ifndef SUGAROPPORTUNITY_H
#define SUGAROPPORTUNITY_H

#include "kdcrmdata_export.h"

#include <QMetaType>
#include <QSharedDataPointer>

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
    SugarOpportunity(const SugarOpportunity &);

    /**
      Assignment operator.

      return a reference to this
    */
    SugarOpportunity &operator=(const SugarOpportunity &);

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
      Return Modification date.
     */
    QDateTime dateModified() const;
    void setDateModified(const QDateTime &date);

    void setDateModifiedRaw(const QString &name);
    QString dateModifiedRaw() const;


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
      Return the wanted number of paragraphs in the description (or the full
      description if the number of paragraphs is less than the wanted number).
     */
    QString limitedDescription(int wantedParagraphs) const;

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
      Set Opportunity type.
     */
    void setOpportunityType(const QString &value);
    /**
      Return Opportunity type.
     */
    QString opportunityType() const;

    /**
      Return Opportunity size.
     */
    QString opportunitySize() const;
    /**
      Return Opportunity priority.
     */
    QString opportunityPriority() const;

    /**
      Set the Account Name; used temporarily during listing.
      Don't use from the GUI, use the account ID instead.
     */
    void setTempAccountName(const QString &value);
    /**
      Return the temporary Account Name.
      Don't use from the GUI, use the account ID instead.
    */
    QString tempAccountName() const;

    /**
      Set the Account Id.
     */
    void setAccountId(const QString &value);
    /**
      Return the Account Name.
    */
    QString accountId() const;

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

    /**
      Set Lead Source.
     */
    void setLeadSource(const QString &value);
    /**
      Return Lead Source.
     */
    QString leadSource() const;

    /**
      Set Amount.
     */
    void setAmount(const QString &value);
    /**
      Return Amount.
     */
    QString amount() const;

    /**
      Set Amount US dollar.
     */
    void setAmountUsDollar(const QString &value);
    /**
      Return Amount US dollar.
     */
    QString amountUsDollar() const;

    /**
       Set the currency Id.
     */
    void setCurrencyId(const QString &value);
    /**
      Return the currency Id.
     */
    QString currencyId() const;

    /**
      Set the Currency Name.
     */
    void setCurrencyName(const QString &value);
    /**
      Return the Currency Name.
     */
    QString currencyName() const;

    /**
       Set the Currency Symbol.
     */
    void setCurrencySymbol(const QString &value);
    /**
       Return the Currency Symbol.
     */
    QString currencySymbol() const;

    /**
      Set the Closed Date.
     */
    void setDateClosed(const QString &value);
    /**
      Return the Closed Date.
     */
    QString dateClosed() const;

    /**
      Set the Next Step.
     */
    void setNextStep(const QString &value);
    /**
      Return the Next Step.
     */
    QString nextStep() const;

    /**
      Set the Sales Stage.
     */
    void setSalesStage(const QString &value);
    /**
      Return the Sales Stage.
     */
    QString salesStage() const;

    /**
      Set the Probability.
     */
    void setProbability(const QString &value);
    /**
      Return the Probability.
     */
    QString probability() const;

    /**
      Set the next call date.
     */
    void setNextCallDate(const QDate &date);
    /**
      Return next call date.
     */
    QDate nextCallDate() const;

    // Unknown fields (missing in this code) and custom fields (added by Sugar configuration)
    void setCustomField(const QString &name, const QString &value);
    QMap<QString, QString> customFields() const;

    /**
      Convenience: Set the data for this opportunity
     */
    void setData(const QMap<QString, QString> &data);

    /**
      Convenience: Retrieve the data for this opportunity
     */
    QMap<QString, QString> data();

    /**
       Return the Mime type
     */
    static QString mimeType();

    using valueGetter = QString (SugarOpportunity::*)() const;
    using valueSetter = void (SugarOpportunity::*)(const QString &);

    class OpportunityAccessorPair
    {
    public:
        OpportunityAccessorPair(valueGetter get, valueSetter set, const QString &name)
            : getter(get), setter(set), diffName(name)
        {}

    public:
        valueGetter getter;
        valueSetter setter;
        QString diffName;
    };

    typedef QHash<QString, OpportunityAccessorPair> AccessorHash;

    static AccessorHash accessorHash();

private:
    class Private;
    QSharedDataPointer<Private> d;
};

Q_DECLARE_METATYPE(SugarOpportunity)

#endif /* SUGAROPPORTUNITY_H */
