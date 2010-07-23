#ifndef CAMPAIGNSPAGE_H
#define CAMPAIGNSPAGE_H

#include "ui_campaignspage.h"

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

class CampaignsPage : public QWidget
{
    Q_OBJECT
public:
    explicit CampaignsPage( QWidget *parent = 0 );

    ~CampaignsPage();

     void addCampaignsData();

Q_SIGNALS:
    void statusMessage( const QString& );

public Q_SLOTS:
    void syncronize();

private:
    void initialize();
    void setupCachePolicy();
    void removeCampaignsData( const Akonadi::Item &item );
    bool proceedIsOk();

    Akonadi::ChangeRecorder *mChangeRecorder;
    Akonadi::Collection mCampaignsCollection;
    QModelIndex mCurrentIndex;
    Ui_CampaignsPage mUi;

private Q_SLOTS:
    void slotResourceSelectionChanged( const QByteArray &identifier );
    void slotCollectionFetchResult( KJob *job );
    void slotCampaignChanged( const Akonadi::Item &item );
    void slotNewCampaignClicked();
    void slotAddCampaign();
    void slotModifyCampaign();
    void slotRemoveCampaign();
    void slotSetCurrent( const QModelIndex&,int,int );
    void cachePolicyJobCompleted( KJob* );
    void slotUpdateItemDetails( const QModelIndex&, const QModelIndex& );
};

#endif /* CAMPAIGNSPAGE_H */

