#ifndef ACCOUNTSTREEMODEL_H
#define ACCOUNTSTREEMODEL_H

#include <akonadi/entitytreemodel.h>

namespace Akonadi {

/**
 * @short A model for accounts.
 *
 * This class provides a model for displaying the sugar accounts.
 *
 */
class AccountsTreeModel : public EntityTreeModel
{
  Q_OBJECT

  public:
    /**
     * Describes the columns that can be shown by the model.
     */
    enum Column
    {
      /**
       * Shows the Account name.
       */
      Name,

      /**
       * Shows the Billing Address City for the Account.
       */
      City,

      /**
       * Shows the Billing Address Country for the Account
       */
      Country,

      /**
       * Shows the Office phone.
       */
      Phone,

      /**
       * Shows the primary email for the Account
       */
      Email,

      /**
       * Shows the Created By User Name
       */
      CreatedBy
    };

    /**
     * Describes a list of columns of the accounts tree model.
     */
    typedef QList<Column> Columns;


    /**
     * Creates a new accounts tree model.
     *
     * @param monitor The ChangeRecorder whose entities should be represented in the model.
     * @param parent The parent object.
     */
    explicit AccountsTreeModel( ChangeRecorder *monitor, QObject *parent = 0 );

    /**
     * Destroys the accounts tree model.
     */
    virtual ~AccountsTreeModel();

    /**
     * Sets the @p columns that the model should show.
     */
    void setColumns( const Columns &columns );

    /**
     * Returns the columns that the model currently shows.
     */
    Columns columns() const;

    //@cond PRIVATE
    virtual QVariant entityData( const Item &item, int column, int role = Qt::DisplayRole ) const;
    virtual QVariant entityData( const Collection &collection, int column, int role = Qt::DisplayRole ) const;
    virtual QVariant entityHeaderData( int section, Qt::Orientation orientation, int role, HeaderGroup headerGroup ) const;
    virtual int entityColumnCount( HeaderGroup headerGroup ) const;
    //@endcond

  private:
    //@cond PRIVATE
    class Private;
    Private* const d;
    //@endcond
};

}
#endif /* ACCOUNTSTREEMODEL_H */

