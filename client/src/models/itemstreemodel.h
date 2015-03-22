#ifndef ITEMSTREEMODEL_H
#define ITEMSTREEMODEL_H

#include <akonadi/entitytreemodel.h>
#include "enums.h"

namespace Akonadi
{

/**
 * A model for sugar items.
 * This class provides a model for displaying the sugar items.
 *
 */
class ItemsTreeModel : public EntityTreeModel
{
    Q_OBJECT

public:
    /**
     * Describes the columns that can be shown by the model.
     */
    enum Column {

        Name,
        City,
        Country,
        Phone,
        Email,
        CreatedBy,
        CampaignName,
        Status,
        Type,
        EndDate,
        User,
        FullName,
        Role,
        Organization,
        PreferredEmail,
        PhoneNumber,
        GivenName,
        LeadName,
        LeadAccountName,
        LeadEmail,
        LeadStatus,
        LeadUser,
        OpportunityName,
        OpportunityAccountName,
        SalesStage,
        Amount,
        Close,
        AssignedTo
    };

    /**
     * Describes a list of columns of the items tree model.
     */
    typedef QList<Column> Columns;

    /**
     * Creates a new items tree model.
     *
     * param: monitor The ChangeRecorder whose entities should be
     * represented in the model.
     * param: parent The parent object.
     */
    explicit ItemsTreeModel(DetailsType type, ChangeRecorder *monitor, QObject *parent = 0);

    virtual ~ItemsTreeModel();

    void setColumns();
    Columns columns() const;

    /*reimp*/ QVariant entityData(const Item &item, int column, int role = Qt::DisplayRole) const;
    /*reimp*/ QVariant entityData(const Collection &collection, int column, int role = Qt::DisplayRole) const;
    /*reimp*/ QVariant entityHeaderData(int section, Qt::Orientation orientation, int role, HeaderGroup headerGroup) const;
    /*reimp*/ int entityColumnCount(HeaderGroup headerGroup) const;

private:
    QVariant accountData(const Item &item, int column, int role) const;
    QVariant campaignData(const Item &item, int column, int role) const;
    QVariant contactData(const Item &item, int column, int role) const;
    QVariant leadData(const Item &item, int column, int role) const;
    QVariant opportunityData(const Item &item, int column, int role) const;
    Columns  columnsGroup(DetailsType type) const;

private:
    class Private;
    Private *const d;
    DetailsType mType;
};
}

#endif /* ITEMSTREEMODEL_H */

