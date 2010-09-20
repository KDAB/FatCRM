#ifndef CONTACTSFILTERPROXYMODEL_H
#define CONTACTSFILTERPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>

namespace Akonadi {

/**
 * A proxy model for ContactsTreeModel models.
 *
 * This class provides a filter proxy model for a ContactsTreeModel.
 * The list of shown contacts can be limited by settings a filter pattern.
 * Only contacts that contain this pattern as part of their data will be
 * listed.
 *
 */
class ContactsFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

  public:
    /**
     * Creates a new contacts filter proxy model.
     *
     * @param parent The parent object.
     */
    explicit ContactsFilterProxyModel( QObject *parent = 0 );

    /**
     * Destroys the contacts filter proxy model.
     */
    ~ContactsFilterProxyModel();

  public Q_SLOTS:
    /**
     * Sets the filter that is used to filter for matching contacts
     */
    void setFilterString( const QString &filter );

  protected:
    virtual bool filterAcceptsRow( int row, const QModelIndex &parent ) const;

  private:
    class Private;
    Private* const d;
};

}

#endif /* CONTACTSFILTERPROXYMODEL_H */

