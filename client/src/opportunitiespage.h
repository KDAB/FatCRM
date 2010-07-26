#ifndef OPPORTUNITIESPAGE_H
#define OPPORTUNITIESPAGE_H

#include "ui_opportunitiespage.h"

#include <akonadi/collection.h>
#include <akonadi/entitytreemodel.h>

#include <QtGui/QWidget>

namespace Akonadi
{
    class ChangeRecorder;
    class Item;
    class ItemModel;
}

class KJob;
class QModelIndex;

class OpportunitiesPage : public QWidget
{
    Q_OBJECT
public:
    explicit OpportunitiesPage( QWidget *parent = 0 );

    ~OpportunitiesPage();

     void addOpportunitiesData();

Q_SIGNALS:
    void statusMessage( const QString& );

public Q_SLOTS:
    void syncronize();

private:
    void initialize();
    void setupCachePolicy();
    void opportunityChanged( const Akonadi::Item &item );
    bool proceedIsOk();

    Akonadi::ChangeRecorder *mChangeRecorder;
    Akonadi::Collection mOpportunitiesCollection;
    QModelIndex mCurrentIndex;
    Ui_OpportunitiesPage mUi;

private Q_SLOTS:
    void slotResourceSelectionChanged( const QByteArray &identifier );
    void slotCollectionFetchResult( KJob *job );
    void slotOpportunityClicked( const QModelIndex& index );
    void slotNewOpportunityClicked();
    void slotAddOpportunity();
    void slotModifyOpportunity();
    void slotRemoveOpportunity();
    void slotSetCurrent( const QModelIndex&,int,int );
    void cachePolicyJobCompleted( KJob* );
    void slotUpdateItemDetails( const QModelIndex&, const QModelIndex& );
};

#endif /* OPPORTUNITIESPAGE_H */

