#ifndef OPPORTUNITIESTREEMODEL_H
#define OPPORTUNITIESTREEMODEL_H

#include <akonadi/entitytreemodel.h>

namespace Akonadi {

/**
 * @short A model for Sugar Opportunities objects.
 *
 * This class provides a model for displaying the sugar opportunites.
 *
 */
class OpportunitiesTreeModel : public EntityTreeModel
{
  Q_OBJECT

  public:
    /**
     * Describes the columns that can be shown by the model.
     */
    enum Column
    {
      /**
       * Shows the Name of the Opportunity.
       */
      Name,

      /**
       * Shows the Account Name for the Opportunity.
       */
      AccountName,

      /**
       * Shows the Sales Stage
       */
      SalesStage,

      /**
       * Shows the Amount.
       */
      Amount,

      /**
       * Shows the Closed Date
       */
      Close,

      /**
       * Shows the Assigned User Name
       */
      AssignedTo
    };

    /**
     * Describes a list of columns of the accounts tree model.
     */
    typedef QList<Column> Columns;


    /**
     * Creates a new accounts tree model.
     *
     * param monitor The ChangeRecorder whose entities should be represented in the model.
     * param parent The parent object.
     */
    explicit OpportunitiesTreeModel( ChangeRecorder *monitor, QObject *parent = 0 );

    /**
     * Destroys the accounts tree model.
     */
    virtual ~OpportunitiesTreeModel();

    /**
     * Sets the columns that the model should show.
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

#endif /* OPPORTUNITIESTREEMODEL_H */

