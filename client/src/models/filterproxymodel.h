#ifndef FILTERPROXYMODEL_H
#define FILTERPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>
#include "enums.h"

namespace Akonadi
{

/**
 * A proxy model for sugar tree models.
 *
 * This class provides a filter proxy model for a sugar tree models.
 * The list of shown items can be limited by settings a filter pattern.
 * Only items that contain this pattern as part of their data will be
 * listed.
 *
 */
class FilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    /**
     * Creates a new filter proxy model.
     *
     * @param DetailsType the type of CRM object
     * @param parent The parent object.
     */
    explicit FilterProxyModel(DetailsType type, QObject *parent = 0);

    /**
     * Destroys the filter proxy model.
     */
    ~FilterProxyModel();

public Q_SLOTS:
    /**
     * Sets the filter that is used to filter for matching items
     */
    void setFilterString(const QString &filter);

protected:
    virtual bool filterAcceptsRow(int row, const QModelIndex &parent) const;

private:
    class Private;
    Private *const d;
};

}

#endif /* FILTERPROXYMODEL_H */

