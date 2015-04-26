#ifndef SUGARCAMPAIGN_H
#define SUGARCAMPAIGN_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedDataPointer>

#include "kdcrmdata_export.h"

/**
 Short: sugar campaign entry

  This class represents an campaign entry in sugarcrm.

  The data of this class is implicitly shared. You can pass this class by value.
 */
class KDCRMDATA_EXPORT SugarCampaign
{

public:

    /**
      Construct an empty SugarCampaign entry.
     */
    SugarCampaign();

    /**
      Destroys the SugarCampaign entry.
     */
    ~SugarCampaign();

    /**
      Copy constructor.
     */
    SugarCampaign(const SugarCampaign &);

    /**
      Assignment operator.

      return a reference to this
    */
    SugarCampaign &operator=(const SugarCampaign &);

    /**
      Equality operator.

      return true if this and the given SugarCampaign are equal,
              otherwise false
    */
    bool operator==(const SugarCampaign &) const;

    /**
      Not-equal operator.

      return true if  this and the given SugarCampaign are not equal,
              otherwise false
    */
    bool operator!=(const SugarCampaign &) const;

    /**
      Return, if the SugarCampaign entry is empty.
     */
    bool isEmpty() const;

    /**
      Clears all entries of the SugarCampaign.
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
      Set the Campaign Name.
     */
    void setName(const QString &id);
    /**
      Return the Campaign Name.
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
      Set the tracker key.
     */
    void setTrackerKey(const QString &value);
    /**
      Return tracker key.
     */
    QString trackerKey() const;

    /**
      Set tracker count.
     */
    void setTrackerCount(const QString &value);
    /**
      Return tracker count.
     */
    QString trackerCount() const;

    /**
      Set the reference Url.
     */
    void setReferUrl(const QString &value);
    /**
      Return the reference Url.
     */
    QString referUrl() const;

    /**
      Set the tracker text.
     */
    void setTrackerText(const QString &value);
    /**
      Return the tracker text.
     */
    QString trackerText() const;

    /**
      Set the Campaign start date.
     */
    void setStartDate(const QString &value);
    /**
      Return the Campaign start date.
     */
    QString startDate() const;

    /**
       Set the Campaign end date.
     */
    void setEndDate(const QString &value);
    /**
      Return the Campaign end date.
     */
    QString endDate() const;

    /**
      Set the Campaign status.
     */
    void setStatus(const QString &value);
    /**
      Return the Campaign Status.
     */
    QString status() const;

    /**
       Set the Campaign impressions.
     */
    void setImpressions(const QString &value);
    /**
       Return the Campaign impressions.
     */
    QString impressions() const;

    /**
      Set the Campaign currency id.
     */
    void setCurrencyId(const QString &value);
    /**
      Return the Campaign currency id.
     */
    QString currencyId() const;

    /**
      Set the Campaign budget.
     */
    void setBudget(const QString &value);
    /**
      Return the Campaign budget.
     */
    QString budget() const;

    /**
      Set the Campaign expected cost.
     */
    void setExpectedCost(const QString &value);
    /**
      Return the Campaign expected cost.
     */
    QString expectedCost() const;

    /**
      Set the Campaign actual cost .
     */
    void setActualCost(const QString &value);
    /**
      Return the Campaign actual cost.
     */
    QString actualCost() const;

    /**
      Set the Campaign expected revenue.
     */
    void setExpectedRevenue(const QString &value);
    /**
      Return the Campaign expected revenue.
     */
    QString expectedRevenue() const;

    /**
      Set the Campaign type.
     */
    void setCampaignType(const QString &value);
    /**
      Return the Campaign type.
     */
    QString campaignType() const;

    /**
      Set the Campaign objective.
     */
    void setObjective(const QString &value);
    /**
      Return the Campaign objective.
     */
    QString objective() const;

    /**
      Set the Campaign content.
     */
    void setContent(const QString &value);
    /**
      Return the Campaign content.
     */
    QString content() const;

    /**
      Set the Campaign frequency.
     */
    void setFrequency(const QString &value);
    /**
      Return Primary Address Postal code.
     */
    QString frequency() const;

    /**
      Convenience: set the data for this campaign
     */
    void setData(const QMap<QString, QString> &data);

    /**
      Convenience: retrieve the data for this campaign
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

Q_DECLARE_METATYPE(SugarCampaign)

#endif /* SUGARCAMPAIGN_H */

