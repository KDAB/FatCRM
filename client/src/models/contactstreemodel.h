#ifndef CONTACTSTREEMODEL_H
#define CONTACTSTREEMODEL_H

#include <akonadi/entitytreemodel.h>

namespace Akonadi {

/**
 * @short A model for contacts.
 *
 * This class provides a model for displaying the sugar contacts.
 *
 */
class ContactsTreeModel : public EntityTreeModel
{
  Q_OBJECT

  public:
    /**
     * Describes the columns that can be shown by the model.
     */
    enum Column
    {
      /**
       * Shows the Contact full name.
       */
      FullName,

      /**
       * Shows the role for this Contact.
       */
      Role,

      /**
       * Shows the Contact's company ( organization)
       */
      Organization,

      /**
       * Shows the contact's primary email.
       */
      PreferredEmail,

      /**
       * Shows the work phone number for the Contact
       */
      PhoneNumber,

      /**
       * Shows the contacts given name
       */
      GivenName
    };

    /**
     * Describes a list of columns of the contacts tree model.
     */
    typedef QList<Column> Columns;


    /**
     * Creates a new contacts tree model.
     *
     * @param monitor The ChangeRecorder whose entities should be represented in the model.
     * @param parent The parent object.
     */
    explicit ContactsTreeModel( ChangeRecorder *monitor, QObject *parent = 0 );

    /**
     * Destroys the contacts tree model.
     */
    virtual ~ContactsTreeModel();

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

#endif /* CONTACTSTREEMODEL_H */

