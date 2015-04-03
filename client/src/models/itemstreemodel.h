#ifndef ITEMSTREEMODEL_H
#define ITEMSTREEMODEL_H

#include <akonadi/entitytreemodel.h>
#include <enums.h>

/**
 * A model for sugar items.
 * This class provides a model for displaying the sugar items.
 *
 */
class ItemsTreeModel : public Akonadi::EntityTreeModel
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
        NextStepDate,
        LastModifiedDate,
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
    explicit ItemsTreeModel(DetailsType type, Akonadi::ChangeRecorder *monitor, QObject *parent = 0);

    virtual ~ItemsTreeModel();

    void setColumns();
    Columns columns() const;

    /*reimp*/ QVariant entityData(const Akonadi::Item &item, int column, int role = Qt::DisplayRole) const;
    /*reimp*/ QVariant entityData(const Akonadi::Collection &collection, int column, int role = Qt::DisplayRole) const;
    /*reimp*/ QVariant entityHeaderData(int section, Qt::Orientation orientation, int role, HeaderGroup headerGroup) const;
    /*reimp*/ int entityColumnCount(HeaderGroup headerGroup) const;

private:
    QVariant accountData(const Akonadi::Item &item, int column, int role) const;
    QVariant campaignData(const Akonadi::Item &item, int column, int role) const;
    QVariant contactData(const Akonadi::Item &item, int column, int role) const;
    QVariant leadData(const Akonadi::Item &item, int column, int role) const;
    QVariant opportunityData(const Akonadi::Item &item, int column, int role) const;
    Columns  columnsGroup(DetailsType type) const;

private:
    class Private;
    Private *const d;
    DetailsType mType;
};

#endif /* ITEMSTREEMODEL_H */

