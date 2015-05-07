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
    enum ColumnType {
        Name,
        City,
        Country,
        Phone,
        Email,
        CreationDate,
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

    Q_ENUMS(ColumnType)

    /**
     * Describes a list of columns of the items tree model.
     */
    typedef QList<ColumnType> ColumnTypes;

    /**
     * Creates a new items tree model.
     *
     * param: monitor The ChangeRecorder whose entities should be
     * represented in the model.
     * param: parent The parent object.
     */
    explicit ItemsTreeModel(DetailsType type, Akonadi::ChangeRecorder *monitor, QObject *parent = 0);

    virtual ~ItemsTreeModel();

    ColumnTypes columnTypes() const; // all available column types
    ColumnTypes defaultVisibleColumns() const; // the column types that should be initially visible
    QString columnName(int column) const;

    static QString columnNameFromType(ColumnType col);
    static ColumnType columnTypeFromName(const QString &name);
    static ColumnTypes columnTypes(DetailsType type);

    QVariant entityData(const Akonadi::Item &item, int column, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant entityData(const Akonadi::Collection &collection, int column, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant entityHeaderData(int section, Qt::Orientation orientation, int role, HeaderGroup headerGroup) const Q_DECL_OVERRIDE;
    int entityColumnCount(HeaderGroup headerGroup) const Q_DECL_OVERRIDE;

private:
    QVariant accountData(const Akonadi::Item &item, int column, int role) const;
    QVariant campaignData(const Akonadi::Item &item, int column, int role) const;
    QVariant contactData(const Akonadi::Item &item, int column, int role) const;
    QVariant leadData(const Akonadi::Item &item, int column, int role) const;
    QVariant opportunityData(const Akonadi::Item &item, int column, int role) const;
    QString columnTitle(ColumnType col) const;

private:
    class Private;
    Private *const d;
    DetailsType mType;
};

#endif /* ITEMSTREEMODEL_H */

