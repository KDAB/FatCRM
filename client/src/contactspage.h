#ifndef CONTACTSPAGE_H
#define CONTACTSPAGE_H

#include "ui_contactspage.h"

#include <akonadi/collection.h>

#include <QtGui/QWidget>

namespace Akonadi
{
    class ChangeRecorder;
    class Item;
}

class KJob;
class QModelIndex;

class ContactsPage : public QWidget
{
    Q_OBJECT
public:
    explicit ContactsPage( QWidget *parent = 0 );

    ~ContactsPage();

     void addAccountsData();

Q_SIGNALS:
    void statusMessage( const QString& );

public Q_SLOTS:
    void syncronize();

private:
    void initialize();
    void setupCachePolicy();
    void contactChanged(const Akonadi::Item &item );
    bool proceedIsOk();

    Akonadi::ChangeRecorder *mChangeRecorder;
    Akonadi::Collection mContactsCollection;
    QModelIndex mCurrentIndex;
    Ui_ContactsPage mUi;

private Q_SLOTS:
    void slotResourceSelectionChanged( const QByteArray &identifier );
    void slotCollectionFetchResult( KJob *job );
    void slotContactClicked( const QModelIndex& index );
    void slotNewContactClicked();
    void slotAddContact();
    void slotModifyContact();
    void slotRemoveContact();
    void slotSetCurrent( const QModelIndex&,int,int );
    void cachePolicyJobCompleted( KJob* );
    void slotUpdateItemDetails( const QModelIndex&, const QModelIndex& );
};

#endif
