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
class OpportunityFilterProxyModel : public FilterProxyModel
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

    /**
     * Filter by assignees OR by country.
     * And with a possible max date.
     * And either open, or closed, or both.
     */
    void setFilter(const QStringList &assignees, const QStringList &countries, const QDate &maxDate,
                   bool showOpen, bool showClosed);

protected:
    virtual bool filterAcceptsRow(int row, const QModelIndex &parent) const Q_DECL_OVERRIDE;
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE;

private:
    class Private;
    Private *const d;
};

#endif

