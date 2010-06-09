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
    void opportunityItemChanged();
    void statusMessage( const QString& );
    void showDetails();

public Q_SLOTS:
    void syncronize();

private:
    void initialize();
    void setupCachePolicy();

    Akonadi::ChangeRecorder *mChangeRecorder;
    Akonadi::Collection mOpportunitiesCollection;
    Ui_OpportunitiesPage mUi;

private Q_SLOTS:
    void slotResourceSelectionChanged( const QByteArray &identifier );
    void slotCollectionFetchResult( KJob *job );
    void slotOpportunityChanged( const Akonadi::Item &item );
    void slotNewOpportunityClicked();
    void slotAddOpportunity();
    void slotModifyOpportunity();
    void slotRemoveOpportunity();
    void slotSetCurrent( const QModelIndex&,int,int );
    void cachePolicyJobCompleted( KJob* );
};

#endif /* OPPORTUNITIESPAGE_H */

