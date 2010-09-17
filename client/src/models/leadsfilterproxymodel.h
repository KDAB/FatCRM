#ifndef LEADSFILTERPROXYMODEL_H
#define LEADSFILTERPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>

namespace Akonadi {

/**
 * A proxy model for LeadsTreeModel models.
 *
 * This class provides a filter proxy model for a LeadsTreeModel.
 * The list of shown leads can be limited by settings a filter pattern.
 * Only leads that contain this pattern as part of their data will be
 * listed.
 *
 */
class LeadsFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

  public:
    /**
     * Creates a new leads filter proxy model.
     *
     * @param parent The parent object.
     */
    explicit LeadsFilterProxyModel( QObject *parent = 0 );

    /**
     * Destroys the leads filter proxy model.
     */
    ~LeadsFilterProxyModel();

  public Q_SLOTS:
    /**
     * Sets the filter that is used to filter for matching leads
     */
    void setFilterString( const QString &filter );

  protected:
    virtual bool filterAcceptsRow( int row, const QModelIndex &parent ) const;

  private:
    class Private;
    Private* const d;
};

}


#endif /* LEADSFILTERPROXYMODEL_H */

