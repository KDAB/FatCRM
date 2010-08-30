#ifndef PAGE_H
#define PAGE_H

#include "ui_page.h"
#include "kdcrmdata/sugaraccount.h"
#include "enums.h"

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

class Page : public QWidget
{
    Q_OBJECT
public:
    explicit Page( QWidget *parent, QString mimeType, QString type );

    ~Page();


Q_SIGNALS:
    void statusMessage( const QString& );

public Q_SLOTS:
    void syncronize();

protected:
    inline QString mimeType() const { return mMimeType; }
    inline QString type() const { return mType; }
    inline Akonadi::EntityTreeView* treeView() { return mUi.treeView; }
    inline Akonadi::Collection collection() { return mCollection; }
    inline Akonadi::ChangeRecorder* recorder() { return mChangeRecorder; }
    inline QLineEdit* search() { return mUi.searchLE; }

    virtual void addItem() = 0;
    virtual void modifyItem() = 0;
    virtual void addAccountsData() = 0;
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
    DetailsType typeToDetailsType( const QString &type ) const;


    QString mMimeType;
    QString mType;
    Akonadi::ChangeRecorder *mChangeRecorder;
    Akonadi::Collection mCollection;
    QModelIndex mCurrentIndex;
    DetailsType mDetailsType;
    Ui_page mUi;
};

#endif
