#ifndef OPPORTUNITIESFILTERPROXYMODEL_H
#define OPPORTUNITIESFILTERPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>

namespace Akonadi {

/**
 * A proxy model for OpportunitiessTreeModel models.
 *
 * This class provides a filter proxy model for a OpportunitiessTreeModel.
 * The list of shown opportunities can be limited by settings a filter pattern.
 * Only Opportunitiess that contain this pattern as part of their data will be
 * listed.
 *
 */
class OpportunitiesFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

  public:
    /**
     * Creates a new opportunities filter proxy model.
     *
     * @param parent The parent object.
     */
    explicit OpportunitiesFilterProxyModel( QObject *parent = 0 );

    /**
     * Destroys the opportunities filter proxy model.
     */
    ~OpportunitiesFilterProxyModel();

  public Q_SLOTS:
    /**
     * Sets the filter that is used to filter for matching opportunities
     */
    void setFilterString( const QString &filter );

  protected:
    virtual bool filterAcceptsRow( int row, const QModelIndex &parent ) const;

  private:
    class Private;
    Private* const d;
};

}


#endif /* OPPORTUNITIESFILTERPROXYMODEL_H */

