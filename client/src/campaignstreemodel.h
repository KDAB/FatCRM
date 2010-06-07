#ifndef CAMPAIGNSTREEMODEL_H
#define CAMPAIGNSTREEMODEL_H

#include <akonadi/entitytreemodel.h>

namespace Akonadi {

/**
 * @short A model for campaigns.
 *
 * This class provides a model for displaying the sugar campaigns.
 *
 */
class CampaignsTreeModel : public EntityTreeModel
{
  Q_OBJECT

  public:
    /**
     * Describes the columns that can be shown by the model.
     */
    enum Column
    {
      /**
       * Shows the Campaign name.
       */
      Campaign,

      /**
       * Shows the Campaign status.
       */
      Status,

      /**
       * Shows the Campaign type.
       */
      Type,

      /**
       * Shows the End date for the campaign.
       */
      EndDate,

      /**
       * Shows the Campaign assigned user name
       */
      User
    };

    /**
     * Describes a list of columns of the campaigns tree model.
     */
    typedef QList<Column> Columns;


    /**
     * Creates a new campaigns tree model.
     *
     * @param monitor The ChangeRecorder whose entities should be represented in the model.
     * @param parent The parent object.
     */
    explicit CampaignsTreeModel( ChangeRecorder *monitor, QObject *parent = 0 );

    /**
     * Destroys the campaigns tree model.
     */
    virtual ~CampaignsTreeModel();

    /**
     * Sets the columns that the model should show.
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

#endif /* CAMPAIGNSTREEMODEL_H */

