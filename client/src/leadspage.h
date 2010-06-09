#ifndef LEADSPAGE_H
#define LEADSPAGE_H

#include "ui_leadspage.h"

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

class LeadsPage : public QWidget
{
    Q_OBJECT
public:
    explicit LeadsPage( QWidget *parent = 0 );

    ~LeadsPage();

    void addLeadsData();

Q_SIGNALS:
    void leadItemChanged();
    void statusMessage( const QString& );
    void showDetails();

public Q_SLOTS:
    void syncronize();

private:
    void initialize();
    void setupCachePolicy();

    Akonadi::ChangeRecorder *mChangeRecorder;
    Akonadi::Collection mLeadsCollection;
    Ui_LeadsPage mUi;

private Q_SLOTS:
    void slotResourceSelectionChanged( const QByteArray &identifier );
    void slotCollectionFetchResult( KJob *job );
    void slotLeadChanged( const Akonadi::Item &item );
    void slotNewLeadClicked();
    void slotAddLead();
    void slotModifyLead();
    void slotRemoveLead();
    void slotSetCurrent( const QModelIndex&,int,int );
    void cachePolicyJobCompleted( KJob* );
};


#endif /* LEADSPAGE_H */

