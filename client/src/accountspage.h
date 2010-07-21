#ifndef ACCOUNTSPAGE_H
#define ACCOUNTSPAGE_H

#include "ui_accountspage.h"

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

class AccountsPage : public QWidget
{
    Q_OBJECT
public:
    explicit AccountsPage( QWidget *parent = 0 );

    ~AccountsPage();

     void addAccountsData();

Q_SIGNALS:
    void statusMessage( const QString& );

public Q_SLOTS:
    void syncronize();

private:
    void initialize();
    void setupCachePolicy();
    void removeAccountsData( const Akonadi::Item &item );

    Akonadi::ChangeRecorder *mChangeRecorder;
    Akonadi::Collection mAccountsCollection;
    Ui_AccountsPage mUi;

private Q_SLOTS:
    void slotResourceSelectionChanged( const QByteArray &identifier );
    void slotCollectionFetchResult( KJob *job );
    void slotAccountChanged( const Akonadi::Item &item );
    void slotNewAccountClicked();
    void slotAddAccount();
    void slotModifyAccount();
    void slotRemoveAccount();
    void slotSetCurrent( const QModelIndex&,int,int );
    void cachePolicyJobCompleted( KJob* );
    void slotUpdateItemDetails( const QModelIndex&, const QModelIndex& );
};
#endif /* ACCOUNTSPAGE_H */

