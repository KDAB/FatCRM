#ifndef CAMPAIGNSFILTERPROXYMODEL_H
#define CAMPAIGNSFILTERPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>

namespace Akonadi {

/**
 * A proxy model for CampaignsTreeModel models.
 *
 * This class provides a filter proxy model for a CampaignsTreeModel.
 * The list of shown campaigns can be limited by settings a filter pattern.
 * Only campaigns that contain this pattern as part of their data will be
 * listed.
 *
 */
class CampaignsFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

  public:
    /**
     * Creates a new campaigns filter proxy model.
     *
     * @param parent The parent object.
     */
    explicit CampaignsFilterProxyModel( QObject *parent = 0 );

    /**
     * Destroys the campaigns filter proxy model.
     */
    ~CampaignsFilterProxyModel();

  public Q_SLOTS:
    /**
     * Sets the filter that is used to filter for matching campaigns
     */
    void setFilterString( const QString &filter );

  protected:
    virtual bool filterAcceptsRow( int row, const QModelIndex &parent ) const;

  private:
    class Private;
    Private* const d;
};

}


#endif /* CAMPAIGNSFILTERPROXYMODEL_H */

