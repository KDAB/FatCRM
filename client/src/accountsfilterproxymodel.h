#ifndef ACCOUNTSFILTERPROXYMODEL_H
#define ACCOUNTSFILTERPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>

namespace Akonadi {

/**
 * A proxy model for AccountsTreeModel models.
 *
 * This class provides a filter proxy model for a AccountsTreeModel.
 * The list of shown accounts can be limited by settings a filter pattern.
 * Only accounts that contain this pattern as part of their data will be
 * listed.
 *
 */
class AccountsFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

  public:
    /**
     * Creates a new accounts filter proxy model.
     *
     * @param parent The parent object.
     */
    explicit AccountsFilterProxyModel( QObject *parent = 0 );

    /**
     * Destroys the accounts filter proxy model.
     */
    ~AccountsFilterProxyModel();

  public Q_SLOTS:
    /**
     * Sets the filter that is used to filter for matching accounts
     */
    void setFilterString( const QString &filter );

  protected:
    virtual bool filterAcceptsRow( int row, const QModelIndex &parent ) const;

  private:
    class Private;
    Private* const d;
};

}

#endif /* ACCOUNTSFILTERPROXYMODEL_H */

