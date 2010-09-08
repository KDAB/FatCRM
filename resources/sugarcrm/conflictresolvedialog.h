// temporary "copy" from kdepimlibs/akonadi/conflicthandling

#ifndef CONFLICTRESOLVEDIALOG_H
#define CONFLICTRESOLVEDIALOG_H

#include "conflicthandler.h"

#include <KDialog>

class ConflictResolveDialog : public KDialog
{
  Q_OBJECT

  public:
    /**
     * Creates a new conflict resolve dialog.
     *
     * @param parent The parent widget.
     */
    explicit ConflictResolveDialog( QWidget *parent = 0 );

    ~ConflictResolveDialog();

    /**
     * Sets the items that causes the conflict.
     *
     * @param localItem The local item which causes the conflict.
     * @param otherItem The conflicting item from the Akonadi storage.
     *
     * @note Both items need the full payload set.
     */
    void setConflictingItems( const Akonadi::Item &localItem, const Akonadi::Item &otherItem );

    void setDifferencesInterface( Akonadi::DifferencesAlgorithmInterface *interface );

    /**
     * Returns the resolve strategy the user choose.
     */
    ConflictHandler::ResolveStrategy resolveStrategy() const;

private:
    class Private;
    Private *const d;
};

#endif
