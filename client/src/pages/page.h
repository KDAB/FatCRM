#ifndef PAGE_H
#define PAGE_H

#include "ui_page.h"
#include "enums.h"
#include "itemstreemodel.h"
#include "filterproxymodel.h"

#include <akonadi/changerecorder.h>
#include <akonadi/collection.h>

#include <QtGui/QWidget>

namespace Akonadi
{
class ChangeRecorder;
class Item;
}

class Details;
class DetailsDialog;
class DetailsWidget;
class KJob;
class QAction;
class QModelIndex;
class SugarClient;

class Page : public QWidget
{
    Q_OBJECT
public:
    explicit Page(QWidget *parent, const QString &mimeType, DetailsType type);

    ~Page();

    QAction *showDetailsAction(const QString &title) const;

    bool showsDetails() const;
    void printReport();

Q_SIGNALS:
    void statusMessage(const QString &);
    void showDetailsChanged(bool on);
    void modelItemChanged(const Akonadi::Item &item);
    void synchronizeCollection(const Akonadi::Collection &collection);

public Q_SLOTS:
    void showDetails(bool on);

protected:
    inline SugarClient *clientWindow()
    {
        return mClientWindow;
    }
    inline QString mimeType() const
    {
        return mMimeType;
    }
    inline Akonadi::EntityTreeView *treeView()
    {
        return mUi.treeView;
    }
    inline Akonadi::Collection collection()
    {
        return mCollection;
    }
    inline Akonadi::ChangeRecorder *recorder()
    {
        return mChangeRecorder;
    }
    inline QLineEdit *search()
    {
        return mUi.searchLE;
    }
    void setFilter(Akonadi::FilterProxyModel *filter)
    {
        mFilter = filter;
    }

    virtual void setupModel();

    Details *details() const;

    void insertFilterWidget(QWidget *widget);

private Q_SLOTS:
    void slotResourceSelectionChanged(const QByteArray &identifier);
    void slotCollectionFetchResult(KJob *job);
    void slotCurrentItemChanged(const QModelIndex &index);
    void slotNewClicked();
    void slotAddItem();
    void slotModifyItem();
    void slotRemoveItem();
    void slotRowsInserted(const QModelIndex &, int, int);
    void slotDataChanged(const QModelIndex &, const QModelIndex &);
    void slotSetItem();
    void slotResetSearch();
    void slotReloadCollection();
    void slotCollectionChanged(const Akonadi::Collection &collection);
    void slotEnsureDetailsVisible();
    void slotItemDoubleClicked(const QModelIndex &);
    void slotCreateJobResult(KJob *job);
    void slotModifyJobResult(KJob *job);

private:
    virtual QString reportTitle() const = 0;
    void initialize();
    bool askSave();
    // manages accounts combo box
    void addAccountsData();
    void addCampaignsData();
    void addContactsData();
    void addLeadsData();
    void addOpportunitiesData();
    void removeAccountsData(Akonadi::Item &item);
    void removeCampaignsData(Akonadi::Item &item);

    QString typeToString(const DetailsType &type) const;
    DetailsDialog *createDetailsDialog();

private:
    SugarClient *mClientWindow;
    QString mMimeType;
    DetailsType mType;
    DetailsWidget *mDetailsWidget;
    Akonadi::FilterProxyModel *mFilter;
    Akonadi::ChangeRecorder *mChangeRecorder;
    Akonadi::Collection mCollection;
    QModelIndex mCurrentIndex;
    Ui_page mUi;
    QAction *mShowDetailsAction;
    QByteArray mResourceIdentifier;
};

#endif
