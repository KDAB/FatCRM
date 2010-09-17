#ifndef LEADSTREEMODEL_H
#define LEADSTREEMODEL_H

#include <akonadi/entitytreemodel.h>

namespace Akonadi {

/**
 * @short A model for leads.
 *
 * This class provides a model for displaying the sugar leads.
 *
 */
class LeadsTreeModel : public EntityTreeModel
{
  Q_OBJECT

  public:
    /**
     * Describes the columns that can be shown by the model.
     */
    enum Column
    {
      /**
       * Shows the Lead name.
       */
      Name,

      /**
       * Shows the Status of the Lead.
       */
      Status,

      /**
       * Shows the Account Name for the Lead
       */
      AccountName,

      /**
       * Shows the primary email for the Lead
       */
      Email,

      /**
       * Shows the Assigned User Name for the Lead
       */
      User
    };

    /**
     * Describes a list of columns of the leads tree model.
     */
    typedef QList<Column> Columns;


    /**
     * Creates a new leads tree model.
     *
     * param monitor The ChangeRecorder whose entities should be represented in the model.
     * param parent The parent object.
     */
    explicit LeadsTreeModel( ChangeRecorder *monitor, QObject *parent = 0 );

    /**
     * Destroys the leads tree model.
     */
    virtual ~LeadsTreeModel();

    /**
     * Sets the  columns that the model should show.
     */
    void setColumns( const Columns &columns );

    /**
     * Returns the columns that the model currently shows.
     */
    Columns columns() const;

    virtual QVariant entityData( const Item &item, int column, int role = Qt::DisplayRole ) const;
    virtual QVariant entityData( const Collection &collection, int column, int role = Qt::DisplayRole ) const;
    virtual QVariant entityHeaderData( int section, Qt::Orientation orientation, int role, HeaderGroup headerGroup ) const;
    virtual int entityColumnCount( HeaderGroup headerGroup ) const;

  private:
    class Private;
    Private* const d;
};

}

#endif /* LEADSTREEMODEL_H */

