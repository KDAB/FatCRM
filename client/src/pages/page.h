#ifndef PAGE_H
#define PAGE_H

#include "ui_page.h"
#include "enums.h"

#include "accountsfilterproxymodel.h"
//#include <akonadi/entitymimetypefiltermodel.h>

#include <akonadi/changerecorder.h>
#include <akonadi/collection.h>

#include <QtGui/QWidget>

namespace Akonadi
{
    class ChangeRecorder;
    class Item;
}

class KJob;
class QModelIndex;
class SugarClient;

class Page : public QWidget
{
    Q_OBJECT
public:
    explicit Page( QWidget *parent, const QString &mimeType, DetailsType type );

    ~Page();


Q_SIGNALS:
    void statusMessage( const QString& );

protected:
    void updateAccountCombo( const QString& name, const QString& id );
    void updateAssignedToCombo( const QString& name, const QString& id );
    void updateCampaignCombo( const QString& name, const QString& id );
    void updateReportToCombo( const QString& name, const QString& id );

    inline SugarClient* clientWindow() { return mClientWindow; }
    inline QString mimeType() { return mMimeType; } const
    inline Akonadi::EntityTreeView* treeView() { return mUi.treeView; }
    inline Akonadi::Collection collection() { return mCollection; }
    inline Akonadi::ChangeRecorder* recorder() { return mChangeRecorder; }
    inline QLineEdit* search() { return mUi.searchLE; }
    inline void setFilter ( QSortFilterProxyModel *filter ) { mFilter = filter; }

    virtual void addItem( const QMap<QString, QString> &data ) = 0;
    virtual void modifyItem( Akonadi::Item &item, const QMap<QString, QString> &data ) = 0;
    virtual void setupModel();

private Q_SLOTS:
    void slotResourceSelectionChanged( const QByteArray &identifier );
    void slotCollectionFetchResult( KJob *job );
    void slotItemClicked( const QModelIndex &index );
    void slotNewClicked();
    void slotAddItem();
    void slotModifyItem();
    void slotRemoveItem();
    void slotSetCurrent( const QModelIndex&,int,int );
    void cachePolicyJobCompleted( KJob* );
    void slotUpdateDetails( const QModelIndex&, const QModelIndex& );
    void slotSetItem();
    void slotResetSearch();

private:
    void initialize();
    void setupCachePolicy();
    void itemChanged(const Akonadi::Item &item );
    bool proceedIsOk();
    // manages accounts combo box
    void addAccountsData();
    void addCampaignsData();
    void addContactsData();
    void removeAccountsData( Akonadi::Item &item );
    void removeCampaignsData( Akonadi::Item &item );

    QString typeToString( const DetailsType &type ) const;


    SugarClient *mClientWindow;
    QString mMimeType;
    DetailsType mType;
    QSortFilterProxyModel *mFilter;
    Akonadi::ChangeRecorder *mChangeRecorder;
    Akonadi::Collection mCollection;
    QModelIndex mCurrentIndex;
    Ui_page mUi;
};

#endif