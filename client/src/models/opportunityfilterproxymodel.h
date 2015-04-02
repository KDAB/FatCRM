#ifndef OPPORTUNITYFILTERPROXYMODEL_H
#define OPPORTUNITYFILTERPROXYMODEL_H

#include "filterproxymodel.h"

/**
 * A proxy model for sugar tree models.
 *
 * This class provides a filter proxy model for a sugar tree models.
 * The list of shown items can be limited by settings a filter pattern.
 * Only items that contain this pattern as part of their data will be
 * listed.
 *
 */
class OpportunityFilterProxyModel : public Akonadi::FilterProxyModel
{
    Q_OBJECT

public:
    /**
     * Creates a new filter proxy model.
     *
     * @param parent The parent object.
     */
    explicit OpportunityFilterProxyModel(QObject *parent = 0);

    /**
     * Destroys the filter proxy model.
     */
    ~OpportunityFilterProxyModel();

    void setFilter(const QStringList &assignees, const QDate &maxDate);
public Q_SLOTS:
    void showAll();

protected:
    virtual bool filterAcceptsRow(int row, const QModelIndex &parent) const;

private:
    class Private;
    Private *const d;
};

#endif

